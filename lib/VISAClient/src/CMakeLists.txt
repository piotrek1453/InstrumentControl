cmake_minimum_required(VERSION 3.27)

# VISAClient implementation (PC, RPC to pyvisa-py server)
file(GLOB SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/*.cpp")

add_library(instrumentcontrol_visaclient ${SRC_FILES})
add_library(InstrumentControl::visaclient ALIAS instrumentcontrol_visaclient)

target_include_directories(instrumentcontrol_visaclient
                           PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../inc")
target_link_libraries(instrumentcontrol_visaclient
                      PUBLIC InstrumentControl::ifc)

target_compile_definitions(instrumentcontrol_visaclient
                           PUBLIC IMPLEMENTATION_VISAClient)

# Logging for PC
find_package(spdlog CONFIG REQUIRED)
target_link_libraries(instrumentcontrol_visaclient PUBLIC spdlog::spdlog)

# gRPC/protobuf integration: generate C++ sources from proto files in the
# pyvisa-grpc submodule and link into the visaclient library.
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(PYVISA_PROTO_DIR ${CMAKE_SOURCE_DIR}/lib/VISAClient/pyvisa-grpc/protos)
set(PYVISA_PROTO ${PYVISA_PROTO_DIR}/pyvisa_grpc.proto)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
file(MAKE_DIRECTORY ${GEN_DIR})

set(PB_HDR ${GEN_DIR}/pyvisa_grpc.pb.h)
set(PB_SRC ${GEN_DIR}/pyvisa_grpc.pb.cc)
set(GRPC_HDR ${GEN_DIR}/pyvisa_grpc.grpc.pb.h)
set(GRPC_SRC ${GEN_DIR}/pyvisa_grpc.grpc.pb.cc)

if(TARGET Protobuf::protoc)
  set(_PROTOC_CMD "$<TARGET_FILE:Protobuf::protoc>")
elseif(DEFINED Protobuf_PROTOC_EXECUTABLE AND Protobuf_PROTOC_EXECUTABLE)
  set(_PROTOC_CMD "${Protobuf_PROTOC_EXECUTABLE}")
else()
  find_program(_PROTOC_CMD protoc)
endif()

if(TARGET gRPC::grpc_cpp_plugin)
  set(_GRPC_PLUGIN_CMD "$<TARGET_FILE:gRPC::grpc_cpp_plugin>")
else()
  find_program(_GRPC_PLUGIN_CMD grpc_cpp_plugin)
endif()

if(NOT _PROTOC_CMD)
  message(
    FATAL_ERROR
      "protoc not found. Install protoc or ensure Conan provides Protobuf::protoc or Protobuf_PROTOC_EXECUTABLE."
  )
endif()

if(NOT _GRPC_PLUGIN_CMD)
  message(
    FATAL_ERROR
      "grpc_cpp_plugin not found. Install gRPC tools or ensure Conan provides gRPC::grpc_cpp_plugin."
  )
endif()

add_custom_command(
  OUTPUT ${PB_HDR} ${PB_SRC} ${GRPC_HDR} ${GRPC_SRC}
  COMMAND
    ${_PROTOC_CMD} --proto_path=${PYVISA_PROTO_DIR} --cpp_out=${GEN_DIR}
    --grpc_out=${GEN_DIR} --plugin=protoc-gen-grpc=${_GRPC_PLUGIN_CMD}
    ${PYVISA_PROTO}
  DEPENDS ${PYVISA_PROTO}
  COMMENT "Generating C++ proto sources for pyvisa_grpc.proto")

add_library(visaclient_proto ${PB_SRC} ${GRPC_SRC})
target_include_directories(visaclient_proto PUBLIC ${GEN_DIR})
target_link_libraries(visaclient_proto PUBLIC gRPC::grpc++
                                              protobuf::libprotobuf)

# Link generated proto library into visaclient implementation
target_link_libraries(instrumentcontrol_visaclient PUBLIC visaclient_proto)
