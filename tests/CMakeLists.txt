include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
)

FetchContent_MakeAvailable(googletest)

set(test_files
        test_sparse_set.h test_sparse_set.cpp)

set(source_files
        ../include/framework64/sparse_set.h ../src/n64/sparse_set.c)

add_executable(framework64_tests ${test_files} ${source_files})
target_link_libraries(framework64_tests PRIVATE gtest gtest_main)
target_include_directories(framework64_tests PRIVATE ../include)