#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include <docopt/docopt.h>
#include <spdlog/spdlog.h>
#include "json_compiler.hpp"


static constexpr auto USAGE =
  R"(json_compiler

    Usage:
          json_compiler <document_name> <file_name> <output_base_name>
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
      "json_compiler 0.0.1");// version string

    std::string document_name = args.at("<document_name>").asString();
    std::filesystem::path filename = args.at("<file_name>").asString();
    std::filesystem::path output_filename = args.at("<output_base_name>").asString();

    compile_to(document_name, filename, output_filename);

  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
