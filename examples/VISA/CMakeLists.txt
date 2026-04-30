cmake_minimum_required(VERSION 3.27)

add_executable(InstrumentControlVISA main.cpp)

target_link_libraries(InstrumentControlVISA PRIVATE InstrumentControl::visa)
