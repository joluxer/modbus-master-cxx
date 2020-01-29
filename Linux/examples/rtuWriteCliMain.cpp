/*
 * rtuWriteCliMain.cpp
 *
 *  Created on: 29.01.2020
 *      Author: lode
 */

#include "version.h"

#include "ModbusMaster/ModbusWriteMultipleRegisters.h"
#include "ModbusMaster/ModbusRtuMaster.h"
#include "ModbusMaster/ModbusSlaveProxy.h"
#include "Hardware/Linux/TtyUart.h"
#include "Util/PosixTimer.h"
#include "Util/FdIoStream.h"
#include "Util/HexConverter.h"
#include "Util/TaggingOutStream.h"

#include "Util/ParseRegisterStrings.h"

#include "arg/arg.h"
#include "arg/val.h"

#include <unistd.h>

#include <cstdio>
#include <iostream>
#include <cstring>

int debug = 0;

int main(int argc, char ** argv)
{
  ::std::string busDevice("/dev/ttyS0");
  unsigned baudrate = 19200;
  ::std::string charFormat("8e1");
  uint16_t startAddr = 1000;
  uint16_t count = 1;
  ::std::string dataType("hex16");
  unsigned slaveId = 1;
  int rs485DriverEnable = 0;
  ::std::string dataValue("0");

  arg::Parser p;

  p.set_header("Modbus RTU register writer");
  p.add_help("Version r" REVISION);
  p.add_help("");
  p.add_help(std::string("available options are: ") + argv[0] + " [options]");

  p.add_opt('p', "serial-port").stow(busDevice).help("serial port device for the Modbus", "/dev/UART").show_default();
  p.add_opt('b', "baudrate").stow(baudrate).help("serial port baudrate", "BAUD").show_default();
  p.add_opt("char-format").stow(charFormat).help("serial port character format", "CHARFMT").show_default();
  p.add_opt('r', "rs485-driver-enable").set(&rs485DriverEnable, 1).help("enable RS485 driver control signals of serial port device");
  p.add_opt('s', "slave-addr").stow(slaveId).help("Modbus slave ID (1..247)", "ID").show_default();
  p.add_opt('a', "start-addr").stow(startAddr).help("Modbus start address for the registers to write", "ADDR").show_default();
  p.add_opt('t', "data-type").stow(dataType).help("select data format", "FMT").show_default();
  p.add_opt('v', "data-value").stow(dataValue).help("data value to write", "VAL").show_default();
  p.add_opt("debug", true).set(&debug, 1).help("enable developer debug output");

  p.add_opt_help();
  p.add_opt_version("r" REVISION);

  p.add_help("");
  p.add_help("Valid character formats:");
  p.add_help("8e1       8 bit, even parity, 1 stop bit");
  p.add_help("8n2       8 bit, no parity, 2 stop bits");
  p.add_help("8n1       (not allowed by Modbus standard) 8 bit, no parity, 1 stop bit");

  p.add_help("");
  p.add_help("Valid data formats:");
  p.add_help("hex16     16 bit hexadecimal value");
  p.add_help("int16     16 bit signed decimal value");
  p.add_help("uint16    16 bit unsigned decimal value");
  p.add_help("hex32     32 bit hexadecimal value (big endian)");
  p.add_help("hex32le   32 bit hexadecimal value (little endian)");
  p.add_help("int32     32 bit signed decimal value (big endian)");
  p.add_help("int32le   32 bit signed decimal value (little endian)");
  p.add_help("uint32    32 bit unsigned decimal value (big endian)");
  p.add_help("uint32le  32 bit unsigned decimal value (little endian)");
  p.add_help("float32   32 bit floating point value (big endian)");
  p.add_help("float32le 32 bit floating point value (little endian)");
  p.add_help("char      2x 8 bit character per register (high byte is first character)");

  try
  {
    p.parse(argc, argv, true);
  }
  catch (arg::OptError& err)
  {
    std::cerr << "command option error, try\"" << argv[0] << " --help\" to get a list of valid options" << std::endl;
    return -1;
  }
  catch (arg::UnknError& err)
  {
    std::cerr << "command option error, try\"" << argv[0] << " --help\" to get a list of valid options" << std::endl;
    return -1;
  }

  unsigned arrayLen = getRegisterCountFor(dataType, count);
  if (dataType == "char")
    arrayLen = getRegisterCountFor(dataType, dataValue.length());

  uint16_t registerArray[arrayLen];
  memset(registerArray, 0, sizeof registerArray);
  Modbus::WriteMultipleRegistersFromArray writeRegister(registerArray, arrayLen);
  Modbus::SlaveProxy slave(slaveId);
  PosixTimer masterTimer, guardTimer, splitTimer;
  Modbus::RtuMaster busController(masterTimer, guardTimer, splitTimer);
  Hardware::Linux::TtyUart busPort;

  FdIoStream stderrStream(2);
  MergedOutStream mbDebugStream(stderrStream);
  TaggingOutStream mbOutDebugText(mbDebugStream);
  TaggingOutStream mbInDebugText(mbDebugStream);
  HexConverter mbOutDebugBin(mbOutDebugText);
  HexConverter mbInDebugBin(mbInDebugText);

  mbOutDebugText.tag = "\nMB out:";
  mbInDebugText.tag  = "\nMB  in:";

  if (debug)
  {
    busPort.dupOutputTo(&mbOutDebugBin);
    busPort.dupInputTo(&mbInDebugBin);
  }

  unsigned dataFormat = Uart::UnChanged;
  if (charFormat == "8e1")
    dataFormat = Uart::LsbFirst8e1;
  else if (charFormat == "8n2")
    dataFormat = Uart::LsbFirst8n2;
  else if (charFormat == "8n1")
    dataFormat = Uart::LsbFirst8n1;
  else
  {
    std::cerr << "invalid character format specified, try\"" << argv[0] << " --help\" to get a list of valid options" << std::endl;
    return -1;
  }


  busPort.placeOnTty(busDevice.c_str()).configure(baudrate, dataFormat);

  if (rs485DriverEnable)
    busPort.enableRs485Mode();

  busController.setTransportChannel(&busPort);
  busController.setTimeoutsForBaudrate(baudrate);
  uint32_t txGuardTime_ms, rxSplitLimit_ms;

  busController.getTimeouts_ms(txGuardTime_ms, rxSplitLimit_ms);
  if (debug)
    printf("regular TX guard time = %d ms\nregular RX split limit = %d ms\n", txGuardTime_ms, rxSplitLimit_ms);

  busController.setTimeouts_ms(txGuardTime_ms + 10, rxSplitLimit_ms + 20);
  if (debug)
  {
    busController.getTimeouts_ms(txGuardTime_ms, rxSplitLimit_ms);
    printf("    new TX guard time = %d ms\n    new RX split limit = %d ms\n", txGuardTime_ms, rxSplitLimit_ms);
  }

  slave.connectVia(busController);

  writeRegister.setStartAddr(startAddr);

  if (not parseRegisterStrings(dataType, dataValue, registerArray, arrayLen))
  {
    printf("ERROR: could not convert %s to a valid %s value\n", dataValue.c_str(), dataType.c_str());
    return -1;
  }

  if (slave.enqueueNoDelete(writeRegister))
  {
    while (slave.runQueue())
      usleep(500);
  }

  auto doneTxn = slave.dequeueResult();

  auto rc = doneTxn->getResultCode();

  if (debug)
    putchar('\n');

  if (Modbus::NoError == rc)
  {
    puts("OK");
  }
  else
  {
    printf("Modbus error code: %d\n", rc);
  }

  return rc;
}





