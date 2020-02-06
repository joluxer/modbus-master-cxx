/*
 * ModbusWriteSingleRegister.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSWRITESINGLEREGISTER_H_
#define MODBUSWRITESINGLEREGISTER_H_

#include "ModbusTxn.h"

namespace Modbus
{

class WriteSingleRegister: public Modbus::Txn
{
public:
  explicit
  WriteSingleRegister(uint16_t addr, TxnReturnPath* rp = nullptr);
//  virtual ~WriteSingleRegister();

  WriteSingleRegister& setAddr(uint16_t addr);
  WriteSingleRegister& setWriteValue(uint16_t val);

  virtual
  uint8_t getTxLength() override;

  virtual
  bool getTxData(PduConstDataBuffer& txData, unsigned offset) override;

  virtual
  unsigned processRxData(const PduConstDataBuffer& rxData) override;

protected:
  explicit
  WriteSingleRegister(uint8_t functionCode, uint16_t addr, TxnReturnPath* rp);

  static const uint8_t FunctionCode = 0x06;
  static const unsigned PduLength = 5;

  uint8_t pduBuffer[PduLength];
};

} /* namespace Modbus */
#endif /* MODBUSWRITESINGLEREGISTER_H_ */
