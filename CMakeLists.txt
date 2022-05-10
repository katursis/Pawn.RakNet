cmake_minimum_required(VERSION 3.10)

project(pawnraknet)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include(AddSAMPPlugin)

add_subdirectory(lib/omp-sdk)

add_samp_plugin(${PROJECT_NAME}
  plugin.def

  src/Pawn.RakNet.inc
  src/main.h
  src/main.cc
  src/plugin.h
  src/plugin.cc
  src/native_param.h
  src/config.h
  src/config.cc
  src/bitstream_pool.h
  src/bitstream_pool.cc
  src/script.h
  src/script.cc
  src/hooks.h
  src/hooks.cc

  lib/RakNet/bitstream.hpp
  lib/RakNet/bitstream.cpp
  lib/RakNet/packet.hpp
  lib/RakNet/Encoding/huffman_tree_node.hpp
  lib/RakNet/Encoding/huffman_tree.hpp
  lib/RakNet/Encoding/huffman_tree.cpp
  lib/RakNet/Encoding/str_compress.hpp
  lib/RakNet/Encoding/str_compress.cpp

  lib/samp-ptl/ptl.h
)

target_include_directories(${PROJECT_NAME} PRIVATE lib)

target_link_libraries(${PROJECT_NAME} PRIVATE OMP-SDK)
