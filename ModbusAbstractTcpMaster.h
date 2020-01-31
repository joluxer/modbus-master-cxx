/*
 * ModbusAbstractTcpMaster.h
 *
 *  Created on: 12.02.2016
 *      Author: lode
 */

#ifndef MODBUSABSTRACTTCPMASTER_H_
#define MODBUSABSTRACTTCPMASTER_H_

#include "ModbusAbstractMaster.h"

#include "pt/pt.h"

namespace Modbus
{

class AbstractTcpMaster: public Modbus::AbstractMaster
{
public:
//  virtual ~AbstractTcpMaster();

  static const uint16_t DefaultTcpPort = 502;

protected:
  AbstractTcpMaster(AbstractTimer& masterTimer);

  void disconnectTxns(); // this must be called before changing the server address

  virtual
  bool tcpIsConnected() =0;

  virtual
  bool setupTcpConnection() =0;

  virtual
  uint16_t getTcpTxSpace() =0;

  virtual
  unsigned queueTcpTxData(const unsigned char* dataBuffer, uint16_t dataLength) =0; ///< return number of actual data amount was queued

  virtual
  void startTcpTx() =0;

  virtual
  uint16_t getTcpRxLengthAvailable() =0;

  virtual
  uint16_t readTcpRxData(unsigned char* dataBuffer, unsigned short bufferSize) =0; ///< return the amount of data actually read

  virtual
  void shutdownTcpConnection() =0;

private:
  uint16_t serverPort;
  uint8_t numberMaxOfClientTransaction;

  uint16_t txnCounter;

  uint8_t mbaphdTxBuffer[7];
  uint8_t mbaphdRxBuffer[7];
  uint8_t rxBuffer[255];
  std::unique_ptr<Txn> rxTxn;

  struct pt rxState;
  unsigned rxOffset;
  uint16_t expectedRxLen;

  PT_THREAD(rxEngine());

  virtual
  bool startTx() override;

  virtual
  bool runTx() override;

  virtual
  bool runRx() override;
};

} /* namespace Modbus */
#endif /* MODBUSABSTRACTTCPMASTER_H_ */
