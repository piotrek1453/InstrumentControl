cmake_minimum_required(VERSION 3.27)

add_executable(InstrumentControlVISAClient main.cpp)

target_link_libraries(InstrumentControlVISAClient
                      PRIVATE InstrumentControl::visaclient)
