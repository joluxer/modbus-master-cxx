/*
 * ModbusConnection.cpp
 *
 *  Created on: 05.02.2016
 *      Author: lode
 */

#include "ModbusAbstractMaster.h"

#include <cassert>

namespace Modbus
{

AbstractMaster::AbstractMaster(AbstractTimer& masterTimer)
: turnaroundDelay_ms(DefaultTurnaroundDelay_ms),
  responseTimeout_ms(DefaultResponseTimeout_ms),
  responseTimer(masterTimer),
  broadcastRunning(false),
  listNext(nullptr)
{}

AbstractMaster::~AbstractMaster()
{
  // Auto-generated destructor stub
}

void AbstractMaster::setTurnaroundDelay_ms(uint32_t ms)
{
  turnaroundDelay_ms = ms;
}

void AbstractMaster::setResponseTimeout_ms(uint32_t ms)
{
  responseTimeout_ms = ms;
}


unsigned AbstractMaster::operate()
{
  unsigned opState = 0;

  if (runTx())
    opState |= BusyWaitingTxHardware;

  if (!broadcastRunning)
  {
    if (responseTimer.isActive())
    {
      if (runRx())
        opState |= BusyWaitingRxHardware;
    }
    else
    {
      // Timeout, laufende TXNs mit Timeout beenden
      if (runningTxn)
      {
        runningTxn->setResultCode(CommunicationTimeout);
        completedList.push(std::move(runningTxn));
      }

      while (pendingList)
      {
        auto txn = pendingList.pop();
        txn->setResultCode(CommunicationTimeout);
        completedList.push(std::move(txn));
      }
    }
  }
  else
  {
    if (responseTimer.isActive())
      opState |= BusyWaitingTurnaround;
    else
      broadcastRunning = false;
  }

  return opState;
}

bool AbstractMaster::readyForNextTxn() const
{
  return (!runningTxn && (!broadcastRunning || !responseTimer.isActive()));
}

bool AbstractMaster::startTxn(std::unique_ptr<Txn>&& txn)
{
  bool success = false;

  if (txn)
  {
    if (readyForNextTxn())
    {
      runningTxn = std::move(txn);

      bool broadcast = runningTxn->getSlaveId() == 0;
      success = startTx();

      if (success)
      {
        broadcastRunning = broadcast;

        if (broadcast)
          responseTimer.start(turnaroundDelay_ms);
        else
          responseTimer.start(responseTimeout_ms);
      }
      else
      {
        broadcastRunning = false;
        runningTxn->setResultCode(UnknownCommunicationError);
        completedList.push(std::move(runningTxn));
      }
    }
    else
    {
      txn->setResultCode(MasterNotReady);
      completedList.push(std::move(txn));
    }
  }

  return success;
}

std::unique_ptr<Txn> AbstractMaster::getCompletedTxn(uint8_t slaveId)
{
  auto txn = completedList.pullBy(slaveId);

  if (0 == slaveId)
    txn->busy = false;

  return txn;
}

void AbstractMaster::addToList(AbstractMaster* &listHead)
{
  assert(!listNext && "this object is already a list member, must not added again");

  if (!listHead)
    listHead = this;
  else
  {
    auto lst = listHead;

    while (lst->listNext)
      lst = lst->listNext;

    lst->listNext = this;
  }
}

void AbstractMaster::removeFromList(AbstractMaster* &listHead)
{
  if (listHead)
  {
    if (this == listHead)
    {
      listHead = listNext;
      listNext = nullptr;
    }
    else
    {
      auto lst = listHead;
      while (lst && (this != lst->listNext))
        lst = listNext;

      if (this == lst->listNext)
      {
        lst->listNext = this->listNext;
        this->listNext = nullptr;
      }
    }
  }
}

bool AbstractMaster::handledAsException(const PduConstDataBuffer& rx, uint16_t txnId)
{
  bool isException = false;

  if (rx.data && (rx.data[0] & 0x80))
  {
    isException = true;
    // decode exception and move txn in question
    auto txn = std::move(runningTxn);

    if (txn)
    {
      if (txn->exceptionCode() != rx.data[0])
        runningTxn = std::move(txn);
    }
    else
    {
      txn = pendingList.pullBy(txnId);
      if (txn->exceptionCode() != rx.data[0])
        pendingList.push(std::move(txn));
    }

    if (txn)
    {
      txn->setResultCode(ResultCode(rx.data[1]));
      completedList.push(std::move(txn));
    }

    // TODO: logging of rejected exception data
  }

  return isException;
}

} /* namespace Modbus */
