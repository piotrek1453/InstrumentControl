# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension
# to work ==
if(WIN32)
  set(USERHOME $ENV{USERPROFILE})
else()
  set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
  include(${picoVscode})
endif()
# ====================================================================================
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension
# to work ==
if(WIN32)
  set(USERHOME $ENV{USERPROFILE})
else()
  set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
  include(${picoVscode})
endif()
# ====================================================================================
# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location (note this can come from
# environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension
# to work ==
if(WIN32)
  set(USERHOME $ENV{USERPROFILE})
else()
  set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if(EXISTS ${picoVscode})
  include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD
    pico
    CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(RP2040 C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

set(INSTRUMENTCONTROL_BUILD_VISA
    OFF
    CACHE BOOL "" FORCE)
set(INSTRUMENTCONTROL_BUILD_VISACLIENT
    OFF
    CACHE BOOL "" FORCE)
set(INSTRUMENTCONTROL_BUILD_ESP32
    OFF
    CACHE BOOL "" FORCE)
set(INSTRUMENTCONTROL_BUILD_RP2040
    ON
    CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../lib
                 ${CMAKE_BINARY_DIR}/instrumentcontrol)

# Add executable. Default name is the project name, version 0.1

add_executable(RP2040 main/src/main.cpp)

pico_set_program_name(RP2040 "RP2040")
pico_set_program_version(RP2040 "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(RP2040 1)
pico_enable_stdio_usb(RP2040 0)

# Add the standard library to the build
target_link_libraries(RP2040 pico_stdlib InstrumentControl::rp2040)

# Add the standard include files to the build
target_include_directories(
  RP2040
  PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/../../lib
          ${CMAKE_CURRENT_LIST_DIR}/main/inc
          ${CMAKE_CURRENT_LIST_DIR}/../common)

pico_add_extra_outputs(RP2040)
