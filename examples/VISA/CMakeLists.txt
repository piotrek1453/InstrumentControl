cmake_minimum_required(VERSION 3.27)

add_executable(InstrumentControlVISA main.cpp)

target_link_libraries(InstrumentControlVISA PRIVATE InstrumentControl::visa)
target_include_directories(InstrumentControlVISA
                           PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../common)
