cmake_minimum_required(VERSION 3.4)

########################################
# Project setup
########################################
set(PROJECT_NAME StringFragmentator)

########################################
# Set up the compiler flags
########################################
set(CMAKE_CXX_STANDARD 17)

########################################
# Define include directories
########################################
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/include)
include_directories(${COMMON_INCLUDES})

add_subdirectory(thirdparty)
add_subdirectory(src)
add_subdirectory(tests)
