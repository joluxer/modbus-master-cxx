/*
 * PrintModbusMasterErrorCode.cpp
 *
 *  Created on: 31.01.2020
 *      Author: lode
 */

#include "ModbusMasterExceptionCodeToString.h"
#include <cstdio>

const char* modbusMasterExceptionCodeToString(::Modbus::ResultCode rc)
{
  const char* rcText = "unknown code";

  switch (rc)
  {
  case ::Modbus::ResultCode::NoError:
    rcText = "no error";
    break;
  case ::Modbus::ResultCode::IllegalFunction:
    rcText = "illegal function code";
    break;
  case ::Modbus::ResultCode::IllegalDataAddress:
    rcText = "illegal data address";
    break;
  case ::Modbus::ResultCode::IllegalDataValue:
    rcText = "illegal data value";
    break;
  case ::Modbus::ResultCode::ServerDeviceFailure:
    rcText = "server device failure";
    break;
  case ::Modbus::ResultCode::Acknowledge:
    rcText = "ACK";
    break;
  case ::Modbus::ResultCode::ServerDeviceBusy:
    rcText = "server device busy";
    break;
  case ::Modbus::ResultCode::MemoryParityError:
    rcText = "memory parity error";
    break;
  case ::Modbus::ResultCode::GatewayPathUnavailable:
    rcText = "gateway path unavailable";
    break;
  case ::Modbus::ResultCode::GatewayTargetDeviceFailedToRespond:
    rcText = "gateway target device failed to respond";
    break;
  case ::Modbus::ResultCode::ProxyHasNoConnection:
    rcText = "slave proxy has no master connection";
    break;
  case ::Modbus::ResultCode::UnknownCommunicationError:
    rcText = "unknown communication error";
    break;
  case ::Modbus::ResultCode::MasterNotReady:
    rcText = "master not ready";
    break;
  case ::Modbus::ResultCode::CommunicationTimeout:
    rcText = "communication timeout";
    break;
  case ::Modbus::ResultCode::AnswerDoesNotMatchRequest:
    rcText = "answer does not match request";
    break;
  case ::Modbus::ResultCode::NoReceiveBuffer:
    rcText = "no receive buffer available";
    break;
  case ::Modbus::ResultCode::SlaveDisconnectInTransfer:
    rcText = "slaved disconnected in transfer";
    break;
  case ::Modbus::ResultCode::NoTxBufferSpace:
    rcText = "no tx buffer space";
    break;
  case ::Modbus::ResultCode::TooManyPendingRequests:
    rcText = "too many pending requests";
    break;
  case ::Modbus::ResultCode::TcpDestinationNotReachable:
    rcText = "tcp destination not reachable";
    break;
  case ::Modbus::ResultCode::ReceivedChecksumDoesNotMatch:
    rcText = "received checksum does not match";
    break;
  case ::Modbus::ResultCode::ReceivedSlaveIdDoesNotMatch:
    rcText = "received slave id does not match";
    break;
  default:
    break;
  }

  return rcText;
}


