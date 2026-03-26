cmake_minimum_required(VERSION 3.27)

add_executable(InstrumentControlExample main.cpp)

set(_backend_target "")
if(EXAMPLE_BACKEND STREQUAL "VISA")
	set(_backend_target InstrumentControl::visa)
elseif(EXAMPLE_BACKEND STREQUAL "VISAClient")
	set(_backend_target InstrumentControl::visaclient)
elseif(EXAMPLE_BACKEND STREQUAL "ESP32")
	set(_backend_target InstrumentControl::esp32)
elseif(EXAMPLE_BACKEND STREQUAL "RP2040")
	set(_backend_target InstrumentControl::rp2040)
else()
	message(FATAL_ERROR "Unsupported EXAMPLE_BACKEND='${EXAMPLE_BACKEND}'")
endif()

target_link_libraries(InstrumentControlExample PRIVATE ${_backend_target})
