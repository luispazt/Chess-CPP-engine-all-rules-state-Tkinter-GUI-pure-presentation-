add_executable(chess_tests
   test_main.cpp
)

target_link_libraries(chess_tests PRIVATE chess_core_static)
target_include_directories(chess_tests PRIVATE ${CMAKE_SOURCE_DIR}/cpp/include)
target_include_directories(chess_tests PRIVATE ${CMAKE_SOURCE_DIR}/cpp/tests)

include(CTest)
add_test(NAME chess_tests COMMAND chess_tests)

