/*
 * FdSelectRtuDriver.cpp
 *
 *  Created on: 27.01.2020
 *      Author: lode
 */

#include "FdSelectRtuDriver.h"
#include "../ModbusSlaveProxy.h"

#include <cassert>

namespace Modbus
{

FdSelectRtuDriver::FdSelectRtuDriver(bool slavesPushMaster)
: uartStream(nullptr),
  modbusMaster(mbMasterTimer, txGuardTimer, rxSplitTimer),
  txSignal(this, &FdSelectRtuDriver::privEnableTx),
  rxSignal(this, &FdSelectRtuDriver::privEnableRx),
  myErrno(0),
  destroyed(false),
  slavesPushMaster(slavesPushMaster)
{
  modbusMaster.connectEnableReceiver(&rxSignal);
  modbusMaster.connectEnableTransmitter(&txSignal);
}

FdSelectRtuDriver::~FdSelectRtuDriver()
{
  if (not destroyed)
    destroy();
}

void FdSelectRtuDriver::destroy()
{
  destroyed = true;
  if (uartStream)
    uartStream->closeOsDevice();
  slaves.clear();
}

void FdSelectRtuDriver::setTimeoutsForBaudrate(uint32_t baudrate)
{
  modbusMaster.setTimeoutsForBaudrate(baudrate);
}

void FdSelectRtuDriver::setTimeouts_ms(uint32_t txGuardTime_ms, uint32_t rxSplitLimit_ms)
{
  modbusMaster.setTimeouts_ms(txGuardTime_ms, rxSplitLimit_ms);
}

void FdSelectRtuDriver::getTimeouts_ms(uint32_t &txGuardTime_ms, uint32_t &rxSplitLimit_ms)
{
  modbusMaster.getTimeouts_ms(txGuardTime_ms, rxSplitLimit_ms);
}

void FdSelectRtuDriver::connectPort(::Hardware::Linux::TtyUart& uart)
{
  uartStream = &uart;
  if (uartStream)
  {
    modbusMaster.setTransportChannel(uartStream);

    uint32_t baudrate;
    unsigned dummyDF;

    uartStream->getConfiguration(baudrate, dummyDF);
    modbusMaster.setTimeoutsForBaudrate(baudrate);
  }
}

bool FdSelectRtuDriver::setPort(const ::std::string& portPath, unsigned baudrate, const ::std::string& charFmt)
{
  bool success = false;

  if (not destroyed and uartStream)
  {
    uartStream->closeOsDevice();
    modbusMaster.setTransportChannel(nullptr);

    unsigned dataFormat = Uart::UnChanged;
    if (charFmt == "8e1")
      dataFormat = Uart::LsbFirst8e1;
    else if (charFmt == "8n2")
      dataFormat = Uart::LsbFirst8n2;
    else if (charFmt == "8n1")
      dataFormat = Uart::LsbFirst8n1;
    else
      dataFormat = Uart::LsbFirst8e1;

    success = uartStream->placeOnTty(portPath.c_str()).enableRs485Mode().configure(baudrate, dataFormat);

    // Fehlerbehandlung
    if (not success or uartStream->hasPosixError())
      myErrno = uartStream->getErrno();
    else
    {
      modbusMaster.setTransportChannel(uartStream);
      modbusMaster.setTimeoutsForBaudrate(baudrate);
    }
  }

  return success;
}

::std::weak_ptr<SlaveProxy> FdSelectRtuDriver::getSlave(uint8_t slaveId)
{
  assert(not destroyed);

  ::std::weak_ptr<SlaveProxy> slavePtr;

  bool wasEmpty = slaves.empty();

  if (0 == slaves.count(slaveId))
  {
    auto result = slaves.emplace(slaveId, new SlaveProxy(slaveId, slavesPushMaster));

    if (result.second)
    {
      slavePtr = result.first->second;

      auto slave = slavePtr.lock();
      if (slave)
        slave->connectVia(modbusMaster);
    }
  }
  else
    slavePtr = slaves.at(slaveId);

  if (wasEmpty)
    nextStartSlave = slaves.begin();

  return slavePtr;
}

bool FdSelectRtuDriver::hasPosixError() const
{
  errno = myErrno;
  return !!myErrno;
}

int FdSelectRtuDriver::getErrno() const
{
  return myErrno;
}


// this implements the workhorse for reacting to readable and writable I/O
void FdSelectRtuDriver::execute(bool readable, bool writable)
{
  assert(not destroyed and uartStream);

  if (uartStream->hasPosixError())
    myErrno = uartStream->getErrno();

  if (modbusMaster.readyForNextTxn() and not slaves.empty())
  {
    // do round robin scheduling of the slaves, so if the master is busy,
    // not always the same slave pushes its transactions through first
    ++nextStartSlave;
    if (slaves.end() == nextStartSlave)
      nextStartSlave = slaves.begin();

    // run around the chain of slaves one time
    auto slave = nextStartSlave;
    do
    {
      slave->second->runQueue();
      ++slave;

      if (slaves.end() == slave)
        slave = slaves.begin();

    } while (slave != nextStartSlave);
  }

  auto masterState = modbusMaster.operate();

  waitForWrite = !!(masterState & 1);

  waitForRead = !!(masterState & 2);
}

int FdSelectRtuDriver::fd()
{
  assert(not destroyed and uartStream);

  return uartStream->fd();
}

void FdSelectRtuDriver::privEnableTx(bool enable)
{
  waitForWrite = enable;
}

void FdSelectRtuDriver::privEnableRx(bool enable)
{
  waitForRead = enable;
}

} /* namespace Modbus */
