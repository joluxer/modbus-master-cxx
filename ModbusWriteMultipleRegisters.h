/*
 * ModbusWriteMultipleRegisters.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSWRITEMULTIPLEREGISTERS_H_
#define MODBUSWRITEMULTIPLEREGISTERS_H_

#include "ModbusTxn.h"

namespace Modbus
{

class WriteMultipleRegistersFromArray: public Modbus::Txn
{
public:
  WriteMultipleRegistersFromArray(const uint16_t* array, uint16_t numOfRegs, TxnReturnPath* rp = nullptr);
//  virtual ~WriteMultipleRegistersFromArray();

  void readFrom(const uint16_t* array, uint16_t numOfRegs);
  void setStartAddr(uint16_t addr);

  virtual
  uint8_t getTxLength() override;

  virtual
  bool getTxData(PduConstDataBuffer& txData, unsigned offset) override;

  virtual
  unsigned processRxData(const PduConstDataBuffer& rxData) override;

protected:
  static const uint8_t FunctionCode = 0x10;
  static const unsigned TxPduLength = 6;
  uint8_t txPduHeader[TxPduLength];
  uint8_t txByteBuffer[sizeof(uint16_t)];
  const uint16_t* regsBuffer;
  uint16_t bufferLength;
};

} /* namespace Modbus */
#endif /* MODBUSWRITEMULTIPLEREGISTERS_H_ */
