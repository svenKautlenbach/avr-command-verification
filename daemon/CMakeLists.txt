cmake_minimum_required (VERSION 3.2.2)
project (avr-command-daemon)

set (CMAKE_CXX_FLAGS "-Wall -Werror -std=gnu++11")

include_directories (
	"${PROJECT_SOURCE_DIR}"
	)

file(GLOB SOURCES "*.cpp")

add_executable(avr-command-daemon ${SOURCES})
