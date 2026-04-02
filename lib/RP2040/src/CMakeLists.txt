cmake_minimum_required(VERSION 3.27)

option(INSTRUMENTCONTROL_RP2040_ENABLE_W5500
       "Enable W5500 transport dependencies for RP2040 backend" ON)

set(INSTRUMENTCONTROL_RP2040_WIZCHIP
    "W5500"
    CACHE STRING "WIZnet chip used by RP2040 backend")
set_property(CACHE INSTRUMENTCONTROL_RP2040_WIZCHIP PROPERTY STRINGS W5500)

set(INSTRUMENTCONTROL_RP2040_DEVICE_BOARD_NAME
    "W5500_EVB_PICO"
    CACHE STRING "Board profile used by WIZnet RP2040 port")

set(INSTRUMENTCONTROL_RP2040_THIRD_PARTY_DIR
    "${CMAKE_CURRENT_LIST_DIR}/../third_party")

set(_wiznet_iolib_candidates
    "${INSTRUMENTCONTROL_RP2040_THIRD_PARTY_DIR}/ioLibrary_Driver"
    "${INSTRUMENTCONTROL_RP2040_THIRD_PARTY_DIR}/ioLibrary_Driver"
    "${INSTRUMENTCONTROL_RP2040_THIRD_PARTY_DIR}/WIZnet-PICO-C/libraries/ioLibrary_Driver"
)

unset(INSTRUMENTCONTROL_RP2040_WIZNET_DIR)
foreach(_candidate IN LISTS _wiznet_iolib_candidates)
  if(EXISTS "${_candidate}/Ethernet/wizchip_conf.c")
    set(INSTRUMENTCONTROL_RP2040_WIZNET_DIR "${_candidate}")
    break()
  endif()
endforeach()

set(_wiznet_port_candidates
    "${INSTRUMENTCONTROL_RP2040_THIRD_PARTY_DIR}/WIZnet-PICO-C/port")

unset(INSTRUMENTCONTROL_RP2040_PORT_DIR)
foreach(_candidate IN LISTS _wiznet_port_candidates)
  if(EXISTS "${_candidate}/ioLibrary_Driver/src/wizchip_spi.c")
    set(INSTRUMENTCONTROL_RP2040_PORT_DIR "${_candidate}")
    break()
  endif()
endforeach()

if(INSTRUMENTCONTROL_RP2040_ENABLE_W5500)
  if(NOT INSTRUMENTCONTROL_RP2040_WIZNET_DIR)
    message(
      FATAL_ERROR
        "RP2040: missing ioLibrary_Driver submodule. Run: git submodule update --init --recursive"
    )
  endif()

  if(NOT INSTRUMENTCONTROL_RP2040_PORT_DIR)
    message(
      FATAL_ERROR
        "RP2040: missing WIZnet-PICO-C port sources. Run: git submodule update --init --recursive"
    )
  endif()

  add_library(
    instrumentcontrol_rp2040_iolibrary_driver STATIC
    "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet/wizchip_conf.c"
    "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet/socket.c"
    "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet/W5500/w5500.c")

  target_include_directories(
    instrumentcontrol_rp2040_iolibrary_driver
    PUBLIC "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet"
           "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet/W5500")

  target_compile_definitions(
    instrumentcontrol_rp2040_iolibrary_driver
    PUBLIC _WIZCHIP_=${INSTRUMENTCONTROL_RP2040_WIZCHIP})

  add_library(
    instrumentcontrol_rp2040_iolibrary_port STATIC
    "${INSTRUMENTCONTROL_RP2040_PORT_DIR}/ioLibrary_Driver/src/wizchip_spi.c"
    "${INSTRUMENTCONTROL_RP2040_PORT_DIR}/ioLibrary_Driver/src/wizchip_gpio_irq.c"
  )

  target_include_directories(
    instrumentcontrol_rp2040_iolibrary_port
    PUBLIC "${INSTRUMENTCONTROL_RP2040_PORT_DIR}/ioLibrary_Driver/inc"
           "${INSTRUMENTCONTROL_RP2040_PORT_DIR}"
           "${INSTRUMENTCONTROL_RP2040_WIZNET_DIR}/Ethernet")

  target_compile_definitions(
    instrumentcontrol_rp2040_iolibrary_port
    PUBLIC _WIZCHIP_=${INSTRUMENTCONTROL_RP2040_WIZCHIP}
           DEVICE_BOARD_NAME=${INSTRUMENTCONTROL_RP2040_DEVICE_BOARD_NAME})

  add_library(instrumentcontrol_rp2040_iolibrary_timer STATIC
              "${INSTRUMENTCONTROL_RP2040_PORT_DIR}/timer/timer.c")

  target_include_directories(
    instrumentcontrol_rp2040_iolibrary_timer
    PUBLIC "${INSTRUMENTCONTROL_RP2040_PORT_DIR}/timer")

  if(TARGET pico_stdlib)
    target_link_libraries(
      instrumentcontrol_rp2040_iolibrary_port
      PUBLIC pico_stdlib hardware_spi hardware_dma
             instrumentcontrol_rp2040_iolibrary_driver)
    target_link_libraries(instrumentcontrol_rp2040_iolibrary_timer
                          PUBLIC pico_stdlib)
  else()
    message(
      STATUS
        "RP2040: pico targets not found; link them in the final firmware target"
    )
    target_link_libraries(instrumentcontrol_rp2040_iolibrary_port
                          PUBLIC instrumentcontrol_rp2040_iolibrary_driver)
  endif()
endif()

# RP2040 implementation (MCU; toolchain provided by caller)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_rp2040 ${SRC_FILES})
add_library(InstrumentControl::rp2040 ALIAS instrumentcontrol_rp2040)

target_include_directories(
  instrumentcontrol_rp2040 PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc"
                                  "${CMAKE_CURRENT_LIST_DIR}/..")

target_link_libraries(instrumentcontrol_rp2040 PUBLIC InstrumentControl::ifc)

target_compile_definitions(
  instrumentcontrol_rp2040
  PUBLIC IMPLEMENTATION_RP2040 _WIZCHIP_=${INSTRUMENTCONTROL_RP2040_WIZCHIP}
         DEVICE_BOARD_NAME=${INSTRUMENTCONTROL_RP2040_DEVICE_BOARD_NAME})

if(INSTRUMENTCONTROL_RP2040_ENABLE_W5500)
  target_link_libraries(
    instrumentcontrol_rp2040
    PUBLIC instrumentcontrol_rp2040_iolibrary_driver
           instrumentcontrol_rp2040_iolibrary_port
           instrumentcontrol_rp2040_iolibrary_timer)
endif()
