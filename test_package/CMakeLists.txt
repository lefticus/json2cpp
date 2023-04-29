cmake_minimum_required(VERSION 3.15)
project(PackageTest CXX)

find_package(json2cpp CONFIG REQUIRED)

set(COMPILED_JSON "${CMAKE_CURRENT_BINARY_DIR}/test_json")
add_custom_command(
  OUTPUT "${COMPILED_JSON}_impl.hpp" "${COMPILED_JSON}.hpp" "${COMPILED_JSON}.cpp"
  COMMAND json2cpp "test_json" "${CMAKE_SOURCE_DIR}/test.json" "${COMPILED_JSON}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(example src/example.cpp "${COMPILED_JSON}.cpp")
target_link_libraries(example PRIVATE json2cpp::libjson2cpp)
target_include_directories(example PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
