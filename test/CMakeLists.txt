cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(myproject CONFIG REQUIRED) # for intro, json2cpp_options, ...

  if(NOT TARGET myjson2cpp_options)
    message(FATAL_ERROR "Requiered config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)


set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/test_json")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
  COMMAND json2cpp "test_json" "${CMAKE_SOURCE_DIR}/examples/test.json" "${BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(tests tests.cpp "${BASE_NAME}.cpp")
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(tests PRIVATE json2cpp_warnings json2cpp_options Catch2::Catch2WithMain)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

set(SCHEMA_BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/allof_integers_and_numbers.schema")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${SCHEMA_BASE_NAME}_impl.hpp" "${SCHEMA_BASE_NAME}.hpp" "${SCHEMA_BASE_NAME}.cpp"
  COMMAND json2cpp "allof_integers_and_numbers_schema"
          "${CMAKE_SOURCE_DIR}/examples/allof_integers_and_numbers.schema.json" "${SCHEMA_BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(INT_BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/array_integers_10_20_30_40")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${INT_BASE_NAME}_impl.hpp" "${INT_BASE_NAME}.hpp" "${INT_BASE_NAME}.cpp"
  COMMAND json2cpp "array_integers_10_20_30_40" "${CMAKE_SOURCE_DIR}/examples/array_integers_10_20_30_40.json"
          "${INT_BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

set(DOUBLE_BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/array_doubles_10_20_30_40")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${DOUBLE_BASE_NAME}_impl.hpp" "${DOUBLE_BASE_NAME}.hpp" "${DOUBLE_BASE_NAME}.cpp"
  COMMAND json2cpp "array_doubles_10_20_30_40" "${CMAKE_SOURCE_DIR}/examples/array_doubles_10_20_30_40.json"
          "${DOUBLE_BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(
  valijson_tests
  valijson_tests.cpp
  ${SCHEMA_BASE_NAME}.cpp
  ${DOUBLE_BASE_NAME}.cpp
  ${INT_BASE_NAME}.cpp)
target_include_directories(valijson_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(valijson_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(
  valijson_tests
  PRIVATE json2cpp_warnings
          json2cpp_options)

target_link_system_libraries(
  valijson_tests
  PRIVATE Catch2::Catch2WithMain
          ValiJSON::valijson)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  valijson_tests
  TEST_PREFIX
  "unittests."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)

# Add a file containing a set of constexpr tests
add_executable(constexpr_tests constexpr_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(constexpr_tests PRIVATE json2cpp_options json2cpp_warnings Catch2::Catch2WithMain)

target_include_directories(constexpr_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(constexpr_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

catch_discover_tests(
  constexpr_tests
  TEST_PREFIX
  "constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "constexpr."
  OUTPUT_SUFFIX
  .xml)

# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
add_executable(relaxed_constexpr_tests constexpr_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(relaxed_constexpr_tests PRIVATE json2cpp_options json2cpp_warnings Catch2::Catch2WithMain)
target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
target_include_directories(relaxed_constexpr_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(relaxed_constexpr_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

catch_discover_tests(
  relaxed_constexpr_tests
  TEST_PREFIX
  "relaxed_constexpr."
  REPORTER
  XML
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "relaxed_constexpr."
  OUTPUT_SUFFIX
  .xml)

if(json2cpp_ENABLE_LARGE_TESTS)
  set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/schema")
  add_custom_command(
    DEPENDS json2cpp
    OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
    COMMAND json2cpp "schema" "${CMAKE_SOURCE_DIR}/examples/Energy+.schema.epJSON" "${BASE_NAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

  # Add a file containing a set of constexpr_schema tests
  add_executable(constexpr_schema_tests constexpr_schema_tests.cpp "${BASE_NAME}_impl.hpp")
  target_link_libraries(constexpr_schema_tests PRIVATE json2cpp_options json2cpp_warnings Catch2::Catch2WithMain)
  target_include_directories(constexpr_schema_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
  target_include_directories(constexpr_schema_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

  if(MSVC)
    target_compile_options(constexpr_schema_tests PRIVATE "/bigobj")
  endif()

  # disable analysis for these very large generated bits of code
  set_target_properties(constexpr_schema_tests PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

  catch_discover_tests(
    constexpr_schema_tests
    TEST_PREFIX
    "constexpr_schema."
    REPORTER
    XML
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "constexpr_schema."
    OUTPUT_SUFFIX
    .xml)

  # Disable the constexpr_schema portion of the test, and build again this allows us to have an executable that we can debug when
  # things go wrong with the constexpr_schema testing
  add_executable(relaxed_constexpr_schema_tests constexpr_schema_tests.cpp "${BASE_NAME}_impl.hpp")
  target_link_libraries(relaxed_constexpr_schema_tests PRIVATE json2cpp_options json2cpp_warnings Catch2::Catch2WithMain)
  target_compile_definitions(relaxed_constexpr_schema_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
  target_include_directories(relaxed_constexpr_schema_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
  target_include_directories(relaxed_constexpr_schema_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

  if(MSVC)
    target_compile_options(relaxed_constexpr_schema_tests PRIVATE "/bigobj")
  endif()

  # disable analysis for these very large generated bits of code
  set_target_properties(relaxed_constexpr_schema_tests PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")

  catch_discover_tests(
    relaxed_constexpr_schema_tests
    TEST_PREFIX
    "relaxed_constexpr_schema."
    REPORTER
    XML 
    OUTPUT_DIR
    .
    OUTPUT_PREFIX
    "relaxed_constexpr_schema."
    OUTPUT_SUFFIX
    .xml)

endif()
