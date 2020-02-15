/*
 * PrintModbusMasterExceptionCode.h
 *
 *  Created on: 31.01.2020
 *      Author: lode
 */

#ifndef MODBUSMASTEREXCEPTIONCODETOSTRING_H_
#define MODBUSMASTEREXCEPTIONCODETOSTRING_H_

#include "ModbusMaster/ModbusTypes.h"

/**
 * Returns a constant zero terminated string pointer for the given result code.
 *
 * The returned string is not allocated, so no free() needed.
 *
 * @param rc
 * @return
 */
const char* modbusMasterExceptionCodeToString(::Modbus::ResultCode rc);


#endif /* MODBUSMASTEREXCEPTIONCODETOSTRING_H_ */
