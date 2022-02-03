find_package(Catch2 REQUIRED)
find_package(ValiJSON REQUIRED)

include(CTest)
include(Catch)

add_library(catch_main OBJECT catch_main.cpp)
target_link_libraries(catch_main PUBLIC Catch2::Catch2)
target_link_libraries(catch_main PRIVATE project_options)

set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/test_json")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
  COMMAND json2cpp "test_json" "${CMAKE_SOURCE_DIR}/examples/test.json" "${BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(tests tests.cpp "${BASE_NAME}.cpp")
target_include_directories(tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(tests PRIVATE project_warnings project_options catch_main)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  tests
  TEST_PREFIX
  "unittests."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)





add_executable(valijson_tests valijson_tests.cpp)
target_include_directories(valijson_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(valijson_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(valijson_tests PRIVATE project_warnings project_options catch_main ValiJSON::valijson)

# automatically discover tests that are defined in catch based test files you can modify the unittests. Set TEST_PREFIX
# to whatever you want, or use different for different binaries
catch_discover_tests(
  valijson_tests
  TEST_PREFIX
  "unittests."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "unittests."
  OUTPUT_SUFFIX
  .xml)







# Add a file containing a set of constexpr tests
add_executable(constexpr_tests constexpr_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(constexpr_tests PRIVATE project_options project_warnings catch_main)
target_include_directories(constexpr_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(constexpr_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

catch_discover_tests(
  constexpr_tests
  TEST_PREFIX
  "constexpr."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "constexpr."
  OUTPUT_SUFFIX
  .xml)

# Disable the constexpr portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr testing
add_executable(relaxed_constexpr_tests constexpr_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(relaxed_constexpr_tests PRIVATE project_options project_warnings catch_main)
target_compile_definitions(relaxed_constexpr_tests PRIVATE -DCATCH_CONFIG_RUNTIME_STATIC_REQUIRE)
target_include_directories(relaxed_constexpr_tests PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(relaxed_constexpr_tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

catch_discover_tests(
  relaxed_constexpr_tests
  TEST_PREFIX
  "relaxed_constexpr."
  REPORTER
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "relaxed_constexpr."
  OUTPUT_SUFFIX
  .xml)

set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/schema")
add_custom_command(
  DEPENDS json2cpp
  OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
  COMMAND json2cpp "schema" "${CMAKE_SOURCE_DIR}/examples/Energy+.schema.epJSON" "${BASE_NAME}"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

# Add a file containing a set of constexpr_schema tests
add_executable(constexpr_schema_tests constexpr_schema_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(constexpr_schema_tests PRIVATE project_options project_warnings catch_main)
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
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "constexpr_schema."
  OUTPUT_SUFFIX
  .xml)

# Disable the constexpr_schema portion of the test, and build again this allows us to have an executable that we can debug when
# things go wrong with the constexpr_schema testing
add_executable(relaxed_constexpr_schema_tests constexpr_schema_tests.cpp "${BASE_NAME}_impl.hpp")
target_link_libraries(relaxed_constexpr_schema_tests PRIVATE project_options project_warnings catch_main)
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
  xml
  OUTPUT_DIR
  .
  OUTPUT_PREFIX
  "relaxed_constexpr_schema."
  OUTPUT_SUFFIX
  .xml)
