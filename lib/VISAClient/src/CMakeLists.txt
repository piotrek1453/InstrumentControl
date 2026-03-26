cmake_minimum_required(VERSION 3.27)

# VISAClient implementation (PC, RPC to pyvisa-py server)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(InstrumentControlLib ${SRC_FILES})

target_include_directories(
  InstrumentControlLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc"
                              "${CMAKE_CURRENT_LIST_DIR}/../..")

target_compile_definitions(InstrumentControlLib
                           PUBLIC IMPLEMENTATION_VISAClient)

# Logging for PC
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(InstrumentControlLib PUBLIC spdlog::spdlog)
