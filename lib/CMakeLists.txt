cmake_minimum_required(VERSION 3.27)

add_library(instrumentcontrol_ifc INTERFACE)
add_library(InstrumentControl::ifc ALIAS instrumentcontrol_ifc)

# Keep include paths stable for #include "ifc/..." and #include "<backend>/inc/..."
target_include_directories(instrumentcontrol_ifc
                                                     INTERFACE "${CMAKE_CURRENT_LIST_DIR}")

option(INSTRUMENTCONTROL_BUILD_VISA "Build VISA backend" ON)
option(INSTRUMENTCONTROL_BUILD_VISACLIENT "Build VISAClient backend" ON)
option(INSTRUMENTCONTROL_BUILD_ESP32 "Build ESP32 backend" ON)
option(INSTRUMENTCONTROL_BUILD_RP2040 "Build RP2040 backend" ON)

if(INSTRUMENTCONTROL_BUILD_VISA)
    add_subdirectory(VISA/src)
endif()

if(INSTRUMENTCONTROL_BUILD_VISACLIENT)
    add_subdirectory(VISAClient/src)
endif()

if(INSTRUMENTCONTROL_BUILD_ESP32)
    add_subdirectory(ESP32/src)
endif()

if(INSTRUMENTCONTROL_BUILD_RP2040)
    add_subdirectory(RP2040/src)
endif()
