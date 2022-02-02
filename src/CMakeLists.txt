find_package(fmt CONFIG)
find_package(spdlog CONFIG)
find_package(docopt CONFIG)
find_package(nlohmann_json CONFIG)

# Generic test that uses conan libs
add_executable(intro main.cpp json_compiler.cpp)
target_link_libraries(
  intro
  PRIVATE project_options
          project_warnings
          docopt::docopt
          fmt::fmt
          spdlog::spdlog
          nlohmann_json::nlohmann_json)
