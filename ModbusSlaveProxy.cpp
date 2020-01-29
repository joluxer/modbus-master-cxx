/*
 * ModbusSlaveProxy.cpp
 *
 *  Created on: 06.02.2016
 *      Author: lode
 */

#include "ModbusSlaveProxy.h"

#include <cassert>

namespace Modbus
{

SlaveProxy::SlaveProxy(uint8_t slaveId, bool pushMaster)
: mySlaveId(slaveId), master(nullptr),
  pendingCount(0),
  pushMaster(pushMaster)
{}

//SlaveProxy::~SlaveProxy()
//{
//  // Auto-generated destructor stub
//}

void SlaveProxy::connectVia(AbstractMaster& connection)
{
  master = &connection;
}

bool SlaveProxy::runQueue()
{
  bool busy = false;

  if (master)
  {
    if (master->readyForNextTxn() && txnQueue)
      master->startTxn(txnQueue.pop());

    if (pushMaster)
      busy = !!master->operate();

    if (txnQueue)
      busy = true;

    auto txn = master->getCompletedTxn(mySlaveId);
    if (txn)
    {
      if (not txn->autoDiscard)
        resultQueue.push(std::move(txn));
      else
        --pendingCount;
    }
  }

  return busy;
}

bool SlaveProxy::enqueueNoDelete(Txn& txn)
{
  txn.allowDeleteByUniquePtr = false;
  auto txnPtr = std::unique_ptr<Txn>(&txn);

  return enqueue(std::move(txnPtr));
}

bool SlaveProxy::enqueueDynamic(Txn& txn)
{
  txn.allowDeleteByUniquePtr = true;
  auto txnPtr = std::unique_ptr<Txn>(&txn);

  return enqueue(std::move(txnPtr));
}

bool SlaveProxy::enqueue(std::unique_ptr<Txn>&& txn)
{
  bool success = false;

  if (txn && not txn->busy)
  {
    if (master)
    {
      txn->busy = true;
      txn->slaveId = mySlaveId;
      txnQueue.push(std::move(txn));

      if (master->readyForNextTxn())
        master->startTxn(txnQueue.pop());
      success = true;
    }
    else
    {
      txn->setResultCode(ProxyHasNoConnection);
      resultQueue.push(std::move(txn));
      success = true;
    }
  }
  // TODO: logging of rejected TXNs because of busy flag

  if (success)
    ++pendingCount;

  txn = master->getCompletedTxn(mySlaveId);
  if (txn)
  {
    if (not txn->autoDiscard)
      resultQueue.push(std::move(txn));
    else
      --pendingCount;
  }

  return success;
}

std::unique_ptr<Txn> SlaveProxy::dequeueResult()
{
  auto result = resultQueue.pop();

  if (result)
  {
    assert(pendingCount);
    --pendingCount;
    result->busy = false;
  }


  return result;
}

} /* namespace Modbus */
