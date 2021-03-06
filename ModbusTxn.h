/*
 * ModbusTxn.h
 *
 *  Created on: 05.02.2016
 *      Author: lode
 */

#ifndef MODBUSTXN_H_
#define MODBUSTXN_H_

#include "ModbusTypes.h"

#include "ByteStream.h"

#include <memory>

namespace Modbus
{
class Txn;
}

namespace std
{
template<>
class default_delete< Modbus::Txn >
{
public:
  void operator()(Modbus::Txn *ptr) const;
};
}

//#ifndef DEBUG_MB_TXNPATH
//#define DEBUG_MB_TXNPATH
//#endif

//#ifdef DEBUG_MB_TXNPATH
//#undef DEBUG_MB_TXNPATH
//#endif

namespace Modbus
{

class TxnReturnPath;
class SlaveProxy;
class AbstractMaster;

/**
 * Diese Klasse ist die Basis-Klasse für Transaktionen am Modbus. Die Verhaltensimplementationen
 * für verschiedene Function-Codes der Modbus-Kommunikation sind von dieser Basis-Klasse abzuleiten.
 */
class Txn
{
public:
  virtual ~Txn();

  bool isBusy() const;
  ResultCode getResultCode() const;
  uint8_t getSlaveId() const;
  uint8_t getFunctionCode() const;
  uint8_t exceptionCode() const;
  bool hasTxnId(uint16_t id) const;

  virtual
  uint8_t getTxLength() =0;

  virtual
  bool getTxData(PduConstDataBuffer& txData, unsigned offset) =0;

  virtual
  unsigned processRxData(const PduConstDataBuffer& rxData) =0; ///< @return the amount of processed data

  void setTxnId(uint16_t id);
  void setResultCode(ResultCode c);

  bool hasReturnPath() const;

  class Fifo
  {
  public:
#ifdef DEBUG_MB_TXNPATH
    Fifo(const std::type_info& messageClass, void* owner)
    : qBack{&qHead}, listOwner(owner), ownerType(messageClass.name())
    {};
#else
    Fifo() : qBack{&qHead} {};
#endif

    void push(std::unique_ptr<Txn>&& t);
    std::unique_ptr<Txn> pop();

    operator bool() const;

  protected:
    std::unique_ptr<Txn> qHead, *qBack;
#ifdef DEBUG_MB_TXNPATH
    void* listOwner;
    const char* ownerType;
#endif
  };

  class List: public Fifo
  {
  public:
    using Fifo::Fifo;

    std::unique_ptr<Txn> pullBy(uint8_t slaveId);
    std::unique_ptr<Txn> pullBy(uint16_t txnId);
    unsigned length() const;
  };

  static
  ByteStream *logDebug;

protected:
  explicit
  Txn(uint8_t functionCode, TxnReturnPath* rp);

  uint8_t const functionCode;
  uint8_t slaveId;  // wird bei der Übergabe an ein SlaveProxy-Objekt festgelegt
  uint16_t txnId;   // should be set on the handover to a connection object, if the transport needs this, is nessessary to pull pending transactions form the pending list
  ResultCode resultCode;
  TxnReturnPath* returnPath;

private:
  std::unique_ptr<Txn> qNext;

  bool busy:1; // wird gesetzt, wenn das Objekt an einen SlaveProxy oder einen Master (für Broadcast) übergeben wurde,
  bool autoDiscard:1; // wenn dieses Flag gesetzt ist, wird das Objekt automatisch freigegeben, es braucht nicht wieder beim SlaveProxy abgeholt werden.
  bool allowDeleteBySmartPtr:1;  // wird vom SlaveProxy gesetzt/gelöscht, wenn es über equeueNoDelete()/enqueueDynamic() in Umlauf gebracht wird.

#ifdef DEBUG_MB_TXNPATH
  Fifo* fifo;
#endif

  friend class TxnReturnPath;
  friend class SlaveProxy;
  friend class AbstractMaster;
  friend class std::default_delete< Txn >;
};

} /* namespace Modbus */

namespace Modbus
{

inline
bool Txn::isBusy() const
{
  return busy;
}

inline
ResultCode Txn::getResultCode() const
{
  return resultCode;
}

inline
uint8_t Txn::getSlaveId() const
{
  return slaveId;
}

inline
uint8_t Txn::getFunctionCode() const
{
  return functionCode;
}

inline
uint8_t Txn::exceptionCode() const
{
  return functionCode | 0x80;
}

inline
bool Txn::hasTxnId(uint16_t id) const
{
  return (txnId == id);
}

inline
Txn::Fifo::operator bool() const
{
  return bool(qHead);
}

inline
bool Txn::hasReturnPath() const
{
  return !!returnPath;
}

} /* namespace Modbus */

#endif /* MODBUSTXN_H_ */
