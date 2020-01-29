/*
 * RegisterUtils.cpp
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#include "RegisterUtils.h"

unsigned getRegisterCountFor(::std::string const& dataType, unsigned dataCount)
{
  unsigned regCount = 0;

  if ((dataType == "hex16") or (dataType == "int16") or (dataType == "uint16"))
    regCount = dataCount;
  else if ((dataType == "hex32") or (dataType == "hex32le") or (dataType == "int32") or (dataType == "int32le") or (dataType == "uint32") or (dataType == "uint32le"))
    regCount = dataCount * 2;
  else if ((dataType == "float32") or (dataType == "float32le"))
    regCount = dataCount * 2;
  else if (dataType == "char")
    regCount = (dataCount / 2) + (dataCount % 2);

  return regCount;
}

