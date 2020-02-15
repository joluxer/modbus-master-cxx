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

class SlaveProxy;

/**
 * This class is the interface, how to return completed transactions automatically,
 * if an transaction is provided with an return path.
 *
 */
class TxnReturnPath
{
public:
  TxnReturnPath();
  virtual
  ~TxnReturnPath();

  virtual
  void txnComingHome(::std::unique_ptr<Txn>&& txn); ///< The default implementation pushes the txn to the returned list to be processed further from there

  int getPendingCount() const;

protected:
  Txn::List returnedList;
  int pendingCount;

  friend class SlaveProxy;
};

inline
int TxnReturnPath::getPendingCount() const
{
  return pendingCount;
}


} /* namespace Modbus */

#endif /* MODBUSTXNRETURNPATH_H_ */
