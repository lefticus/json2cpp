/*
MIT License

Copyright (c) 2022 Jason Turner

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include "json2cpp.hpp"
#include <docopt/docopt.h>
#include <spdlog/spdlog.h>


static constexpr auto USAGE =
  R"(json2cpp

 Copyright 2022 Jason Turner

    Usage:
          json2cpp <document_name> <file_name> <output_base_name>
          json2cpp (-h | --help)
          json2cpp --version
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
      "json2cpp 0.0.1 Copyright 2022 Jason Turner");// version string

    const std::string document_name = args.at("<document_name>").asString();
    const std::filesystem::path filename = args.at("<file_name>").asString();
    const std::filesystem::path output_filename = args.at("<output_base_name>").asString();

    compile_to(document_name, filename, output_filename);

  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
