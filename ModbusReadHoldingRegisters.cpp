/*
 * ModbusReadHoldingRegisters.cpp
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#include "ModbusReadHoldingRegisters.h"

namespace Modbus
{

ReadHoldingRegisters2Array::ReadHoldingRegisters2Array(uint16_t* array, uint16_t numOfRegs, TxnReturnPath* rp)
: ReadRegisters2Array(FunctionCode, array, numOfRegs, rp)
{}

//ReadHoldingRegisters2Array::~ReadHoldingRegisters2Array()
//{
//  // Auto-generated destructor stub
//}

} /* namespace Modbus */
