/*
 * ModbusSlaveProxy.h
 *
 *  Created on: 06.02.2016
 *      Author: lode
 */

#ifndef MODBUSSLAVEPROXY_H_
#define MODBUSSLAVEPROXY_H_

#include "ModbusTxn.h"
#include "TxnReturnPath.h"
#include "ModbusAbstractMaster.h"

#include <memory>
#include <cstdint>

namespace Modbus
{

class SlaveProxy
{
public:
  explicit
  SlaveProxy(uint8_t slaveId, bool pushMaster = true);
//  virtual ~SlaveProxy();

  void connectVia(AbstractMaster& connection);

  bool runQueue();  ///< @return true while busy, indicating more transactions waiting in the queue, thus waiting for the master to become idle

  bool enqueueNoDelete(Txn& txn); ///< @return true on success, suppress delete by smart ptr
  bool enqueueDynamic(Txn* txn);  ///< @return true on success, allow delete by smart ptr
  bool enqueue(std::unique_ptr<Txn>&& txn);  ///< @return true on success, no change of delete by unique_ptr
  unsigned queueLength() const; ///< @return the sum of waiting and running and pending TXNs
  std::unique_ptr<Txn> dequeueResult(); ///< @return a non-empty pointer, when a result is available and not yet returned to sender

  uint8_t slaveId() const;

protected:
  const uint8_t mySlaveId;
  AbstractMaster* master;

  unsigned pendingCount;
  Txn::Fifo txnQueue;
  Txn::Fifo resultQueue;

  bool pushMaster:1;
private:
};

inline
unsigned SlaveProxy::queueLength() const
{
  return pendingCount;
}

inline
uint8_t SlaveProxy::slaveId() const
{
  return mySlaveId;
}

} /* namespace Modbus */
#endif /* MODBUSSLAVEPROXY_H_ */
