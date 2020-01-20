/*
 * ModbusReadDiscreteInputs.h
 *
 *  Created on: 11.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADDISCRETEINPUTS_H_
#define MODBUSREADDISCRETEINPUTS_H_

#include "ModbusReadBits.h"

namespace Modbus
{

class ReadDiscreteInputs2Array: public Modbus::ReadBits2Array
{
public:
  ReadDiscreteInputs2Array(unsigned* array = nullptr, uint16_t numOfBits = 0);
//  virtual ~ReadDiscreteInputs2Array();

protected:
  static const uint8_t FunctionCode = 0x02;
};

} /* namespace Modbus */
#endif /* MODBUSREADDISCRETEINPUTS_H_ */
