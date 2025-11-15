cmake_minimum_required(VERSION 3.27)

project(InstrumentControlLib LANGUAGES CXX)

# -----------------------------
# Choose instrument control backend from CLI
# -----------------------------
set(IMPLEMENTATION
    "VISA"
    CACHE STRING "Implementation: VISA / OTHER")
set_property(CACHE IMPLEMENTATION PROPERTY STRINGS VISA MOCK OTHER)

message(STATUS "Selected implementation: ${IMPLEMENTATION}")

# -----------------------------
# Choose sources based on backend
# -----------------------------
set(LIB_SOURCES "")

file(GLOB IMPLEMENTATION_SOURCES
     "${CMAKE_CURRENT_SOURCE_DIR}/src/${IMPLEMENTATION}/*.cpp")
list(APPEND LIB_SOURCES ${IMPLEMENTATION_SOURCES})

# -----------------------------
# Add specific lib
# -----------------------------
add_library(InstrumentControlLib ${LIB_SOURCES})
target_include_directories(InstrumentControlLib
                           PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/inc")
target_compile_definitions(InstrumentControlLib
                           PUBLIC IMPLEMENTATION_${IMPLEMENTATION})

# -----------------------------
# Link external libs
# -----------------------------
if(IMPLEMENTATION STREQUAL "VISA")
  if(WIN32)
    find_library(
      VISA_LIB REQUIRED
      NAMES visa64
      HINTS "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Lib_x64/msc")
    target_include_directories(
      InstrumentControlLib
      PUBLIC "C:/Program Files (x86)/IVI Foundation/VISA/WinNT/Include")
    target_link_libraries(InstrumentControlLib PUBLIC "${VISA_LIB}")
  elseif(UNIX)
    find_library(
      VISA_LIB REQUIRED
      NAMES visa
      HINTS "/usr/include")
    target_include_directories(InstrumentControlLib
                               PUBLIC "/usr/include/ni-visa")
    target_link_libraries(InstrumentControlLib PUBLIC visa)
  endif()

  # -----------------------------
  # Link spdlog
  # -----------------------------
  find_package(spdlog CONFIG REQUIRED)
  target_link_libraries(InstrumentControlLib PUBLIC spdlog::spdlog)
endif()
