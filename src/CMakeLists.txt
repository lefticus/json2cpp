find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(nlohmann_json CONFIG)

# Generic test that uses conan libs
add_executable(json2cpp main.cpp json2cpp.cpp)
target_link_libraries(
  json2cpp
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog
          nlohmann_json::nlohmann_json)
