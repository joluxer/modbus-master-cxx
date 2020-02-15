/*
 * TxnReturnPath.cpp
 *
 *  Created on: 05.02.2020
 *      Author: lode
 */

#include "TxnReturnPath.h"

namespace Modbus
{

TxnReturnPath::TxnReturnPath()
: pendingCount(0)
{}

TxnReturnPath::~TxnReturnPath()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

void TxnReturnPath::txnComingHome(::std::unique_ptr<Txn>&& txn)
{
  if (txn->returnPath)
    -- txn->returnPath->pendingCount;

  returnedList.push(::std::move(txn));
}

} /* namespace Modbus */
