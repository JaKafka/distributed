cmake_minimum_required(VERSION 3.1.0)
project (main C)

# prevent for in-source building
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
if ("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
  message(SEND_ERROR "In-source builds are not allowed.")
endif ()

#add_definitions(-DERROR)

set(CMAKE_BUILD_TYPE Debug) #Release
set(CMAKE_C_STANDARD 11)


### wersja z wszystkim w mainie
set(INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${INCLUDES})

file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.c")

# add_library(...)

add_executable(${PROJECT_NAME} main.c ${SOURCES})
#target_link_libraries(...)

add_custom_target(run ./main)
