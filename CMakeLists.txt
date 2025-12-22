cmake_minimum_required(VERSION 3.10)
project(HttpEchoServer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройки компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()
include_directories(${CMAKE_SOURCE_DIR}/include)
# Исполняемый файл
add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/http_server.cpp
)

# Для работы с сетью нужны дополнительные библиотеки
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

# Удобные цели
add_custom_target(run
    COMMAND ./${PROJECT_NAME}
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(test-curl
    COMMAND echo "Testing with curl..." &&
            curl -v http://localhost:8080/hello
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)