/*
 * ModbusRtuMaster.h
 *
 *  Created on: 15.02.2016
 *      Author: lode
 */

#ifndef MODBUSRTU_H_
#define MODBUSRTU_H_

#include "ModbusTxn.h"

#include "ByteStream.h"

#include <cstdint>
#include <memory>

#ifndef MODBUS_CRC_ALGORITHM_BITSHIFT
#ifndef MODBUS_CRC_ALGORITHM_TABLE
#define MODBUS_CRC_ALGORITHM_TABLE
#endif
#endif

namespace Modbus
{

class Rtu
{
public:
  Rtu();
//  virtual ~Rtu();

  static
  uint32_t getTxGuardTime_ms(uint32_t baudrate);
  static
  uint32_t getSplitLimit_ms(uint32_t baudrate);

  uint8_t startTx(ByteStream* line, const ::std::unique_ptr<Txn>& runningTxn);
  uint8_t runTx(ByteStream* line, const ::std::unique_ptr<Txn>& runningTxn, uint8_t offset);
  bool    sendChecksum(ByteStream* line); // return true, while sending
  void    startRx();
  unsigned runRx(ByteStream* line, uint8_t* buffer, const size_t bufferSpace); // return numbe rof RX bytes
  static constexpr
  bool    isEndOfRxFrame();
  static constexpr
  bool    restartRxFrame();
  bool    testChecksumIsGood(uint8_t* buffer, uint8_t bufferFill);
  static
  PduConstDataBuffer getPdu(uint8_t* buffer, uint8_t bufferFill, uint8_t offset);

protected:
  uint16_t pduCrc;
  uint8_t crcTxOffset;

  void resetCrc();
  void updateCrc(uint8_t const* data, unsigned len);
};

inline
void Rtu::resetCrc()
{
  pduCrc = uint16_t(~0);
}

inline
bool    Rtu::sendChecksum(ByteStream* line)
{
  switch(crcTxOffset)
  {
  case 0:
    crcTxOffset += line->write(uint8_t(pduCrc >> 0));
    // no break
  case 1:
    crcTxOffset += line->write(uint8_t(pduCrc >> 8));
    // no break
  default:
    ;
    // no break
  }

  return (crcTxOffset < 2);
}

inline constexpr
bool Rtu::isEndOfRxFrame()
{
  // cannot detect end-of-frame here
  return false;
}

inline constexpr
bool Rtu::restartRxFrame()
{
  // cannot detect restart-of-frame here
  return false;
}

inline
bool Rtu::testChecksumIsGood(uint8_t* buffer, uint8_t bufferFill)
{
  if (bufferFill > 2)
  {
    pduCrc = ~0;
    updateCrc(buffer, bufferFill - 2);

    decltype(pduCrc) const bufferCrc = (buffer[bufferFill - 2]  << 0) | (buffer[bufferFill - 1] << 8);

    return (bufferCrc == pduCrc);
  }

  return false;
}

} /* namespace Modbus */
#endif /* MODBUSRTU_H_ */
