/*
 * ModbusWriteMultipleRegisters.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusWriteMultipleRegisters.h"

#include <cstdlib>
#include <cstring>

namespace Modbus
{

WriteMultipleRegistersFromArray::WriteMultipleRegistersFromArray(const uint16_t* array, uint16_t numOfRegs, TxnReturnPath* rp)
: Txn(FunctionCode, rp),
  txPduHeader{FunctionCode, 0xff, 0xff, 0xff, 0xff, 0x00},
  regsBuffer{nullptr}, bufferLength{0}
{
  readFrom(array, numOfRegs);
}

//WriteMultipleRegistersFromArray::~WriteMultipleRegistersFromArray()
//{
//  // Auto-generated destructor stub
//}

void WriteMultipleRegistersFromArray::readFrom(const uint16_t* array, uint16_t numOfRegs)
{
  regsBuffer = array;

  if (array)
  {
    bufferLength = numOfRegs;

    txPduHeader[3] = numOfRegs >> 8;
    txPduHeader[4] = numOfRegs >> 0;

    txPduHeader[5] = numOfRegs * sizeof(regsBuffer[0]);
  }
  else
  {
    bufferLength = 0;

    txPduHeader[3] = 0;
    txPduHeader[4] = 0;

    txPduHeader[5] = 0;
  }
}

void WriteMultipleRegistersFromArray::setStartAddr(uint16_t addr)
{
  txPduHeader[1] = addr >> 8;
  txPduHeader[2] = addr >> 0;
}

uint8_t WriteMultipleRegistersFromArray::getTxLength()
{
  return TxPduLength + txPduHeader[5];
}

bool WriteMultipleRegistersFromArray::getTxData(PduConstDataBuffer& tx, unsigned offset)
{
  bool isComplete = false;

  if (TxPduLength > offset)
  {
    tx.data = &txPduHeader[offset];
    tx.length = TxPduLength - offset;
    isComplete = (not regsBuffer) || (0 == bufferLength);
  }
  else if (regsBuffer && (0 < bufferLength))
  {
    unsigned txOffset, txIdx;
    div_t idx;

    // Offsets berechnen
    txOffset = offset - TxPduLength;
    idx = div(txOffset, sizeof(uint16_t));
    txIdx = idx.quot;

    // Byte-Buffer füllen (optimierbar, wenn schon mal getan für einen bestimmten Offset(-Bereich), ist aber marginal)

    txByteBuffer[0] = regsBuffer[txIdx] >> 8;
    txByteBuffer[1] = regsBuffer[txIdx] >> 0;

    // TX-Daten setzen
    tx.data = &txByteBuffer[idx.rem];
    tx.length = sizeof(txByteBuffer) - idx.rem;

    isComplete = (bufferLength <= (txIdx + 1u));
  }

  return isComplete;
}

unsigned WriteMultipleRegistersFromArray::processRxData(const PduConstDataBuffer& rx)
{
  if (::memcmp(rx.data, txPduHeader, TxPduLength - 1))
    resultCode = NoReceiveBuffer;

  return rx.length;
}

} /* namespace Modbus */
