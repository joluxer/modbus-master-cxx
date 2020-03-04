/*
 * ModbusSlaveProxy.cpp
 *
 *  Created on: 06.02.2016
 *      Author: lode
 */

#include "ModbusSlaveProxy.h"

#include "FormattedLog.h"

#include <cassert>

namespace Modbus
{

ByteStream *SlaveProxy::logDebug(nullptr);
ByteStream *SlaveProxy::logError(nullptr);

SlaveProxy::SlaveProxy(uint8_t slaveId, bool pushMaster)
: mySlaveId(slaveId), master(nullptr),
  pendingCount(0),
#ifdef DEBUG_MB_TXNPATH
  txnQueue(typeid(*this), this),
  resultQueue(typeid(*this), this),
#endif
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
      {
        if (not txn->hasReturnPath())
          resultQueue.push(std::move(txn));
        else
        {
          auto rp = uintptr_t(txn->returnPath);
          auto txnp = uintptr_t(txn.get());
          fLog(logDebug, "pushing TXN 0x%08x to return path 0x%08x: slave ID %d, function code %d", txnp, rp, mySlaveId, txn->getFunctionCode());
          txn->busy = false;
          --pendingCount;
          txn->returnPath->txnComingHome(::std::move(txn));
        }
      }
      else
      {
        txn->busy = false;
        --pendingCount;
      }
    }
  }

  return busy;
}

bool SlaveProxy::enqueueNoDelete(Txn& txn)
{
  txn.allowDeleteBySmartPtr = false;
  auto txnPtr = std::unique_ptr<Txn>(&txn);

  return enqueue(std::move(txnPtr));
}

bool SlaveProxy::enqueueDynamic(Txn* txn)
{
  txn->allowDeleteBySmartPtr = true;
  auto txnPtr = std::unique_ptr<Txn>(txn);

  return enqueue(std::move(txnPtr));
}

bool SlaveProxy::enqueue(std::unique_ptr<Txn>&& txn)
{
  bool success = false;

  if (txn)
  {
    if (not txn->busy)
    {
      // coutn the messages pending for the given returnPath
      if (txn->returnPath)
        ++ txn->returnPath->pendingCount;

      if (master)
      {
        txn->busy = true;
        txn->slaveId = mySlaveId;
        auto rp = uintptr_t(txn->returnPath);
        auto txnp = uintptr_t(txn.get());
        if (rp)
          fLog(logDebug, "TXN 0x%08x enqueued with return path 0x%08x: slave ID %d, function code %d", txnp, rp, mySlaveId, txn->getFunctionCode());
        else
          fLog(logDebug, "TXN 0x%08x enqueued: slave ID %d, function code %d", mySlaveId, txnp, txn->getFunctionCode());
        txnQueue.push(std::move(txn));

        if (master->readyForNextTxn())
          master->startTxn(txnQueue.pop());
        success = true;
      }
      else
      {
        fLog(logDebug, "enqueue() rejected due to missing master connection; slave ID %d", mySlaveId);
        txn->setResultCode(ProxyHasNoConnection);
        resultQueue.push(std::move(txn));
        success = true;
      }
    }
    else
    {
      // logging of rejected TXNs because of busy flag
      fLog(logError, "enqueue() not possible for busy TXN, slave ID %d, function code %d", mySlaveId, txn->getFunctionCode());
    }
  }
  else
  {
    fLog(logDebug, "enqueue() with empty TXN pointer, slave ID %d", mySlaveId);
  }

  if (success)
    ++pendingCount;

  txn = master->getCompletedTxn(mySlaveId);
  if (txn)
  {
    if (not txn->autoDiscard)
    {
      if (not txn->hasReturnPath())
        resultQueue.push(std::move(txn));
      else
      {
        auto rp = uintptr_t(txn->returnPath);
        auto txnp = uintptr_t(txn.get());
        fLog(logDebug, "pushing TXN 0x%08x to return path 0x%08x: slave ID %d, function code %d", txnp, rp, mySlaveId, txn->getFunctionCode());
        txn->busy = false;
        --pendingCount;
        txn->returnPath->txnComingHome(::std::move(txn));
      }
    }
    else
    {
      txn->busy = false;
      --pendingCount;
    }
  }

  return success;
}

std::unique_ptr<Txn> SlaveProxy::dequeueResult()
{
  auto txn = resultQueue.pop();

  if (txn)
  {
    auto txnp = uintptr_t(txn.get());
    fLog(logDebug, "TXN 0x%08x dequeued: slave ID %d, function code %d", txnp, mySlaveId, txn->getFunctionCode());
    assert(pendingCount);
    --pendingCount;
    txn->busy = false;
  }

  return txn;
}

} /* namespace Modbus */
