/*
 * ModbusAbstractSerialMaster.h
 *
 *  Created on: 23.01.2020
 *      Author: lode
 */

#ifndef MODBUSSERIALMASTER_H_
#define MODBUSSERIALMASTER_H_

#include "ModbusAbstractMaster.h"
#include "ModbusRtu.h"
//#include "ModbusAscii.h"

#include "IoPin.h"
#include "ByteStream.h"
#include "pt/pt.h"
#include "Callback.h"

namespace Modbus
{

/**
 * This class defines a MODBUS master for serial transports and cann be tweaked to MODBUS RRTU or MODBUS ASCII by
 * the apropriate policy.
 */
template<typename EncoderPolicy = Modbus::Rtu>
class SerialMaster: public AbstractMaster
{
public:
  SerialMaster(AbstractTimer& masterTimer, AbstractTimer& txGuardTimer, AbstractTimer& rxSplitTimer);
//  virtual
//  ~SerialMaster();

  /**
   * Connect a serial input channel and an optional control pin for the RS485 driver enable. If the serial port handles
   * RS485 driver enable in hardware, or for point-to-point connections, or other means without explicit TX control,
   * the IoPin can safely left nullptr.
   *
   * The connected port must be non-blocking and shall be configured with even parity and at least on stop bit,
   * preferred baudrate is 19200 Bd. Bytes with frame errors shall not be delivered, if they are delivered,
   * they would lead to checksum errors anyway.
   * @param line
   * @param rs485TxEnable
   * @return
   */
  SerialMaster& setTransportChannel(ByteStream* line, IoPin* rs485TxEnable = nullptr);

  SerialMaster& setTimeoutsForBaudrate(uint32_t baudrate); ///< set timeouts depending on baudrate and specifications, see also @ref setTimeouts_ms()
  SerialMaster& setTimeouts_ms(uint32_t txGuardTime_ms, uint32_t rxSplitLimit_ms); ///< set timeouts explicitly, normally they depend on the baudrate in use, see also @ref setTimeoutsForBaudrate()
  void          getTimeouts_ms(uint32_t &txGuardTime_ms, uint32_t &rxSplitLimit_ms);

  void connectEnableTransmitter(const Callback<void, bool>* signal);
  void connectEnableReceiver(const Callback<void, bool>* signal);

protected:
  ByteStream* serialLine;
  IoPin* rs485TxEnable;
  const Callback<void, bool>* enableTransmitter;
  const Callback<void, bool>* enableReceiver;
  AbstractTimer& txGuardTimer;      // runs in the gap between two transmissions, resp. in front of an upcoming transmission
  AbstractTimer& rxSplitTimer;      // runs during character reception for rxSplitLimit_ms, to check the gap between two characters
  uint32_t txGuardTime_ms;
  uint32_t rxSplitLimit_ms;
  uint8_t rxBuffer[256];            // Buffer fill starts with the function code, as the right address is captured at the beginning of reception and fill ends right before the error check, which is done outside of the abstract class
  uint8_t rxFill;
  uint8_t txLength, txDoneCount;
  struct pt txPt, rxPt;
  EncoderPolicy lineCodec;

  bool doTx:1;
  bool doRx:1;
  bool rxFrameIsBad:1;

  bool rxSplitTimerIsActive:1;
  bool txGuardTimerIsActive:1;
  bool checksumIsGood:1;

  bool startTx() override;  ///< @return true on success, @note moves runningTxn to pendingList, when ready for next transmission or to completedList in case of error
  bool runTx() override;    ///< @return true while busy waiting for TX data space, @note moves runningTxn to pendingList, when ready for next transmission or to completedList in case of error
  bool runRx() override; ///< @return true while busy waiting for more RX data, @note moves runningTxn or pendingList element to completedList on completion

  PT_THREAD(txRunner());
  PT_THREAD(rxRunner());

  bool discardToEndOfFrame(); ///< @return true on end-of-frame, side effect: discard all incoming characters, if there are some, mark rxFrameIsBad = true
};

template<typename EncoderPolicy>
void SerialMaster<EncoderPolicy>::connectEnableTransmitter(const Callback<void, bool>* signal)
{
  enableTransmitter = signal;
}

template<typename EncoderPolicy>
void SerialMaster<EncoderPolicy>::connectEnableReceiver(const Callback<void, bool>* signal)
{
  enableReceiver = signal;
}

} /* namespace Modbus */

#endif /* MODBUSSERIALMASTER_H_ */
