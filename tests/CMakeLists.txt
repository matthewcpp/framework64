
find_package(GTest CONFIG REQUIRED)

set(test_files
        test_box.cpp
        test_bump_allocator.cpp
        test_data_cache.cpp
        test_camera.cpp
        test_frustum.cpp
        test_ray.cpp
        test_sparse_set.h test_sparse_set.cpp
        test_vec3.cpp)

add_executable(framework64_tests ${test_files} ${source_files})
target_link_libraries(framework64_tests PRIVATE framework64 GTest::gtest GTest::gtest_main)