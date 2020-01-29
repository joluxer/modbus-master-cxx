/*
 * ModbusRtuMaster.h
 *
 *  Created on: 28.01.2020
 *      Author: lode
 */

#ifndef MODBUSRTUMASTER_H_
#define MODBUSRTUMASTER_H_

#include "ModbusRtu.h"
#include "ModbusSerialMaster.h"

namespace Modbus
{

typedef SerialMaster<Rtu> RtuMaster;

} // namespace Modbus

#endif /* MODBUSRTUMASTER_H_ */
