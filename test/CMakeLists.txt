

macro(ADD_FAILING_TEST _name)

  set(${_name}_build_dir "${CMAKE_CURRENT_BINARY_DIR}/${_name}")

  file(WRITE "${${_name}_build_dir}/CMakeLists.txt"
"cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})
project(${_name})

project(${_name}_good)
add_executable(${_name}_good ${_name}_good.cpp)

project(${_name}_bad)
add_executable(${_name}_bad ${_name}_bad.cpp)
")

  file(WRITE "${${_name}_build_dir}/${_name}_good.cpp"
"int main()
{
    return 0;
}
")

  file(WRITE "${${_name}_build_dir}/${_name}_bad.cpp"
"int main()
{
    foo return 0;
}
")

  add_test(NAME ${_name}::setup
          COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test "${${_name}_build_dir}"
                                                          "${${_name}_build_dir}/build"
                                          ${build_generator}
                                          --build-generator "${CMAKE_GENERATOR}"
                                          --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
                                          --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
                                          --build-config $<CONFIGURATION>
                                          --build-makeprogram ${CMAKE_MAKE_PROGRAM}
                                          --build-noclean
                                          --build-target clean
                                          --build-options -Wno-dev
                                                          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                                          -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                                          -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                                                          -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                                          -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
          WORKING_DIRECTORY "${${_name}_build_dir}")
  set_tests_properties(${_name}::setup PROPERTIES FIXTURES_SETUP ${_name}_fixture)

  add_test(NAME ${_name}::good
          COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test "${${_name}_build_dir}"
                                                          "${${_name}_build_dir}/build"
                                          ${build_generator}
                                          --build-generator "${CMAKE_GENERATOR}"
                                          --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
                                          --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
                                          --build-config $<CONFIGURATION>
                                          --build-makeprogram ${CMAKE_MAKE_PROGRAM}
                                          --build-noclean
                                          --build-nocmake
                                          --build-target ${_name}_good
                                          --build-options -Wno-dev
                                                          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                                          -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                                          -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                                                          -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                                          -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
          WORKING_DIRECTORY "${${_name}_build_dir}")
  set_tests_properties(${_name}::good PROPERTIES FIXTURES_REQUIRED ${_name}_fixture)

  add_test(NAME ${_name}::bad
          COMMAND ${CMAKE_CTEST_COMMAND} --build-and-test "${${_name}_build_dir}"
                                                          "${${_name}_build_dir}/build"
                                          ${build_generator}
                                          --build-generator "${CMAKE_GENERATOR}"
                                          --build-generator-platform "${CMAKE_GENERATOR_PLATFORM}"
                                          --build-generator-toolset "${CMAKE_GENERATOR_TOOLSET}"
                                          --build-config $<CONFIGURATION>
                                          --build-makeprogram ${CMAKE_MAKE_PROGRAM}
                                          --build-noclean
                                          --build-nocmake
                                          --build-target ${_name}_bad
                                          --build-options -Wno-dev
                                                          -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                                          -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                                                          -DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
                                                          -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                                                          -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
          WORKING_DIRECTORY "${${_name}_build_dir}")
  set_tests_properties(${_name}::bad PROPERTIES WILL_FAIL 1)
  set_tests_properties(${_name}::bad PROPERTIES FIXTURES_REQUIRED ${_name}_fixture)

  add_test(NAME ${_name}::cleanup
          COMMAND ${CMAKE_COMMAND} -E remove_directory "${${_name}_build_dir}/build"
          WORKING_DIRECTORY "${${_name}_build_dir}")
  set_tests_properties(${_name}::cleanup PROPERTIES FIXTURES_CLEANUP ${_name}_fixture)

  set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${${_name}_build_dir}/build")

endmacro()



add_failing_test(test01)
    
    

