cmake_minimum_required(VERSION 3.12)
project(daq_pipeline)
set(CMAKE_CXX_STANDARD 14)

set (DAQ_PIPELINE_VERSION "1.0.0")

#configure_file (
#        "${PROJECT_SOURCE_DIR}/version.h.in"
#        "${PROJECT_BINARY_DIR}/version.h"
#)

include_directories("external")
include_directories("${PROJECT_BINARY_DIR}")


add_executable(store_pipeline src/main.cpp src/ZmqReceiver.cpp src/ZmqReceiver.hpp src/CassandraWriter.cpp
        src/CassandraWriter.h src/config.h src/version.h.in src/IDataReceiver.h src/ChannelValue.cpp src/ChannelValue.h src/IDataStore.h src/IStatsProvider.h)