cmake_minimum_required(VERSION 3.9)

set(PROJECT_NAME VLN)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SOURCES
        include/VLN.h src/VLN.cpp)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

add_executable(${PROJECT_NAME}_runtests src/tests/tests.cpp)
target_link_libraries(${PROJECT_NAME}_runtests ${PROJECT_NAME})