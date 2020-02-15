/*
 * ModbusReadBits.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusReadBits.h"

#include "FormattedLog.h"

namespace Modbus
{

ReadBits2Array::ReadBits2Array(uint8_t functionCode, unsigned* array, uint16_t numOfBits, TxnReturnPath* rp)
: Txn(functionCode, rp),
  txPduBuffer{functionCode, 0xff, 0xff, 0xff, 0xff},
  bitBuffer{nullptr}, bufferLength{0}
{
  readTo(array, numOfBits);
}

//ReadBits2Array::~ReadBits2Array()
//{
//  // Auto-generated destructor stub
//}

void ReadBits2Array::readTo(unsigned* array, uint16_t numOfBits)
{
  bitBuffer = array;

  bufferLength = numOfBits2ArrayLength(numOfBits);

  txPduBuffer[3] = numOfBits >> 8;
  txPduBuffer[4] = numOfBits >> 0;
}

void ReadBits2Array::setStartAddr(uint16_t addr)
{
  txPduBuffer[1] = addr >> 8;
  txPduBuffer[2] = addr >> 0;
}

uint8_t ReadBits2Array::getTxLength()
{
  return TxPduLength;
}

bool ReadBits2Array::getTxData(PduConstDataBuffer& tx, unsigned offset)
{
  if (TxPduLength > offset)
  {
    tx.data = &txPduBuffer[offset];
    tx.length = TxPduLength - offset;
    return true;
  }

  tx.data = nullptr;
  tx.length = 0;

  return true;
}

unsigned ReadBits2Array::processRxData(const PduConstDataBuffer& rx)
{
  if (bitBuffer)
  {
    do // no goto, just break
    {
      if (rx.data[0] != functionCode)
      {
        setResultCode(AnswerDoesNotMatchRequest);
        break;
      }

      if (rx.length != (2 + rx.data[1]))
      {
        fLog(logDebug, "%s: RX length mismatch in PDU: %u status bytes => %u RX bytes, received: %u bytes", __PRETTY_FUNCTION__, rx.data[1], (2 + rx.data[1]), rx.length);
        setResultCode(UnknownCommunicationError);
        break;
      }

      auto const bufferByteCount = bufferLength * sizeof(bitBuffer[0]);
      auto dataLength = rx.data[1];

      if (bufferByteCount < dataLength)
        dataLength = bufferByteCount;

      unsigned bitsTmp;
      unsigned rxIdx, bitShift, bitBufIdx;

      rxIdx = 2;
      bitShift = 0;
      bitBufIdx = 0;
      bitsTmp = 0;
      dataLength += rxIdx;
      while (dataLength > rxIdx)
      {
        bitsTmp |= rx.data[rxIdx] << bitShift;
        bitShift += 8;
        ++rxIdx;
        if (((sizeof(bitBuffer[0]) * 8) <= bitShift) || (dataLength <= rxIdx))
        {
          bitBuffer[bitBufIdx] = bitsTmp;
          ++bitBufIdx;
          bitShift = 0;
          bitsTmp = 0;
        }
      }
    } while (false);
  }
  else
    resultCode = NoReceiveBuffer;

  return rx.length;
}

} /* namespace Modbus */
