/*
 * ModbusReadBits.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSREADBITS_H_
#define MODBUSREADBITS_H_

#include "ModbusTxn.h"

namespace Modbus
{

class ReadBits2Array: public Modbus::Txn
{
public:
  explicit
  ReadBits2Array(uint8_t functionCode, unsigned* array, uint16_t numOfBits);
//  virtual ~ReadBits2Array();

  void readTo(unsigned* array, uint16_t numOfBits);
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
  unsigned* bitBuffer;
  uint16_t bufferLength;
};

} /* namespace Modbus */
#endif /* MODBUSREADBITS_H_ */
