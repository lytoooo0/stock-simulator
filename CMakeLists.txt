cmake_minimum_required(VERSION 4.1)
project(StockSystemSimulator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_DEBUGGING "print debug info" ON)

if(ENABLE_DEBUGGING)
    message(STATUS "Debugging enabled")
    add_compile_definitions(ENABLE_DEBUGGING)
else()
    message(STATUS "Debugging disabled")
endif()

include_directories(include)
add_executable(stock_generator generator.cpp utils.cpp)
add_executable(stock_handler handler.cpp utils.cpp)
