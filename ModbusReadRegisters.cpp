/*
 * ModbusReadRegisters.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusReadRegisters.h"

namespace Modbus
{

ReadRegisters2Array::ReadRegisters2Array(uint8_t functionCode, uint16_t* array, uint16_t numOfRegs)
: Txn(functionCode),
  txPduBuffer{functionCode, 0xff, 0xff, 0xff, 0xff},
  regsBuffer{nullptr}, bufferLength{0}
{
  readTo(array, numOfRegs);
}

//ReadRegisters2Array::~ReadRegisters2Array()
//{
//  // Auto-generated destructor stub
//}

void ReadRegisters2Array::readTo(uint16_t* array, uint16_t numOfRegs)
{
  regsBuffer = array;

  bufferLength = numOfRegs;

  txPduBuffer[3] = numOfRegs >> 8;
  txPduBuffer[4] = numOfRegs >> 0;
}

void ReadRegisters2Array::setStartAddr(uint16_t addr)
{
  txPduBuffer[1] = addr >> 8;
  txPduBuffer[2] = addr >> 0;
}

uint8_t ReadRegisters2Array::getTxLength()
{
  return TxPduLength;
}

bool ReadRegisters2Array::getTxData(PduConstDataBuffer& tx, unsigned offset)
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

unsigned ReadRegisters2Array::processRxData(const PduConstDataBuffer& rx)
{
  if (regsBuffer)
  {
    do // no goto, just break
    {
      if (rx.data[0] != functionCode)
      {
        setResultCode(AnswerDoesNotMatchRequest);
        break;
      }

      if (rx.length == (2 + rx.data[1]))
      {
        setResultCode(UnknownCommunicationError);
        break;
      }

      auto const bufferByteCount = bufferLength * sizeof(regsBuffer[0]);
      auto dataLength = rx.data[1];

      if (bufferByteCount < dataLength)
        dataLength = bufferByteCount;

      unsigned rxIdx, regsIdx;

      rxIdx = 2;
      regsIdx = 0;
      dataLength += rxIdx;
      while (dataLength > rxIdx)
      {
        regsBuffer[regsIdx] = (rx.data[rxIdx] << 8) | (rx.data[rxIdx + 1] << 0);
        rxIdx = rxIdx + 2;
        ++regsIdx;
      }
    } while (false);
  }
  else
    resultCode = NoReceiveBuffer;

  return rx.length;
}

} /* namespace Modbus */
