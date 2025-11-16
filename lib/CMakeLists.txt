cmake_minimum_required(VERSION 3.27)

project(InstrumentControlLib LANGUAGES CXX)

set(IMPLEMENTATION
    "VISA"
    CACHE STRING "Implementation to build")
set_property(CACHE IMPLEMENTATION PROPERTY STRINGS VISA VISAClient ESP32 RP2040)
message(STATUS "Selected implementation: ${IMPLEMENTATION}")

# Each implementation owns its own CMakeLists; avoid big conditionals here.
add_subdirectory(src/${IMPLEMENTATION})
