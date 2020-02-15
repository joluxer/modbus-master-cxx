/*
 * FdSelectRtuDriver.h
 *
 *  Created on: 27.01.2020
 *      Author: lode
 */

#ifndef FDSELECTRTUDRIVER_H_
#define FDSELECTRTUDRIVER_H_

#include "../ModbusRtu.h"
#include "../ModbusSerialMaster.h"
#include "FdSelectHandler.h"
#include "Hardware/Linux/TtyUart.h"
#include "Util/PosixTimer.h"
#include "Callback.h"

#include <memory>
#include <map>
#include <string>

namespace Modbus
{

/*
 *
 */
class FdSelectRtuDriver: public FdSelectHandler
{
public:
  explicit
  FdSelectRtuDriver(bool slavesPushMaster = false);
//  virtual
  ~FdSelectRtuDriver();

  void destroy() final;

  void connectPort(::Hardware::Linux::TtyUart& uart);

  /**
   * Set the port and port parameter.
   * @param portPath Path to the port, i.e. /dev/ttyS0.
   * @param baudrate Desired baudrate number, set to zero, if no configuration of the port file is desired.
   * @param charFmt Desired character format on the line.
   * @return true for successful port access and configuration
   */
  bool setPort(const ::std::string& portPath, unsigned baudrate = 19200, const ::std::string& charFmt = "8e1"); // accepted values for charFmt are "8e1", "8n2" and for broken slaves also "8n1" (which is not allowed by the standard)

  void setTimeoutsForBaudrate(uint32_t baudrate); ///< use this, if serial port baudrate is managed outside of this class
  void setTimeouts_ms(uint32_t txGuardTime_ms, uint32_t rxSplitLimit_ms); ///< set timeouts explicitly, normally they depend on the baudrate in use, see also @ref setTimeoutsForBaudrate()
  void getTimeouts_ms(uint32_t &txGuardTime_ms, uint32_t &rxSplitLimit_ms);

  ::std::weak_ptr<SlaveProxy> getSlave(uint8_t slaveId);

  bool hasPosixError() const;
  int getErrno() const;

protected:
  ::Hardware::Linux::TtyUart* uartStream;
  // TODO: evtl. durch MillisecondCounterTimer ersetzen, um auf die Restlaufzeit des mbMasterTimers zugreifen zu können
  PosixTimer mbMasterTimer, txGuardTimer, rxSplitTimer;
  SerialMaster<Rtu> modbusMaster;
  ::std::map<uint8_t, ::std::shared_ptr<SlaveProxy> > slaves;
  ::std::map<uint8_t, ::std::shared_ptr<SlaveProxy> >::iterator nextStartSlave;
  const CalleeMember<FdSelectRtuDriver, void, bool> txSignal, rxSignal;

  int myErrno;

  bool destroyed:1;
  bool slavesPushMaster:1;
private:
   void execute(bool readable, bool writable) final;  ///< this implements the workhorse for reacting to readable and writable I/O
   int fd() final;  ///< this must return the fd of object

   void privEnableTx(bool enable);
   void privEnableRx(bool enable);
};

} /* namespace Modbus */

#endif /* FDSELECTRTUDRIVER_H_ */
