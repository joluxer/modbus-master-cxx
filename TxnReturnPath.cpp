/*
 * TxnReturnPath.cpp
 *
 *  Created on: 05.02.2020
 *      Author: lode
 */

#include "TxnReturnPath.h"

namespace Modbus
{

//TxnReturnPath::TxnReturnPath()
//{
//  // Auto-generated constructor stub
//
//}

TxnReturnPath::~TxnReturnPath()
{
  // Auto-generated destructor stub
  // intentionally left empty
}

void TxnReturnPath::txnComingHome(::std::unique_ptr<Txn>&& txn)
{
  returnedList.push(::std::move(txn));
}

} /* namespace Modbus */
