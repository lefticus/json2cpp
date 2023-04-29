find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(nlohmann_json CONFIG)
find_package(ValiJSON CONFIG)

add_library(libjson2cpp INTERFACE
  ../include/json2cpp/json2cpp.hpp
  ../include/json2cpp/json2cpp_adapter.hpp
)

target_link_libraries(
  libjson2cpp
  INTERFACE ValiJSON::valijson)

target_include_directories(
  libjson2cpp
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
            $<INSTALL_INTERFACE:include>)

install(DIRECTORY ../include/json2cpp TYPE INCLUDE)
add_library(json2cpp::libjson2cpp ALIAS libjson2cpp)
install(TARGETS libjson2cpp EXPORT json2cppTargets)
export(EXPORT json2cppTargets NAMESPACE json2cpp::)
configure_file("json2cppConfig.cmake" "." COPYONLY)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(json2cppConfigVersion.cmake COMPATIBILITY SameMajorVersion)

# installation
install(
  EXPORT json2cppTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/json2cpp
  NAMESPACE json2cpp::
)

install(FILES json2cppConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/json2cppConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/json2cpp
)

# Generic test that uses conan libs
add_executable(json2cpp main.cpp json2cpp.cpp)
target_link_libraries(
  json2cpp
  PRIVATE libjson2cpp
          project_options
          project_warnings
          docopt_s
          fmt::fmt
          spdlog::spdlog
          nlohmann_json::nlohmann_json)
install(TARGETS json2cpp)

if(ENABLE_LARGE_TESTS)
  set(BASE_NAME "${CMAKE_CURRENT_BINARY_DIR}/schema")
  add_custom_command(
    DEPENDS json2cpp
    OUTPUT "${BASE_NAME}_impl.hpp" "${BASE_NAME}.hpp" "${BASE_NAME}.cpp"
    COMMAND json2cpp "energyplus_schema" "${CMAKE_SOURCE_DIR}/examples/Energy+.schema.epJSON" "${BASE_NAME}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")

  add_executable(schema_validator schema_validator.cpp "${BASE_NAME}.cpp")
  target_link_libraries(
    schema_validator
    PRIVATE json2cpp
            project_options
            project_warnings
            docopt_s
            fmt::fmt
            spdlog::spdlog
            ValiJSON::valijson
            nlohmann_json::nlohmann_json)

  target_include_directories(schema_validator PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

  if(MSVC)
    target_compile_options(schema_validator PRIVATE "/bigobj")
  endif()

  # disable analysis for these very large generated bits of code
  set_target_properties(schema_validator PROPERTIES CXX_CPPCHECK "" CXX_CLANG_TIDY "")
endif()
