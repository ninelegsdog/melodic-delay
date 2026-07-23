include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

add_executable(melodic-delay-tests
    test_ring_buffer.cpp
    test_pitch_shifter.cpp
    test_envelope.cpp
    test_melodic_delay.cpp
)
target_link_libraries(melodic-delay-tests PRIVATE melodic-delay-core GTest::gtest_main)
target_include_directories(melodic-delay-tests PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

include(GoogleTest)
gtest_discover_tests(melodic-delay-tests)
