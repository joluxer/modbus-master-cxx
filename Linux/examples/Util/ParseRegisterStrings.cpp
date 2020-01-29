/*
 * ParseRegisterString.cpp
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#include "ParseRegisterStrings.h"

#include <limits>
#include <type_traits>
#include <cstring>

#include "Util/BinaryBuffer.h"

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseHexStringBE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto uintVal = ::std::stoul(dataValue, &finIndex, 16);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= uintVal))
  {
    int shift = sizeof(NumType) * 8;
    unsigned idx = 0;

    while (!!arrayLength and (shift >= 16))
    {
      shift -= 16;
      regs[idx] = uintVal >> shift;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseHexStringLE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto uintVal = ::std::stoul(dataValue, &finIndex, 16);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= uintVal))
  {
    unsigned shift = 0;
    unsigned idx = 0;

    while (!!arrayLength and (shift < (sizeof(NumType) * 8)))
    {
      regs[idx] = uintVal >> shift;
      shift += 16;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseIntStringBE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto intVal = ::std::stoll(dataValue, &finIndex, 10);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= intVal) and (::std::numeric_limits<NumType>::min() <= intVal))
  {
    unsigned shift = sizeof(NumType) * 8;
    unsigned idx = 0;

    while (!!arrayLength and (shift >= 16))
    {
      shift -= 16;
      regs[idx] = intVal >> shift;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseIntStringLE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto intVal = ::std::stoll(dataValue, &finIndex, 10);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= intVal) and (::std::numeric_limits<NumType>::min() <= intVal))
  {
    unsigned shift = 0;
    unsigned idx = 0;

    while (!!arrayLength and (shift < (sizeof(NumType) * 8)))
    {
      regs[idx] = intVal >> shift;
      shift += 16;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseUintStringBE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto uintVal = ::std::stoull(dataValue, &finIndex, 10);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= uintVal))
  {
    unsigned shift = sizeof(NumType) * 8;
    unsigned idx = 0;

    while (!!arrayLength and (shift >= 16))
    {
      shift -= 16;
      regs[idx] = uintVal >> shift;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

template<typename NumType, typename = typename ::std::is_integral<NumType>::type>
bool parseUintStringLE(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;

  auto uintVal = ::std::stoull(dataValue, &finIndex, 10);

  if ((0 != finIndex) and (::std::numeric_limits<NumType>::max() >= uintVal))
  {
    unsigned shift = 0;
    unsigned idx = 0;

    while (!!arrayLength and (shift < (sizeof(NumType) * 8)))
    {
      regs[idx] = uintVal >> shift;
      shift += 16;
      ++idx;
      --arrayLength;
    }
    success = true;
  }

  return success;
}

static
bool parseFloat32String(bool bigEndian, ::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  bool success = false;
  size_t finIndex = 0;
  uint8_t convBackendBuffer[sizeof(float)];
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  auto fVal = ::std::stof(dataValue, &finIndex);

  if (0 != finIndex)
  {
    if (bigEndian)
      convBuffer.setBigEndian();
    else
      convBuffer.setLittleEndian();

    convBuffer.resetOffset();
    convBuffer.writeFloat32(fVal);

    convBuffer.resetOffset();
    convBuffer.setBigEndian();
    for (unsigned idx = 0; idx < arrayLength; ++idx)
      regs[idx] = convBuffer.readU16();

    success = true;
  }

  return success;
}

static
bool copyString(::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  uint8_t convBackendBuffer[dataValue.length() + 2];
  memset(convBackendBuffer, 0, sizeof convBackendBuffer);
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  convBuffer.resetOffset();
  convBuffer.setBigEndian();

  unsigned idx;
  for (idx = 0; idx < dataValue.length(); ++idx)
    convBuffer.writeI8(dataValue[idx]);

  convBuffer.resetOffset();
  for (idx = 0; idx < arrayLength; ++idx)
    regs[idx] = convBuffer.readU16();

  return true;
}

bool parseRegisterStrings(::std::string const& dataType, ::std::string const& dataValue, uint16_t *regs, unsigned arrayLength)
{
  if (dataType == "hex16")
    return parseHexStringBE<uint16_t>(dataValue, regs, arrayLength);
  else if (dataType == "int16")
    return parseIntStringBE<int16_t>(dataValue, regs, arrayLength);
  else if (dataType == "uint16")
    return parseUintStringBE<uint16_t>(dataValue, regs, arrayLength);
  else if (dataType == "hex32")
    return parseHexStringBE<uint32_t>(dataValue, regs, arrayLength);
  else if (dataType == "hex32le")
    return parseHexStringLE<uint32_t>(dataValue, regs, arrayLength);
  else if (dataType == "int32")
    return parseIntStringBE<int32_t>(dataValue, regs, arrayLength);
  else if (dataType == "int32le")
    return parseIntStringLE<int32_t>(dataValue, regs, arrayLength);
  else if (dataType == "uint32")
    return parseUintStringBE<uint32_t>(dataValue, regs, arrayLength);
  else if (dataType == "uint32le")
    return parseUintStringLE<uint32_t>(dataValue, regs, arrayLength);
  else if (dataType == "float32")
    return parseFloat32String(true, dataValue, regs, arrayLength);
  else if (dataType == "float32le")
    return parseFloat32String(false, dataValue, regs, arrayLength);
  else if (dataType == "char")
    return copyString(dataValue, regs, arrayLength);

  return false;
}
