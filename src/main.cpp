#include <functional>
#include <iostream>
#include <filesystem>
#include <fstream>

#include <docopt/docopt.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>

static constexpr auto USAGE =
  R"(json_compiler

    Usage:
          json_compiler <file_name>
          json_compiler (-h | --help)
          json_compiler --version
 Options:
          -h --help     Show this screen.
          --version     Show version.
)";

int main(int argc, const char **argv)
{
  try {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
      { std::next(argv), std::next(argv, argc) },
      true,// show help if requested
      "json_compiler 0.1");// version string

    std::filesystem::path filename = args.at("<file_name>").asString();
    spdlog::info("Loading file: '{}'", filename.native());

    std::ifstream input(filename);
    nlohmann::json document;
    input >> document;

    spdlog::info("File loaded");


  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
