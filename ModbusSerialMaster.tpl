/*
 * ModbusSerialMaster.cpp
 *
 *  Created on: 23.01.2020
 *      Author: lode
 */

#include "ModbusSerialMaster.h"

#include <cassert>

namespace Modbus
{

template<typename EncoderPolicy>
SerialMaster<EncoderPolicy>::SerialMaster(AbstractTimer& masterTimer, AbstractTimer& txGuardTimer, AbstractTimer& rxSplitTimer)
: AbstractMaster(masterTimer),
  serialLine(nullptr),
  rs485TxEnable(nullptr),
  enableTransmitter(nullptr),
  enableReceiver(nullptr),
  txGuardTimer(txGuardTimer),
  rxSplitTimer(rxSplitTimer),
  txGuardTime_ms(0),
  rxSplitLimit_ms(15),
  rxFill(0),
  txLength(0), txDoneCount(0),
  doTx(false), doRx(false), rxFrameIsBad(false)
{}

//template<typename EncoderPolicy>
//SerialMaster<EncoderPolicy>::~SerialMaster()
//{
//  // Auto-generated destructor stub
//}

template<typename EncoderPolicy>
SerialMaster<EncoderPolicy>& SerialMaster<EncoderPolicy>::setTransportChannel(ByteStream* line, IoPin* rs485TxEnable)
{
  this->serialLine = line;
  this->rs485TxEnable = rs485TxEnable;

  return *this;
}

template<typename EncoderPolicy>
SerialMaster<EncoderPolicy>& SerialMaster<EncoderPolicy>::setTimeoutsForBaudrate(uint32_t baudrate)
{
  txGuardTime_ms = lineCodec.getTxGuardTime_ms(baudrate);
  rxSplitLimit_ms = lineCodec.getSplitLimit_ms(baudrate);

  return *this;
}

template<typename EncoderPolicy>
SerialMaster<EncoderPolicy>& SerialMaster<EncoderPolicy>::setTimeouts_ms(uint32_t txGuardTime_ms, uint32_t rxSplitLimit_ms)
{
  this->txGuardTime_ms = txGuardTime_ms;
  this->rxSplitLimit_ms = rxSplitLimit_ms;

  return *this;
}

template<typename EncoderPolicy>
bool SerialMaster<EncoderPolicy>::startTx()
{
  if (serialLine and runningTxn)
  {
    txLength = runningTxn->getTxLength();

    if (not txGuardTimer.isActive())
      txGuardTimer.start(txGuardTime_ms);

    PT_INIT(&txPt);
    doTx = true;

    return true;
  }

  runningTxn->setResultCode(UnknownCommunicationError);

  completedList.push(std::move(runningTxn));

  return false;
}

template<typename EncoderPolicy>
PT_THREAD(SerialMaster<EncoderPolicy>::txRunner())
{
  register struct pt* pt = &txPt;
  register unsigned txCount = 0;

  PT_BEGIN(pt);

  PT_WAIT_WHILE(pt, txGuardTimer.isActive());

  if (rs485TxEnable)
    rs485TxEnable->activate();

  if (enableTransmitter)
    enableTransmitter->send(true);

  txDoneCount = lineCodec.startTx(serialLine, runningTxn);

  while (txLength > txDoneCount)
  {
    txCount = lineCodec.runTx(serialLine, runningTxn, txDoneCount);
    txDoneCount += txCount;
    if (not txCount)
      PT_YIELD(pt);
  }

  PT_WAIT_WHILE(pt, lineCodec.sendChecksum(serialLine));

  if (enableTransmitter)
    enableTransmitter->send(false);

 if (rs485TxEnable)
    rs485TxEnable->deactivate();

  txGuardTimer.start(txGuardTime_ms);

  if (runningTxn->getSlaveId() != 0)
  {
    // go on with reception, this is no broadcast
    PT_INIT(&rxPt);

    doRx = true;

    if (enableReceiver)
      enableReceiver->send(true);
  }

  PT_END(pt);
}

template<typename EncoderPolicy>
bool SerialMaster<EncoderPolicy>::runTx()
{
  if (doTx)
    doTx = !!PT_SCHEDULE(txRunner());

  return doTx;
}

template<typename EncoderPolicy>
bool SerialMaster<EncoderPolicy>::discardToEndOfFrame()
{
  bool eof = not txGuardTimer.isActive();
  uint8_t dummy[8];

  if (0 != serialLine->readBlock(dummy, sizeof(dummy)))
    rxFrameIsBad = true;

  return eof;
}

template<typename EncoderPolicy>
PT_THREAD(SerialMaster<EncoderPolicy>::rxRunner())
{
  register struct pt* pt = &rxPt;
  register unsigned rxCount = 0;

  PT_BEGIN(pt);

  rxFrameIsBad = false;
  rxFill = 0;
  lineCodec.startRx();

  // wait for first byte(s)
  assert(sizeof(rxBuffer) >= rxFill);
  PT_YIELD_UNTIL(pt, rxCount = lineCodec.runRx(serialLine, rxBuffer + rxFill, sizeof(rxBuffer) - rxFill));
  rxFill += rxCount;

  txGuardTimer.start(txGuardTime_ms);
  rxSplitTimer.start(rxSplitLimit_ms);

  // watch for split frame errors
  while (rxSplitTimer.isActive() and not lineCodec.isEndOfRxFrame())
  {
    if (sizeof(rxBuffer) <= rxFill)
      break;

    // give up CPU while more bytes come in
    PT_YIELD(pt);

    // read regular input
    assert(sizeof(rxBuffer) >= rxFill);
    rxCount = lineCodec.runRx(serialLine, rxBuffer + rxFill, sizeof(rxBuffer) - rxFill);
    rxFill += rxCount;

    if (!!rxCount)
    {
      txGuardTimer.start(txGuardTime_ms);
      rxSplitTimer.start(rxSplitLimit_ms);
    }

    // ignore Byte frame errors: those are hidden inside checksum errors
    // watch for frame restarts
    if (lineCodec.restartRxFrame())
      PT_RESTART(pt);
  }

  // end-of-frame detection
  // discard characters until end-of-frame detection
  PT_YIELD_UNTIL(pt, discardToEndOfFrame());

  // Frame is over by frame limit timeout

  if (not lineCodec.testChecksumIsGood(rxBuffer, rxFill))
    PT_RESTART(pt);

  // check for right slave ID in response
  if (runningTxn->getSlaveId() != rxBuffer[0])
  {
    runningTxn->setResultCode(ReceivedSlaveIdDoesNotMatch);
    PT_RESTART(pt);
  }

  runningTxn->setResultCode(NoError); // if RX processing finds any error, the result code is overwritten
  rxCount = runningTxn->processRxData(PduConstDataBuffer{rxBuffer + 1, uint8_t(rxFill - 1)});

  if (enableReceiver)
    enableReceiver->send(false);

  completedList.push(std::move(runningTxn));

  PT_END(pt);
}

template<typename EncoderPolicy>
bool SerialMaster<EncoderPolicy>::runRx()
{
  if (doRx)
    doRx = !!PT_SCHEDULE(rxRunner());

  return doRx;
}

} /* namespace Modbus */
