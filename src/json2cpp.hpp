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

#ifndef JSON2CPP_HPP
#define JSON2CPP_HPP

#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

struct compile_results
{
  std::vector<std::string> hpp;
  std::vector<std::string> impl;
};


std::string compile(const nlohmann::json &value, std::size_t &obj_count, std::vector<std::string> &lines);


compile_results compile(const std::string_view document_name, const nlohmann::json &json);

compile_results compile(const std::string_view document_name, const std::filesystem::path &filename);


void write_compilation(std::string_view document_name,
  const compile_results &results,
  const std::filesystem::path &base_output);


void compile_to(const std::string_view document_name,
  const nlohmann::json &json,
  const std::filesystem::path &base_output);

void compile_to(const std::string_view document_name,
  const std::filesystem::path &filename,
  const std::filesystem::path &base_output);


#endif
