/*
 * ModbusReadRegisters.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADREGISTERS_H_
#define MODBUSREADREGISTERS_H_

#include "ModbusTxn.h"

namespace Modbus
{

class ReadRegisters2Array: public Modbus::Txn
{
public:
  ReadRegisters2Array(uint8_t functionCode, uint16_t* array, uint16_t numOfRegs);
//  virtual ~ReadRegisters2Array();

  void readTo(uint16_t* array, uint16_t numOfRegs);
  void setStartAddr(uint16_t addr);

  virtual
  uint8_t getTxLength() override;

  virtual
  bool getTxData(PduConstDataBuffer& txData, unsigned offset) override;

  virtual
  unsigned processRxData(const PduConstDataBuffer& rxData) override;

protected:
  static const unsigned TxPduLength = 5;
  uint8_t txPduBuffer[TxPduLength];
  uint16_t* regsBuffer;
  uint16_t bufferLength;
};

} /* namespace Modbus */
#endif /* MODBUSREADREGISTERS_H_ */
