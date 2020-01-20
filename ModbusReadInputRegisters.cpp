/*
 * ModbusReadInputRegisters.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusReadInputRegisters.h"

namespace Modbus
{

ReadInputRegisters2Array::ReadInputRegisters2Array(uint16_t* array, uint16_t numOfRegs)
: ReadRegisters2Array(FunctionCode, array, numOfRegs)
{}

//ReadInputRegisters2Array::~ReadInputRegisters2Array()
//{
//  // Auto-generated destructor stub
//}

} /* namespace Modbus */
