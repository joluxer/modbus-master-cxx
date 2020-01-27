/*
 * Ascii.h
 *
 *  Created on: 24.01.2020
 *      Author: lode
 */

#ifndef MODBUSASCII_H_
#define MODBUSASCII_H_

namespace Modbus
{

/*
 *
 */
class Ascii
{
public:
  Ascii();
//  virtual ~Ascii();

  static
  uint32_t getTxGuardTime_ms(uint32_t baudrate);
  static
  uint32_t getSplitLimit_ms(uint32_t baudrate);

  uint8_t startTx(ByteStream* line, const ::std::unique_ptr<Txn>& runningTxn);
  uint8_t runTx(ByteStream* line, const ::std::unique_ptr<Txn>& runningTxn, uint8_t offset);
  bool    sendChecksum(ByteStream* line); // return true, while sending
  void    startRx();
  unsigned runRx(ByteStream* line, uint8_t* buffer, const size_t bufferSpace); // return numbe rof RX bytes
  bool    isEndOfRxFrame();
  bool    restartRxFrame();
  bool    testChecksumIsGood(uint8_t* buffer, uint8_t bufferFill);

protected:
  uint16_t pduLrc;
  char hexDigit1;

  bool waitForHexDigit2:1;
  bool restartRxFrameFlag:1;
  bool endOfRxFrameFound:1;

  void resetLrc();
  void updateLrc(uint8_t const* data, unsigned len);
};

} /* namespace Modbus */

#endif /* MODBUSASCII_H_ */
