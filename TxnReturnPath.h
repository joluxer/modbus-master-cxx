/*
 * TxnReturnPath.h
 *
 *  Created on: 05.02.2020
 *      Author: lode
 */

#ifndef MODBUSTXNRETURNPATH_H_
#define MODBUSTXNRETURNPATH_H_

#include "ModbusTxn.h"

#include <memory>

namespace Modbus
{

/**
 * This class is the interface, how to return completed transactions automatically,
 * if an transaction is provided with an return path.
 *
 */
class TxnReturnPath
{
public:
//  TxnReturnPath();
  virtual
  ~TxnReturnPath();

  virtual
  void txnComingHome(::std::unique_ptr<Txn>&& txn);

protected:
  Txn::List returnedList;
};

} /* namespace Modbus */

#endif /* MODBUSTXNRETURNPATH_H_ */
