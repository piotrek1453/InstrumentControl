cmake_minimum_required(VERSION 3.27)

add_executable(InstrumentControlExample main.cpp)

target_link_libraries(InstrumentControlExample PRIVATE InstrumentControlLib)
