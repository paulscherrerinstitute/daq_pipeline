cmake_minimum_required(VERSION 3.12)
project(daq_pipeline)
set(CMAKE_CXX_STANDARD 14)

set (DAQ_PIPELINE_VERSION "1.0.0")

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory("src")
#add_subdirectory("test")

enable_testing()
