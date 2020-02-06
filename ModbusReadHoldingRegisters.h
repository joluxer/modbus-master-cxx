/*
 * ModbusReadHoldingRegisters.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADHOLDINGREGISTERS_H_
#define MODBUSREADHOLDINGREGISTERS_H_

#include "ModbusReadRegisters.h"

namespace Modbus
{

class ReadHoldingRegisters2Array: public Modbus::ReadRegisters2Array
{
public:
  ReadHoldingRegisters2Array(uint16_t* array = nullptr, uint16_t numOfRegs = 0, TxnReturnPath* rp = nullptr);
//  virtual ~ReadHoldingRegisters2Array();

protected:
  static const uint8_t FunctionCode = 0x03;
};

} /* namespace Modbus */
#endif /* MODBUSREADHOLDINGREGISTERS_H_ */
