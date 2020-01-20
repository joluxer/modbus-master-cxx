/*
 * ModbusRtuMaster.h
 *
 *  Created on: 15.02.2016
 *      Author: lode
 */

#ifndef MODBUSRTUMASTER_H_
#define MODBUSRTUMASTER_H_

#include "ModbusAbstractMaster.h"

#include "ByteStream.h"
#include "IoPin.h"

namespace Modbus
{

class RtuMaster: public Modbus::AbstractMaster
{
public:
  RtuMaster(AbstractTimer& masterTimer, AbstractTimer& txGuardTimer, MillisecondCounter& rxGapCounter);
//  virtual ~RtuMaster();

  RtuMaster& sendOn(ByteStream* line, IoPin* rs485TxEnable = nullptr);
  RtuMaster& setRxSplitLimit_ms(uint32_t splitTime_ms);

protected:
  AbstractTimer& txGuardTimer;
  MillisecondCounter& rxGapCounter;
  ByteStream* serialLine;
  IoPin* rs485TxEnable;
  uint16_t pduCrc;
  uint8_t txDoneCount;
  uint32_t rxSplitTime_ms;
  uint8_t rxBuffer[253];
  uint8_t rxFill;

  void resetCrc();
  void updateCrc(uint8_t* data, unsigned len);

  virtual
  bool startTx() override;
  virtual
  bool runTx() override;
  virtual
  bool runRx() override;
};

} /* namespace Modbus */
#endif /* MODBUSRTUMASTER_H_ */
