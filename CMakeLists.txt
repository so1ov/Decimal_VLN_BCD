cmake_minimum_required(VERSION 3.9)

set(PROJECT_NAME Decimal_VLN_BCD)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 17)

set(${PROJECT_NAME}_SOURCES
        include/Decimal.h src/Decimal.cpp
        include/DecimalStatus.h
        include/DecimalIntegerDivisionResult.h)

add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC include)

# testing
add_subdirectory(submodule/googletest)

add_executable(${PROJECT_NAME}_runtests src/tests/tests.cpp)
target_link_libraries(${PROJECT_NAME}_runtests ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME}_runtests gtest gmock)

