cmake_minimum_required(VERSION 3.16)
project(melodic-delay VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Core library
add_library(melodic-delay-core STATIC
    src/core/RingBuffer.cpp
    src/core/PitchShifter.cpp
    src/core/Envelope.cpp
    src/core/MelodicDelay.cpp
)
target_include_directories(melodic-delay-core PUBLIC src/core third_party)

# CLI executable
add_executable(melodic-delay src/cli/main.cpp)
target_link_libraries(melodic-delay PRIVATE melodic-delay-core)

# Tests
enable_testing()
add_subdirectory(tests)
