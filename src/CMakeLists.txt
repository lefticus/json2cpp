# Generic test that uses conan libs
add_executable(json2cpp main.cpp json2cpp.cpp)
add_executable(json2cpp::json2cpp ALIAS json2cpp)
target_link_libraries(json2cpp PRIVATE json2cpp_options json2cpp_warnings)

target_link_system_libraries(
  json2cpp
  PRIVATE
  CLI11::CLI11
  fmt::fmt
  spdlog::spdlog
  nlohmann_json::nlohmann_json)

install(TARGETS json2cpp)
install(DIRECTORY ../include DESTINATION .)

if(json2cpp_ENABLE_LARGE_TESTS)
  set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/schema")
  add_custom_command(
    DEPENDS json2cpp
    OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
    COMMAND json2cpp "energyplus_schema" "${CMAKE_SOURCE_DIR}/examples/Energy+.schema.epJSON" "${BASE_NAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

  add_executable(schema_validator schema_validator.cpp "${BASE_NAME}.cpp")
  add_executable(json2cpp::schema_validator ALIAS schema_validator)
  target_link_libraries(schema_validator PRIVATE json2cpp_options json2cpp_warnings)
  target_link_system_libraries(
    schema_validator
    PRIVATE
    CLI11::CLI11
    fmt::fmt
    spdlog::spdlog
    ValiJSON::valijson
    nlohmann_json::nlohmann_json)

  target_include_directories(schema_validator PRIVATE "${CMAKE_SOURCE_DIR}/include")
  target_include_directories(schema_validator PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

  if(MSVC)
    target_compile_options(schema_validator PRIVATE "/bigobj")
  endif()

  # disable analysis for these very large generated bits of code
  set_target_properties(schema_validator PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
endif()
