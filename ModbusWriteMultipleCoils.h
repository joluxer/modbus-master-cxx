/*
 * ModbusWriteMultipleCoils.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSWRITEMULTIPLECOILS_H_
#define MODBUSWRITEMULTIPLECOILS_H_

#include "ModbusTxn.h"

namespace Modbus
{

class WriteMultipleCoilsFromArray: public Modbus::Txn
{
public:
  WriteMultipleCoilsFromArray(const unsigned* array, uint16_t numOfBits);
//  virtual ~WriteMultipleCoilsFromArray();

  void readFrom(const unsigned* array, uint16_t numOfBits);
  void setStartAddr(uint16_t addr);

  virtual
  uint8_t getTxLength() override;

  virtual
  bool getTxData(PduConstDataBuffer& txData, unsigned offset) override;

  virtual
  unsigned processRxData(const PduConstDataBuffer& rxData) override;

protected:
  static const uint8_t FunctionCode = 0x0f;
  static const unsigned TxPduHeaderLength = 6;
  const unsigned* bitBuffer;
  uint16_t bufferLength;
  uint16_t numOfBits;
  uint8_t txPduHeader[TxPduHeaderLength];
  uint8_t txBitBuffer[sizeof(bitBuffer[0])];
};

} /* namespace Modbus */
#endif /* MODBUSWRITEMULTIPLECOILS_H_ */
