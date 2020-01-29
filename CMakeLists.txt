# CMakeLists.txt
#
# Created on: 10.02.2016
#     Author: lode

ADD_LIBRARY(ModbusMasterCore STATIC
#    ModbusAscii.cpp
#    ModbusAsciiMaster.cpp
    ModbusRtu.cpp
    ModbusRtuMaster.cpp
    ModbusAbstractTcpMaster.cpp
    ModbusAbstractMaster.cpp
    ModbusSlaveProxy.cpp
    ModbusTxn.cpp
)

ADD_LIBRARY(ModbusMasterRegisterFunctions STATIC
    ModbusWriteSingleRegister.cpp
    ModbusWriteMultipleRegisters.cpp
    ModbusReadHoldingRegisters.cpp
    ModbusReadInputRegisters.cpp
    ModbusReadRegisters.cpp
)

TARGET_LINK_LIBRARIES(ModbusMasterRegisterFunctions
    ModbusMasterCore
)

ADD_LIBRARY(ModbusMasterBitFunctions STATIC
    ModbusReadCoils.cpp
    ModbusReadDiscreteInputs.cpp
    ModbusReadBits.cpp
    ModbusWriteSingleCoil.cpp
    ModbusWriteMultipleCoils.cpp
)

TARGET_LINK_LIBRARIES(ModbusMasterBitFunctions
    ModbusMasterCore
)

