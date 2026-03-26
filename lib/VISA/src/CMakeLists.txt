cmake_minimum_required(VERSION 3.27)

# VISA implementation (PC, native VISA C API)
file(GLOB VISA_SOURCES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_visa ${VISA_SOURCES})
add_library(InstrumentControl::visa ALIAS instrumentcontrol_visa)

target_include_directories(
  instrumentcontrol_visa PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc")
target_link_libraries(instrumentcontrol_visa PUBLIC InstrumentControl::ifc)

# Export macro for clients (examples)
target_compile_definitions(instrumentcontrol_visa PUBLIC IMPLEMENTATION_VISA)

# VISA native C library
if(WIN32)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa64
    HINTS "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Lib_x64/msc")
  target_include_directories(
    instrumentcontrol_visa
    PUBLIC "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include")
  target_link_libraries(instrumentcontrol_visa PUBLIC "${VISA_LIB}")
elseif(UNIX)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa
    HINTS "/usr/include")
  target_include_directories(instrumentcontrol_visa PUBLIC "/usr/include/ni-visa")
  target_link_libraries(instrumentcontrol_visa PUBLIC visa)
endif()

# Logging for PC
find_package(spdlog CONFIG REQUIRED)
# fmt comes via spdlog dependency typically
target_link_libraries(instrumentcontrol_visa PUBLIC spdlog::spdlog)
