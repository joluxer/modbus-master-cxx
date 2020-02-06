/*
 * ModbusReadCoils.h
 *
 *  Created on: 09.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADCOILS_H_
#define MODBUSREADCOILS_H_

#include "ModbusReadBits.h"

namespace Modbus
{

class ReadCoils2Array: public Modbus::ReadBits2Array
{
public:
  explicit
  ReadCoils2Array(unsigned* array = nullptr, uint16_t numOfBits = 0, TxnReturnPath* rp = nullptr);
//  virtual ~ReadCoils2Array();

protected:
  static const uint8_t FunctionCode = 0x01;
};

} /* namespace Modbus */
#endif /* MODBUSREADCOILS_H_ */
