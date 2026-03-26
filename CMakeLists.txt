cmake_minimum_required(VERSION 3.27)

# basic setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(-O2 -Wall -Wextra -Werror -Wuninitialized -fno-exceptions)
# export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(InstrumentControl)

set(EXAMPLE_BACKEND
  "VISA"
  CACHE STRING "Backend used by examples")
set_property(CACHE EXAMPLE_BACKEND PROPERTY STRINGS VISA VISAClient ESP32 RP2040)

add_subdirectory(lib)

# Optionally build examples showing generic business logic usage
option(BUILD_EXAMPLES "Build example application(s)" OFF)
if(BUILD_EXAMPLES)
  if(EXAMPLE_BACKEND STREQUAL "VISA" AND NOT INSTRUMENTCONTROL_BUILD_VISA)
    message(FATAL_ERROR "EXAMPLE_BACKEND=VISA requires INSTRUMENTCONTROL_BUILD_VISA=ON")
  elseif(EXAMPLE_BACKEND STREQUAL "VISAClient" AND NOT INSTRUMENTCONTROL_BUILD_VISACLIENT)
    message(FATAL_ERROR "EXAMPLE_BACKEND=VISAClient requires INSTRUMENTCONTROL_BUILD_VISACLIENT=ON")
  elseif(EXAMPLE_BACKEND STREQUAL "ESP32" AND NOT INSTRUMENTCONTROL_BUILD_ESP32)
    message(FATAL_ERROR "EXAMPLE_BACKEND=ESP32 requires INSTRUMENTCONTROL_BUILD_ESP32=ON")
  elseif(EXAMPLE_BACKEND STREQUAL "RP2040" AND NOT INSTRUMENTCONTROL_BUILD_RP2040)
    message(FATAL_ERROR "EXAMPLE_BACKEND=RP2040 requires INSTRUMENTCONTROL_BUILD_RP2040=ON")
  endif()
  add_subdirectory(examples)
endif()

# Symlink compile_commands.json to project root
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  execute_process(
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()
