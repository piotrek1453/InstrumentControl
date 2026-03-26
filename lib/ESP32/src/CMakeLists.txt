cmake_minimum_required(VERSION 3.27)

# ESP32 implementation (MCU; toolchain provided by caller)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_esp32 ${SRC_FILES})
add_library(InstrumentControl::esp32 ALIAS instrumentcontrol_esp32)

target_include_directories(
  instrumentcontrol_esp32 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc")
target_link_libraries(instrumentcontrol_esp32 PUBLIC InstrumentControl::ifc)

target_compile_definitions(instrumentcontrol_esp32 PUBLIC IMPLEMENTATION_ESP32)
