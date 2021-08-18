include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
)

FetchContent_MakeAvailable(googletest)

set(test_files
        test_camera.cpp
        test_frustum.cpp
        test_sparse_set.h test_sparse_set.cpp)

add_executable(framework64_tests ${test_files} ${source_files})
target_link_libraries(framework64_tests PRIVATE framework64 gtest gtest_main)