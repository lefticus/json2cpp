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
#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>


int main(int argc, const char **argv)
{
  try {
    CLI::App app("json2cpp version 0.0.1");

    std::string document_name;
    std::filesystem::path input_file_name;
    std::filesystem::path output_base_name;

    bool show_version = false;
    app.add_flag("--version", show_version, "Show version information");
    app.add_option("<document_name>", document_name);
    app.add_option("<input_file_name>", input_file_name);
    app.add_option("<output_base_name>", output_base_name);
    CLI11_PARSE(app, argc, argv);

    compile_to(document_name, input_file_name, output_base_name);
  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
