/*
 * ModbusWriteMultipleCoils.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusWriteMultipleCoils.h"

#include "FormattedLog.h"

#include <cstdlib>
#include <cstring>

namespace Modbus
{

WriteMultipleCoilsFromArray::WriteMultipleCoilsFromArray(const unsigned* array, uint16_t numOfBits, TxnReturnPath* rp)
: Txn(FunctionCode, rp),
  bitBuffer{nullptr}, bufferLength{0}, numOfBits{0},
  txPduHeader{FunctionCode, 0xff, 0xff, 0xff, 0xff, 0x00}
{
  readFrom(array, numOfBits);
}

//WriteMultipleCoilsFromArray::~WriteMultipleCoilsFromArray()
//{
//  // Auto-generated destructor stub
//}

void WriteMultipleCoilsFromArray::readFrom(const unsigned* array, uint16_t numOfBits_)
{
  bitBuffer = array;
  numOfBits = numOfBits_;

  if (array)
  {
    bufferLength = numOfBits2ArrayLength(numOfBits_);

    txPduHeader[3] = numOfBits >> 8;
    txPduHeader[4] = numOfBits >> 0;

    txPduHeader[5] = numOfBits / 8 + (numOfBits % 8 ? 1 : 0);
  }
  else
  {
    bufferLength = 0;

    txPduHeader[3] = 0;
    txPduHeader[4] = 0;

    txPduHeader[5] = 0;
  }
}

void WriteMultipleCoilsFromArray::setStartAddr(uint16_t addr)
{
  txPduHeader[1] = addr >> 8;
  txPduHeader[2] = addr >> 0;
}

uint8_t WriteMultipleCoilsFromArray::getTxLength()
{
  return TxPduHeaderLength + txPduHeader[5];
}

bool WriteMultipleCoilsFromArray::getTxData(PduConstDataBuffer& tx, unsigned offset)
{
  bool isComplete = false;

  if (TxPduHeaderLength > offset)
  {
    tx.data = &txPduHeader[offset];
    tx.length = TxPduHeaderLength - offset;
    isComplete = (not bitBuffer) || (0 == bufferLength);
  }
  else if (bitBuffer && (0 < bufferLength))
  {
    unsigned txOffset, txIdx, bitShift, bitBufIdx;
    div_t idx;

    // Offsets berechnen
    txOffset = offset - TxPduHeaderLength;
    idx = div(txOffset, sizeof(bitBuffer[0]));
    txIdx = idx.quot;
    auto bitShiftOffset = txOffset * 8;

    // Bit-Buffer füllen (optimierbar, wenn schon mal getan für einen bestimmten Offset(-Bereich), ist aber marginal)
    bitShift = 0;
    bitBufIdx = 0;
    do
    {
      txBitBuffer[bitBufIdx] = bitBuffer[txIdx] >> bitShift;
      bitShift += 8;
      ++bitBufIdx;
    } while (numOfBits > (bitShiftOffset + bitShift));

    // TX-Daten setzen
    tx.data = &txBitBuffer[idx.rem];
    tx.length = bitBufIdx - idx.rem;

    isComplete = ((bitShiftOffset + bitShift) >= numOfBits);
  }

  return isComplete;
}

unsigned WriteMultipleCoilsFromArray::processRxData(const PduConstDataBuffer& rx)
{
  if (((TxPduHeaderLength - 1) != rx.length)  or (::memcmp(rx.data, txPduHeader, TxPduHeaderLength - 1)))
    resultCode = AnswerDoesNotMatchRequest;

  return rx.length;
}

} /* namespace Modbus */
