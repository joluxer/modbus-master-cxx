/*
 * ModbusTypes.h
 *
 *  Created on: 05.02.2016
 *      Author: lode
 */

#ifndef MODBUSTYPES_H_
#define MODBUSTYPES_H_

#include <cstdint>

namespace Modbus
{

typedef enum
{
  NoError = 0,
  // from Modbus specs
  IllegalFunction,
  IllegalDataAddress,
  IllegalDataValue,
  ServerDeviceFailure,
  Acknowledge,
  ServerDeviceBusy,
  MemoryParityError = 0x08,
  GatewayPathUnavailable = 0x0A,
  GatewayTargetDeviceFailedToRespond,

  // additional for the framework
  ProxyHasNoConnection = 0x100,
  UnknownCommunicationError,
  MasterNotReady,
  CommunicationTimeout,
  AnswerDoesNotMatchRequest,
  NoReceiveBuffer,
  SlaveDisconnectInTransfer,
  NoTxBufferSpace,
  TooManyPendingRequests,
  TcpDestinationNotReachable,
  ReceivedSlaveIdDoesNotMatch,

} ResultCode;

typedef struct
{
  uint8_t* data;
  uint8_t length;
} PduDataBuffer;

typedef struct
{
  const uint8_t* data;
  uint8_t length;
} PduConstDataBuffer;

constexpr
unsigned numOfBits2ArrayLength(uint16_t numOfBits)
{
  return (numOfBits / (sizeof(unsigned) * 8)) + (numOfBits % (sizeof(unsigned) * 8) ? 1 : 0);
}

} // namespace Modbus


#endif /* MODBUSTYPES_H_ */
