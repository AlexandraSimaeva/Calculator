cmake_minimum_required(VERSION 3.10)
project(Calculator)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(${PROJECT_NAME} src/main.cpp)
add_custom_target(run 
	COMMAND ./${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)