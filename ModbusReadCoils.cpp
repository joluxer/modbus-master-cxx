/*
 * ModbusReadCoils.cpp
 *
 *  Created on: 09.02.2016
 *      Author: lode
 */

#include "ModbusReadCoils.h"

namespace Modbus
{

ReadCoils2Array::ReadCoils2Array(unsigned* array, uint16_t numOfBits)
: ReadBits2Array(FunctionCode, array, numOfBits)
{}

//ReadCoils2Array::~ReadCoils2Array()
//{
//  // Auto-generated destructor stub
//}

} /* namespace Modbus */
