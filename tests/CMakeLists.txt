include(FetchContent)

# --------------------------------------------------
# GoogleTest dependency
# --------------------------------------------------

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# --------------------------------------------------
# Image tests
# --------------------------------------------------

add_executable(mip_tests
    test_image.cpp
)

target_link_libraries(mip_tests
    PRIVATE
        mip::mip
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(mip_tests)