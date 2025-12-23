cmake_minimum_required(VERSION 3.10)
project(Calculator)

# Используем C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Укажем явно компилятор g++-12
set(CMAKE_CXX_COMPILER g++-12)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(Calculator
    src/main.cpp
    src/http_server.cpp
    src/Calc.cpp
)

target_link_libraries(Calculator pthread)