cmake_minimum_required(VERSION 3.27)

project(InstrumentControlLib)

# add sources and headers
add_library(InstrumentControlLib src/InstrumentControl.cpp
                              inc/InstrumentControl.hpp)
target_include_directories(InstrumentControlLib
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")

# find and include VISA lib based on OS
if(WIN32)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa64
    HINTS "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Lib_x64/msc")
  target_include_directories(
    InstrumentControlLib
    PUBLIC "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include")
  target_link_libraries(InstrumentControlLib PUBLIC "${VISA_LIB}")
endif()

if(UNIX)
  find_library(
    VISA_LIB REQUIRED
    NAMES visa
    HINTS "/usr/include")
  target_include_directories(InstrumentControlLib PUBLIC "/usr/include/ni-visa")
  target_include_directories(InstrumentControlLib PUBLIC "/usr/include")
  target_link_libraries(InstrumentControlLib PUBLIC visa)
endif()

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(InstrumentControlLib PUBLIC spdlog::spdlog)

