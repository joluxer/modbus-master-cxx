/*
 * ModbusAbstractMaster.h
 *
 *  Created on: 05.02.2016
 *      Author: lode
 */

#ifndef MODBUSABSTRACTMASTER_H_
#define MODBUSABSTRACTMASTER_H_

#include "ModbusTxn.h"
#include "AbstractTimer.h"

#include <memory>

namespace Modbus
{

class AbstractMaster
{
public:
  virtual ~AbstractMaster();

  void setResponseTimeout_ms(uint32_t ms);
  void setTurnaroundDelay_ms(uint32_t ms);

  enum OperationStateBits
  {
    BusyWaitingTxHardware = 1,  // need operate() call as soon as more TX data is accepted
    BusyWaitingRxHardware = 2,  // need operate() call as soon as more RX data is available or for timeout polling
    BusyWaitingTurnaround = 4,  // need operate() call after medium time, after expiration of masterTimer
  };

  unsigned operate(); ///< @return not zero while busy, meaning: need an other call soon to drive something

  bool readyForNextTxn() const;

  bool startTxn(std::unique_ptr<Txn>&& txn);

  std::unique_ptr<Txn> getCompletedTxn(uint8_t slaveId);

  void addToList(AbstractMaster* &listHead);
  void removeFromList(AbstractMaster* &listHead);

  static const uint32_t DefaultResponseTimeout_ms = 5000;
  static const uint32_t DefaultTurnaroundDelay_ms = 200;

protected:
  explicit
  AbstractMaster(AbstractTimer& masterTimer);

  uint32_t turnaroundDelay_ms;
  uint32_t responseTimeout_ms;
  std::unique_ptr<Txn> runningTxn;
  Txn::List pendingList;
  Txn::List completedList;

  AbstractTimer& responseTimer;

  bool broadcastRunning:1;

  bool handledAsException(const PduConstDataBuffer& rxData, uint16_t txnId = ~0);  ///< @return true on exception found and move TXN in question to the completedList. If there is a runningTxn, it is matched against this, otherwise looked up in the pendingList.

  virtual
  bool startTx() =0;  ///< @return true on success, @note moves runningTxn to pendingList, when ready for next transmission or to completedList in case of error

  virtual
  bool runTx() =0;    ///< @return true while busy waiting for TX data space, @note moves runningTxn to pendingList, when ready for next transmission or to completedList in case of error

  virtual
  bool runRx() =0; ///< @return true while busy waiting for more RX data, @note moves runningTxn or pendingList element to completedList on completion

private:
  AbstractMaster* listNext;



  friend class Master;
};

} /* namespace Modbus */
#endif /* MODBUSABSTRACTMASTER_H_ */
