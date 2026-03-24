cmake_minimum_required(VERSION 3.15 FATAL_ERROR)
project(ZeekPlugin_Fluvio CXX C)
include(ZeekPlugin)

include(FetchContent)
FetchContent_Declare(
  fluvio_client_cpp
  URL https://github.com/stefanDeveloper/fluvio-client-cpp/releases/download/v0.0.2/fluvio-client-cpp-linux-x64.tar.gz
)
FetchContent_MakeAvailable(fluvio_client_cpp)
find_package(fluvio_client_cpp CONFIG REQUIRED PATHS ${fluvio_client_cpp_SOURCE_DIR})

get_target_property(FLUVIO_INC fluvio_client_cpp::fluvio_client_cpp INTERFACE_INCLUDE_DIRECTORIES)
include_directories(BEFORE ${FLUVIO_INC})

zeek_plugin_begin(ZEEK FLUVIO)
zeek_plugin_cc(src/Plugin.cc)
zeek_plugin_cc(src/FluvioWriter.cc)
zeek_plugin_bif(src/fluvio.bif)
zeek_plugin_link_library(fluvio_client_cpp::fluvio_client_cpp)
zeek_plugin_end()
