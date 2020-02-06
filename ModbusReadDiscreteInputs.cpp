/*
 * ModbusReadDiscreteInputs.cpp
 *
 *  Created on: 11.02.2016
 *      Author: lode
 */

#include "ModbusReadDiscreteInputs.h"

namespace Modbus
{

ReadDiscreteInputs2Array::ReadDiscreteInputs2Array(unsigned* array, uint16_t numOfBits, TxnReturnPath* rp)
: ReadBits2Array(FunctionCode, array, numOfBits, rp)
{}

//ReadDiscreteInputs2Array::~ReadDiscreteInputs2Array()
//{
//  // Auto-generated destructor stub
//}

} /* namespace Modbus */
