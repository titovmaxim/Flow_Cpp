cmake_minimum_required(VERSION 3.7)
set(PROJECT_NAME "Unicore.Library")

set(CMAKE_CXX_STANDARD 14)
project(${PROJECT_NAME} LANGUAGES CXX)
file(GLOB_RECURSE SRC CONFIGURE_DEPENDS "*.h" "*.cpp" "*.c")

add_library(${PROJECT_NAME} ${SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)
