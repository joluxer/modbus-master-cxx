/*
 * ModbusAbstractTcpMaster.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusAbstractTcpMaster.h"

#include <cassert>

namespace Modbus
{

AbstractTcpMaster::AbstractTcpMaster(AbstractTimer& masterTimer)
: AbstractMaster(masterTimer),
  serverPort{DefaultTcpPort}, numberMaxOfClientTransaction{16},
  txnCounter{0}
{
  PT_INIT(&rxState);
}

//AbstractTcpMaster::~AbstractTcpMaster()
//{
//  // Auto-generated destructor stub
//}

void AbstractTcpMaster::disconnectTxns()
{
  shutdownTcpConnection();

  if (runningTxn)
  {
    runningTxn->setResultCode(SlaveDisconnectInTransfer);
    completedList.push(std::move(runningTxn));
  }

  while (pendingList)
  {
    auto txn = pendingList.pop();
    txn->setResultCode(SlaveDisconnectInTransfer);
    completedList.push(std::move(txn));
  }
}

bool AbstractTcpMaster::startTx()
{
  bool success = false;

  do  // just a breakable block to avoid goto
  {
    if (!tcpIsConnected())
    {
      // einen Versuch unternehmen zu Verbinden
      if (not setupTcpConnection())
      {
        runningTxn->setResultCode(TcpDestinationNotReachable);
        completedList.push(std::move(runningTxn));
        break;
      }
    }

    // Anzahl der "fliegenden" Anfragen prüfen
    if (pendingList.length() >= numberMaxOfClientTransaction)
    {
      runningTxn->setResultCode(TooManyPendingRequests);
      completedList.push(std::move(runningTxn));
      break;
    }

    auto txLen = runningTxn->getTxLength();
    if (txLen > getTcpTxSpace())
    {
      runningTxn->setResultCode(NoTxBufferSpace);
      completedList.push(std::move(runningTxn));
      break;
    }

    // transaction identifier
    ++txnCounter;
    mbaphdTxBuffer[0] = txnCounter >> 8;
    mbaphdTxBuffer[1] = txnCounter >> 0;
    // Protocol ID
    mbaphdTxBuffer[2] = 0;
    mbaphdTxBuffer[3] = 0;
    // number of following bytes
    uint16_t numOfBytes = txLen + 1; // SlaveID wird vorangestellt
    mbaphdTxBuffer[4] = numOfBytes >> 8;
    mbaphdTxBuffer[5] = numOfBytes >> 0;
    // slave ID
    mbaphdTxBuffer[6] = runningTxn->getSlaveId();
    // put header to TCP packet
    queueTcpTxData(mbaphdTxBuffer, sizeof(mbaphdTxBuffer));
    unsigned offset = 0;
    do
    {
      offset += queueTcpTxData(&mbaphdTxBuffer[offset], sizeof(mbaphdTxBuffer) - offset);
    } while (sizeof(mbaphdTxBuffer) > offset);

    // put PDU to TCP packet
    offset = 0;
    bool complete = false;
    PduConstDataBuffer tx;
    do
    {
      complete = runningTxn->getTxData(tx, offset);
      offset += queueTcpTxData(tx.data, tx.length);
    } while (not complete);

    startTcpTx();
    pendingList.push(std::move(runningTxn));
    success = true;
  } while (false);

  return success;
}

bool AbstractTcpMaster::runTx()
{
  bool busy = false;
  if (pendingList && not tcpIsConnected())
  {
    do
    {
      auto txn = pendingList.pop();
      txn->setResultCode(SlaveDisconnectInTransfer);
      completedList.push(std::move(txn));
    } while (pendingList);
  }

  return busy;
}

PT_THREAD(AbstractTcpMaster::rxEngine())
{
  unsigned oldOffset = ~0;
  uint16_t txnId;
  struct pt *pt = &rxState;
  unsigned rxLen = getTcpRxLengthAvailable();

  PT_BEGIN(pt);

  if (!pendingList)
  {
    if (rxLen)
      readTcpRxData(&rxBuffer[0], sizeof(rxBuffer));  // wenn wir keine wartenden Transactionen haben, werden alle RX/Noise Daten verworfen
    PT_EXIT(pt);  // wir nehmen PT_EXIT() hier, weil dann der Scheduler kein BUSY meldet
  }

  assert(!rxTxn);

  // auf Header warten
  PT_WAIT_UNTIL(pt, sizeof(mbaphdRxBuffer) <= rxLen);

  rxOffset = 0;
  // Header einlesen
  do
  {
    PT_WAIT_WHILE(pt, !rxLen && tcpIsConnected());
    if (!rxLen)
      PT_EXIT(pt);
    rxOffset += readTcpRxData(&mbaphdRxBuffer[rxOffset], sizeof(mbaphdRxBuffer) - rxOffset);
  } while (rxOffset < expectedRxLen);

  if (mbaphdRxBuffer[2] || mbaphdRxBuffer[3])
    PT_EXIT(pt);  // falsches Protokoll, eingelesene Daten verwerfen

  // Header auswerten, RX-Länge bestimmen
  expectedRxLen = (mbaphdRxBuffer[4] << 8) | (mbaphdRxBuffer[5] << 0);

  if (sizeof(rxBuffer) < expectedRxLen)
    PT_EXIT(pt);  // empfangene Daten verwerfen

  rxOffset = 0;
  // RX-Daten einlesen
  do
  {
    PT_WAIT_WHILE(pt, !rxLen && tcpIsConnected());
    if (!rxLen)
      PT_EXIT(pt);
    rxOffset += readTcpRxData(&rxBuffer[rxOffset], expectedRxLen - rxOffset);
  } while (rxOffset < expectedRxLen);

  // TXN ID auswerten, wartende TXNs durchsuchen
  txnId = (mbaphdRxBuffer[0] << 8) | (mbaphdRxBuffer[1] << 0);
  rxTxn = pendingList.pullBy(txnId);

  if (rxTxn->getSlaveId() != mbaphdRxBuffer[6])
  {
    rxTxn->setResultCode(ReceivedSlaveIdDoesNotMatch);
    completedList.push(std::move(rxTxn));
    PT_EXIT(pt);
  }

  // RX-Daten verarbeiten lassen
  rxOffset = 0;
  if (not handledAsException(PduConstDataBuffer{&rxBuffer[rxOffset], uint8_t(expectedRxLen - rxOffset)}))
  {
    rxTxn->setResultCode(NoError); // if RX processing finds any error, the result code is overwritten
    do
    {
      oldOffset = rxOffset;
      rxOffset += rxTxn->processRxData(PduConstDataBuffer{&rxBuffer[rxOffset], uint8_t(expectedRxLen - rxOffset)});
      if (oldOffset == rxOffset)
        PT_YIELD(pt);
    } while (rxOffset < expectedRxLen);
  }

  // TXN ist fertig bearbeitet
  completedList.push(std::move(rxTxn));

  PT_END(pt);

  return 0;
}

bool AbstractTcpMaster::runRx()
{
  bool busy = PT_SCHEDULE(rxEngine());

  return busy;
}

} /* namespace Modbus */
