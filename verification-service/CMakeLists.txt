cmake_minimum_required (VERSION 3.2.2)
project (avr-whitelist-verification)

set (CMAKE_CXX_FLAGS "-Wall -Werror -std=gnu++11")

include_directories (
	"${PROJECT_SOURCE_DIR}"
	)

file(GLOB SOURCES "main.cpp")

add_executable(avr-whitelist-verify ${SOURCES})
