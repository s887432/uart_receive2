cmake_minimum_required(VERSION 3.5)

# define built platform
if( ${ARCH} STREQUAL "" )
# if didn't use -DARCH=xxx assign building plaform, will use the configure here for built platform

set(PLATFORM x86)
#set(PLATFORM Cortex)
#set(PLATFORM Arm9)
else()
set(PLATFORM ${ARCH})
endif()
message("Preparing makefile for ${PLATFORM}")

#
# get project folder as project name
# the executable file will be named as project name
#
get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId})

string(TOLOWER ${PLATFORM} PLATFORM)

# setup compiler
# for x86 ubuntu
if( $(PLATFORM) STREQUAL "x86")
SET(CMAKE_C_COMPILER	gcc)
SET(CMAKE_CXX_COMPILER	g++)

# setup output executable path
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/x86_bin)
endif()

# for Cortex series
if( ${PLATFORM} STREQUAL "cortex" )
SET(CMAKE_C_COMPILER	/home/user/Workarea/buildroot_2024.04/a5d2_xplained/buildroot-at91/output/host/bin/arm-buildroot-linux-gnueabihf-gcc)
SET(CMAKE_CXX_COMPILER	/home/user/Workarea/buildroot_2024.04/a5d2_xplained/buildroot-at91/output/host/bin/arm-buildroot-linux-gnueabihf-g++)

# setup output executable path
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/cortex_bin)
endif()

# for Arm9 series
if( ${PLATFORM} STREQUAL "arm9" )
SET(CMAKE_C_COMPILER	/home/user/Workarea/buildroot_2024.04/9x60_curiosity/buildroot-at91/output/host/bin/arm-buildroot-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER	/home/user/Workarea/buildroot_2024.04/9x60_curiosity/buildroot-at91/output/host/bin/arm-buildroot-linux-gnueabi-g++)

# setup output executable path
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/arm9_bin)
endif()



# add source code
file(GLOB SRCS "${PROJECT_SOURCE_DIR}/src/*.cpp")

# setup executable file
add_executable(${CMAKE_PROJECT_NAME} ${SRCS})

# end of file
