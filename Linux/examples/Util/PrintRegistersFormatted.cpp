/*
 * PrintRegistersFormatted.cpp
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#include "PrintRegistersFormatted.h"

#include "Util/BinaryBuffer.h"

#include <limits>
#include <cctype>
#include <cstdio>

#include <inttypes.h>

static
void printOutputFormatHex16Table(uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  unsigned i;
  auto word = regs[0];
  auto addr = startAddr;
  puts("Register | Value");
  for (i = 0; i < arrayLength; ++i)
  {
    word = regs[i];
    printf("  0x%.4x | 0x%.4" PRIx16 "\n", addr, word);
    --count;
    ++addr;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatInt16Table(uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  unsigned i;
  auto word = int16_t(regs[0]);
  auto addr = startAddr;
  puts("Register | Value");
  for (i = 0; i < arrayLength; ++i)
  {
    word = int16_t(regs[i]);
    printf("  0x%.4x | %" PRIi16 "\n", addr, word);
    --count;
    ++addr;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatUint16Table(uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  unsigned i;
  auto word = uint16_t(regs[0]);
  auto addr = startAddr;
  puts("Register | Value");
  for (i = 0; i < arrayLength; ++i)
  {
    word = uint16_t(regs[i]);
    printf("  0x%.4x | %" PRIu16 "\n", addr, word);
    --count;
    ++addr;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatHex32Table(bool bigEndian, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  static_assert(::std::numeric_limits<float>::is_iec559, "Floating point implementation is not IEEE 754 compatible, this implementation does not work");

  unsigned i;
  auto addr = startAddr;
  puts("Register | Value");
  uint8_t convBackendBuffer[sizeof(uint32_t)];
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  for (i = 0; (i*2) < arrayLength; i += 2)
  {
    // put Modbus data to conversion buffer
    convBuffer.setBigEndian();

    convBuffer.resetOffset();
    convBuffer.writeU16(regs[i]);
    convBuffer.writeU16(regs[i + 1]);

    // read data from conversion buffer
    if (not bigEndian)
      convBuffer.setLittleEndian();

    convBuffer.resetOffset();
    printf("  0x%.4x | 0x%.8" PRIx32 "\n", addr, convBuffer.readU32());
    --count;
    addr += 2;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatInt32Table(bool bigEndian, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  static_assert(::std::numeric_limits<float>::is_iec559, "Floating point implementation is not IEEE 754 compatible, this implementation does not work");

  unsigned i;
  auto addr = startAddr;
  puts("Register | Value");
  uint8_t convBackendBuffer[sizeof(uint32_t)];
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  for (i = 0; (i*2) < arrayLength; i += 2)
  {
    // put Modbus data to conversion buffer
    convBuffer.setBigEndian();

    convBuffer.resetOffset();
    convBuffer.writeU16(regs[i]);
    convBuffer.writeU16(regs[i + 1]);

    // read data from conversion buffer
    if (not bigEndian)
      convBuffer.setLittleEndian();

    convBuffer.resetOffset();
    printf("  0x%.4x | %" PRIi32 "\n", addr, convBuffer.readI32());
    --count;
    addr += 2;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatUint32Table(bool bigEndian, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  static_assert(::std::numeric_limits<float>::is_iec559, "Floating point implementation is not IEEE 754 compatible, this implementation does not work");

  unsigned i;
  auto addr = startAddr;
  puts("Register | Value");
  uint8_t convBackendBuffer[sizeof(uint32_t)];
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  for (i = 0; (i*2) < arrayLength; i += 2)
  {
    // put Modbus data to conversion buffer
    convBuffer.setBigEndian();

    convBuffer.resetOffset();
    convBuffer.writeU16(regs[i]);
    convBuffer.writeU16(regs[i + 1]);

    // read data from conversion buffer
    if (not bigEndian)
      convBuffer.setLittleEndian();

    convBuffer.resetOffset();
    printf("  0x%.4x | %" PRIu32 "\n", addr, convBuffer.readU32());
    --count;
    addr += 2;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatFloat32Table(bool bigEndian, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  static_assert(::std::numeric_limits<float>::is_iec559, "Floating point implementation is not IEEE 754 compatible, this implementation does not work");

  unsigned i;
  auto addr = startAddr;
  puts("Register | Value");
  uint8_t convBackendBuffer[sizeof(float)];
  BinaryBuffer convBuffer(sizeof convBackendBuffer, convBackendBuffer);

  for (i = 0; (i*2) < arrayLength; i += 2)
  {
    // put Modbus data to conversion buffer
    convBuffer.setBigEndian();

    convBuffer.resetOffset();
    convBuffer.writeU16(regs[i]);
    convBuffer.writeU16(regs[i + 1]);

    // read data from conversion buffer
    if (not bigEndian)
      convBuffer.setLittleEndian();

    convBuffer.resetOffset();
    printf("  0x%.4x | %f\n", addr, convBuffer.readFloat32());
    --count;
    addr += 2;
    if (count < 1)
      break;
  }
}

static
void printOutputFormatCharString(uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  unsigned i;
  auto addr = startAddr;
  auto char1 = char(regs[0] >> 8);
  auto char2 = char(regs[0] >> 0);
  puts("Register | Value");
  printf("  0x%.4x | %c%c", addr, char1, char2);
  for (i = 0; i < arrayLength; ++i)
  {
    char1 = char(regs[i] >> 8);
    char2 = char(regs[i] >> 0);

    if (not isprint(char1))
      char1 = ' ';
    if (not isprint(char2))
      char2 = ' ';

    printf("%c%c", char1, char2);
    --count;
    ++addr;
    if (count < 1)
      break;
  }
  putchar('\n');
}

void printRegistersFormatted(::std::string const& dataType, uint16_t *regs, unsigned arrayLength, unsigned count, uint16_t startAddr)
{
  if (dataType == "hex16")
    printOutputFormatHex16Table(regs, arrayLength, count, startAddr);
  else if (dataType == "int16")
    printOutputFormatInt16Table(regs, arrayLength, count, startAddr);
  else if (dataType == "uint16")
    printOutputFormatUint16Table(regs, arrayLength, count, startAddr);
  else if (dataType == "hex32")
    printOutputFormatHex32Table(true, regs, arrayLength, count, startAddr);
  else if (dataType == "hex32le")
    printOutputFormatHex32Table(false, regs, arrayLength, count, startAddr);
  else if (dataType == "int32")
    printOutputFormatInt32Table(true, regs, arrayLength, count, startAddr);
  else if (dataType == "int32le")
    printOutputFormatInt32Table(false, regs, arrayLength, count, startAddr);
  else if (dataType == "uint32")
    printOutputFormatUint32Table(true, regs, arrayLength, count, startAddr);
  else if (dataType == "uint32le")
    printOutputFormatUint32Table(false, regs, arrayLength, count, startAddr);
  else if (dataType == "float32")
    printOutputFormatFloat32Table(true, regs, arrayLength, count, startAddr);
  else if (dataType == "float32le")
    printOutputFormatFloat32Table(false, regs, arrayLength, count, startAddr);
  else if (dataType == "char")
    printOutputFormatCharString(regs, arrayLength, count, startAddr);
}


