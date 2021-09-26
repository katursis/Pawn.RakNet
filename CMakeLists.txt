cmake_minimum_required(VERSION 3.10)

project(pawnraknet)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

include(AddSAMPPlugin)

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
  src/internal_packet_channel.h
  src/internal_packet_channel.cc
  src/script.h
  src/script.cc
  src/rakserver.h
  src/rakserver.cc
  src/hooks.h
  src/hooks.cc

  lib/RakNet/BitStream.h
  lib/RakNet/BitStream.cpp
  lib/RakNet/StringCompressor.h
  lib/RakNet/StringCompressor.cpp
  lib/RakNet/PluginInterface.h
  lib/RakNet/PluginInterface.cpp
  lib/RakNet/DS_HuffmanEncodingTreeNode.h
  lib/RakNet/DS_HuffmanEncodingTree.h
  lib/RakNet/DS_HuffmanEncodingTree.cpp

  lib/samp-ptl/ptl.h
)

target_include_directories(${PROJECT_NAME} PRIVATE lib)
