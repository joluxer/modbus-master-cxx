/*
 * ModbusWriteSingleCoil.cpp
 *
 *  Created on: 07.02.2016
 *      Author: lode
 */

#include "ModbusWriteSingleCoil.h"

namespace Modbus
{

WriteSingleCoil::WriteSingleCoil(uint16_t addr)
: WriteSingleRegister(FunctionCode, addr)
{}

//WriteSingleCoil::~WriteSingleCoil()
//{
//  // Auto-generated destructor stub
//}

WriteSingleCoil& WriteSingleCoil::setWriteState(bool state)
{
  if (state)
    WriteSingleRegister::setWriteValue(0xff00);
  else
    WriteSingleRegister::setWriteValue(0x0000);

  return *this;
}

} /* namespace Modbus */
