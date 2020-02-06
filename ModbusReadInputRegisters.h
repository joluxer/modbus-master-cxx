/*
 * ModbusReadInputRegisters.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADINPUTREGISTERS_H_
#define MODBUSREADINPUTREGISTERS_H_

#include "ModbusReadRegisters.h"

namespace Modbus
{

class ReadInputRegisters2Array: public Modbus::ReadRegisters2Array
{
public:
  ReadInputRegisters2Array(uint16_t* array = nullptr, uint16_t numOfRegs = 0, TxnReturnPath* rp = nullptr);
//  virtual ~ReadInputRegisters2Array();

protected:
  static const uint8_t FunctionCode = 0x04;
};

} /* namespace Modbus */
#endif /* MODBUSREADINPUTREGISTERS_H_ */
