cmake_minimum_required(VERSION 3.27)

# basic setup
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# export compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(InstrumentControl)

add_subdirectory(lib)

# Symlink compile_commands.json to project root
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  execute_process(
    COMMAND
      ${CMAKE_COMMAND} -E create_symlink
      ${CMAKE_BINARY_DIR}/compile_commands.json
      ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif()
