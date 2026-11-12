cmake_minimum_required(VERSION 3.20)
project(chess LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(pybind11 CONFIG REQUIRED)

add_library(chess_core_static STATIC
   cpp/src/Board.cpp
   cpp/src/Game.cpp
   cpp/src/Move.cpp
   cpp/src/Square.cpp
   cpp/src/Serialization.cpp
   cpp/src/Player.cpp
   cpp/src/pieces/Pawn.cpp
   cpp/src/pieces/Knight.cpp
   cpp/src/pieces/Bishop.cpp
   cpp/src/pieces/Rook.cpp
   cpp/src/pieces/Queen.cpp
   cpp/src/pieces/King.cpp
   cpp/src/pieces/SlidingPiece.cpp
)

set_target_properties(chess_core_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(chess_core_static PUBLIC cpp/include)

pybind11_add_module(chess_core_py cpp/src/binding.cpp)
target_link_libraries(chess_core_py PRIVATE chess_core_static)
set_target_properties(chess_core_py PROPERTIES
   OUTPUT_NAME chess_core
   LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python
)

enable_testing()
add_subdirectory(cpp/tests)