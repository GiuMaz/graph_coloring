cmake_minimum_required (VERSION 3.1)

project (graph_coloring VERSION 1.0 LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

add_library(graph STATIC src/graph.cpp)
target_include_directories(graph PUBLIC include)

add_executable(shrink_test src/shrinking_set_test.cpp)
target_link_libraries(shrink_test PRIVATE graph)

add_executable(george_appel src/george_appel.cpp)
target_link_libraries(george_appel PRIVATE graph)

add_executable(dimacs src/dimacs.cpp)
target_link_libraries(dimacs PRIVATE graph)

