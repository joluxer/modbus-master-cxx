/*
 * ModbusWriteSingleCoil.h
 *
 *  Created on: 07.02.2016
 *      Author: lode
 */

#ifndef MODBUSWRITESINGLECOIL_H_
#define MODBUSWRITESINGLECOIL_H_

#include "ModbusWriteSingleRegister.h"

namespace Modbus
{

class WriteSingleCoil: public Modbus::WriteSingleRegister
{
public:
  explicit
  WriteSingleCoil(uint16_t addr);
//  virtual ~WriteSingleCoil();

  WriteSingleRegister& setWriteValue(uint16_t val) = delete;
  WriteSingleCoil& setWriteState(bool state);

protected:
  static const uint8_t FunctionCode = 0x05;
};

} /* namespace Modbus */
#endif /* MODBUSWRITESINGLECOIL_H_ */
