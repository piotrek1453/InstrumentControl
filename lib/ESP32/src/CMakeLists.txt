cmake_minimum_required(VERSION 3.27)

# ESP32 implementation (MCU; toolchain provided by caller)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(InstrumentControlLib ${SRC_FILES})

target_include_directories(
  InstrumentControlLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc"
                              "${CMAKE_CURRENT_LIST_DIR}/../..")

target_compile_definitions(InstrumentControlLib PUBLIC IMPLEMENTATION_ESP32)
