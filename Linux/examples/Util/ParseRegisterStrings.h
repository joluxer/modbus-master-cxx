/*
 * ParseRegisterStrings.h
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#ifndef PARSEREGISTERSTRINGS_H_
#define PARSEREGISTERSTRINGS_H_

#include <string>
#include <cstdint>

#include "RegisterUtils.h"

bool parseRegisterStrings(::std::string const& dataType, ::std::string const& dataValue, uint16_t *regs, unsigned arrayLength);

#endif /* PARSEREGISTERSTRINGS_H_ */
