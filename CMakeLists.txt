cmake_minimum_required(VERSION 3.9)
project(SimplexNoise)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

# Library sources (without the example)
set(SIMPLEXNOISE_SOURCES
    SimplexNoise.cpp
    SimplexNoise.hpp)

# Set path to find SFML 3 installed via Homebrew
list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")

# Try to find SFML
find_package(SFML 3 COMPONENTS Graphics Audio QUIET)

if(SFML_FOUND)
    message(STATUS "SFML found - building with example")
    add_executable(SimplexNoise
        ${SIMPLEXNOISE_SOURCES}
        examples/RandomWorld.cpp)
    target_link_libraries(SimplexNoise SFML::Graphics SFML::Audio)
else()
    message(STATUS "SFML not found - building library only (no example)")
    add_library(SimplexNoise ${SIMPLEXNOISE_SOURCES})
endif()
