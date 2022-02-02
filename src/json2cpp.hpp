#ifndef JSON2CPP_HPP
#define JSON2CPP_HPP

#include <vector>
#include <string>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

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
