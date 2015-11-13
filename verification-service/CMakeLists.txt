cmake_minimum_required (VERSION 3.2.2)
project (whitelist-verification)

set (CMAKE_CXX_FLAGS "-Wall -Werror -std=c++11")

include_directories (
	"${PROJECT_SOURCE_DIR}"
	)

file(GLOB SOURCES "main.cpp")

add_executable(whitelist-verify ${SOURCES})
target_link_libraries(whitelist-verify ${PROJECT_SOURCE_DIR}/libjson11.a)
