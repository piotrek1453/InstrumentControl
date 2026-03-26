cmake_minimum_required(VERSION 3.27)

# RP2040 implementation (MCU; toolchain provided by caller)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_rp2040 ${SRC_FILES})
add_library(InstrumentControl::rp2040 ALIAS instrumentcontrol_rp2040)

target_include_directories(
  instrumentcontrol_rp2040 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc")
target_link_libraries(instrumentcontrol_rp2040 PUBLIC InstrumentControl::ifc)

target_compile_definitions(instrumentcontrol_rp2040 PUBLIC IMPLEMENTATION_RP2040)
