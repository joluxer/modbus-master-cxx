/*
 * ModbusWriteSingleRegister.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusWriteSingleRegister.h"

#include <cstring>

namespace Modbus
{

WriteSingleRegister::WriteSingleRegister(uint8_t functionCode, uint16_t addr, TxnReturnPath* rp)
: Txn(functionCode, rp),
  pduBuffer{functionCode, uint8_t(addr >> 8), uint8_t(addr >> 0), 0x00, 0x00}
{}


WriteSingleRegister::WriteSingleRegister(uint16_t addr, TxnReturnPath* rp)
: WriteSingleRegister(FunctionCode, addr, rp)
{}

//WriteSingleRegister::~WriteSingleRegister()
//{
//  // Auto-generated destructor stub
//}

WriteSingleRegister& WriteSingleRegister::setAddr(uint16_t addr)
{
  pduBuffer[1] = addr >> 8;
  pduBuffer[2] = addr >> 0;

  return *this;
}

WriteSingleRegister& WriteSingleRegister::setWriteValue(uint16_t val)
{
  pduBuffer[3] = val >> 8;
  pduBuffer[4] = val >> 0;

  return *this;
}

uint8_t WriteSingleRegister::getTxLength()
{
  return PduLength;
}

bool WriteSingleRegister::getTxData(PduConstDataBuffer& txData, unsigned offset)
{
  if (PduLength > offset)
  {
    txData.data = &pduBuffer[offset];
    txData.length = PduLength - offset;
    return true;
  }

  txData.data = nullptr;
  txData.length = 0;

  return true;
}

unsigned WriteSingleRegister::processRxData(const PduConstDataBuffer& rx)
{
  if (::memcmp(rx.data, pduBuffer, PduLength))
    resultCode = NoReceiveBuffer;

  return rx.length;
}

} /* namespace Modbus */
