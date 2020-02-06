#/*
 * ModbusTxn.cpp
 *
 *  Created on: 05.02.2016
 *      Author: lode
 */

#include "ModbusTxn.h"

#include <cassert>


namespace std
{

//template<>
void default_delete<Modbus::Txn>::operator()(Modbus::Txn *ptr) const
{
  if (ptr && ptr->allowDeleteBySmartPtr)
    delete ptr;
}

}

namespace Modbus
{

Txn::Txn(uint8_t functionCode, TxnReturnPath* rp)
: functionCode(functionCode),
  slaveId(0xff),
  txnId(0),
  resultCode(NoError),
  returnPath(rp),
  busy(false),
  autoDiscard(false),
  allowDeleteBySmartPtr(false)
{}

Txn::~Txn()
{
  assert(!busy);
}

void Txn::setTxnId(uint16_t id)
{
  txnId = id;
}

void Txn::setResultCode(ResultCode c)
{
  resultCode = c;
}

void Txn::Fifo::push(std::unique_ptr<Txn>&& txn)
{
  if (txn)
  {
    assert(!txn->qNext);

    *qBack = std::move(txn);
    qBack = &(qBack->get()->qNext);
  }
}

std::unique_ptr<Txn> Txn::Fifo::pop()
{
  auto txn = std::move(qHead);

  if (txn)
  {
    qHead = std::move(txn->qNext);

    if (!qHead)
      qBack = &qHead;
  }

  return txn;
}

unsigned Txn::List::length() const
{
  auto ptr = qHead.get();
  unsigned count = 0;

  while (ptr)
  {
    ++count;
    ptr = ptr->qNext.get();
  }

  return count;
}

std::unique_ptr<Txn> Txn::List::pullBy(uint8_t id)
{
  std::unique_ptr<Txn> retVal;

  if (qHead)
  {
    if (qHead.get()->slaveId == id)
    {
      retVal = std::move(qHead);
      qHead = std::move(retVal->qNext);

      if (!qHead)
        qBack = &qHead;
    }
    else
    {
      auto prevPtrPtr = &qHead;

      while (prevPtrPtr->get()->qNext && (prevPtrPtr->get()->qNext->slaveId != id))
        prevPtrPtr = &(prevPtrPtr->get()->qNext);

      if (prevPtrPtr->get()->qNext->slaveId == id)
      {
        retVal = std::move(prevPtrPtr->get()->qNext);
        prevPtrPtr->get()->qNext = std::move(retVal->qNext);
        if (!prevPtrPtr->get()->qNext)
          qBack = &(prevPtrPtr->get()->qNext);
      }
    }
  }

  return retVal;
}

std::unique_ptr<Txn> Txn::List::pullBy(uint16_t id)
{
  std::unique_ptr<Txn> retVal;

  if (qHead)
  {
    if (qHead.get()->hasTxnId(id))
    {
      retVal = std::move(qHead);
      qHead = std::move(retVal->qNext);

      if (!qHead)
        qBack = &qHead;
    }
    else
    {
      auto prevPtrPtr = &qHead;

      while (prevPtrPtr->get()->qNext && not prevPtrPtr->get()->qNext->hasTxnId(id))
        prevPtrPtr = &(prevPtrPtr->get()->qNext);

      if (prevPtrPtr->get()->qNext->hasTxnId(id))
      {
        retVal = std::move(prevPtrPtr->get()->qNext);
        prevPtrPtr->get()->qNext = std::move(retVal->qNext);
        if (!prevPtrPtr->get()->qNext)
          qBack = &(prevPtrPtr->get()->qNext);
      }
    }
  }

  return retVal;
}

} /* namespace Modbus */
