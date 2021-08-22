cmake_minimum_required(VERSION 3.10)

project(raknet)

add_library(${PROJECT_NAME} STATIC
  BitStream.h
  BitStream.cpp
  StringCompressor.h
  StringCompressor.cpp
  PluginInterface.h
  PluginInterface.cpp
  DS_HuffmanEncodingTreeNode.h
  DS_HuffmanEncodingTree.h
  DS_HuffmanEncodingTree.cpp
)

if(CMAKE_COMPILER_IS_GNUCC)
  set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY COMPILE_FLAGS "-m32")
  set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS "-m32")
endif()
