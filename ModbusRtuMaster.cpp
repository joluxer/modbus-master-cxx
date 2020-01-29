/*
 * ModbusRtuMaster.cpp
 *
 *  Created on: 28.01.2020
 *      Author: lode
 */

#include "ModbusRtuMaster.h"
#include "ModbusSerialMaster.tpl"

namespace Modbus
{

template class SerialMaster<Rtu>;

}
