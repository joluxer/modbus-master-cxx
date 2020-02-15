#  CMakeLists.txt
#   Created on: 10.02.2020
#       Author: lode

ADD_LIBRARY(LinuxFdSelectRtuDriver STATIC
    FdSelectRtuDriver.cpp
)

TARGET_LINK_LIBRARIES(LinuxFdSelectRtuDriver
    ModbusMasterCore
)
