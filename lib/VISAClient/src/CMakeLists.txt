cmake_minimum_required(VERSION 3.27)

# VISAClient implementation (PC, RPC to pyvisa-py server)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_visaclient ${SRC_FILES})
add_library(InstrumentControl::visaclient ALIAS instrumentcontrol_visaclient)

target_include_directories(
  instrumentcontrol_visaclient PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc")
target_link_libraries(instrumentcontrol_visaclient
                      PUBLIC InstrumentControl::ifc)

target_compile_definitions(instrumentcontrol_visaclient
                           PUBLIC IMPLEMENTATION_VISAClient)

# Logging for PC
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(instrumentcontrol_visaclient PUBLIC spdlog::spdlog)
