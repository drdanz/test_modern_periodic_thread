cmake_minimum_required(VERSION 3.5)
project(test_modern_periodic_thread)

add_definitions(-DUSE_V1_DEFAULT)

add_executable(test_modern_periodic_thread main.cpp
                                           periodic_thread.h)
target_compile_features(test_modern_periodic_thread PRIVATE cxx_std_17)
target_link_libraries(test_modern_periodic_thread pthread)


add_executable(test_flags test_flags.cpp)
target_compile_features(test_flags PRIVATE cxx_std_17)



enable_testing()
add_subdirectory(test)
