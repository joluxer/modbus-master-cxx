/*
 * PrintRegistersFormatted.h
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#ifndef PRINTREGISTERSFORMATTED_H_
#define PRINTREGISTERSFORMATTED_H_

#include <cstdint>
#include <string>

#include "RegisterUtils.h"

void printRegistersFormatted(::std::string const& dataType, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr);

#endif /* PRINTREGISTERSFORMATTED_H_ */
