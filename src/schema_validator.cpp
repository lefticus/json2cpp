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
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <functional>
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#include <iostream>

#include <CLI/CLI.hpp>
#include <spdlog/spdlog.h>

#include <json2cpp/json2cpp_adapter.hpp>
#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include "schema.hpp"


bool validate(const std::filesystem::path &schema_file_name, const std::filesystem::path &file_to_validate)
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::adapters::json2cppJsonAdapter;
  using valijson::adapters::NlohmannJsonAdapter;

  // Parse JSON schema content using valijson
  spdlog::info("Creating Schema object");
  Schema mySchema;
  spdlog::info("Creating SchemaParser object");
  SchemaParser parser;


  spdlog::info("Creating nlohmann::json object");
  nlohmann::json schema;
  spdlog::info("Opening json file");
  std::ifstream schema_file(schema_file_name);
  spdlog::info("Loading json file");
  schema_file >> schema;
  spdlog::info("Creating NlohmannJsonAdapter object");
  NlohmannJsonAdapter mySchemaAdapter(schema);
  spdlog::info("parser.populateSchema object");
  parser.populateSchema(mySchemaAdapter, mySchema);

  spdlog::info("Creating Validator object");
  Validator validator;
  spdlog::info("Creating nlohmann::json object");
  nlohmann::json document;
  spdlog::info("Opening json file");
  std::ifstream input_file(file_to_validate);
  spdlog::info("Loading json file");
  input_file >> document;
  spdlog::info("Creating NlohmannJsonAdapter object");
  NlohmannJsonAdapter myTargetAdapter(document);

  spdlog::info("validator.validate");
  const auto result = validator.validate(mySchema, myTargetAdapter, nullptr);
  spdlog::info("returning result {}", result);

  return result;
}

bool validate_internal(const std::filesystem::path &file_to_validate)
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::adapters::json2cppJsonAdapter;
  using valijson::adapters::NlohmannJsonAdapter;

  // Parse JSON schema content using valijson
  spdlog::info("Creating Schema object");
  Schema mySchema;
  spdlog::info("Creating SchemaParser object");
  SchemaParser parser;
  spdlog::info("Creating json2cppJsonAdapter object");
  json2cppJsonAdapter mySchemaAdapter(compiled_json::energyplus_schema::get());
  spdlog::info("parser.populateSchema object");
  parser.populateSchema(mySchemaAdapter, mySchema);

  spdlog::info("Creating Validator object");
  Validator validator;
  spdlog::info("Creating nlohmann::json object");
  nlohmann::json document;
  spdlog::info("Opening json file");
  std::ifstream input_file(file_to_validate);
  spdlog::info("Loading json file");
  input_file >> document;
  spdlog::info("Creating NlohmannJsonAdapter object");
  NlohmannJsonAdapter myTargetAdapter(document);

  spdlog::info("validator.validate");
  const auto result = validator.validate(mySchema, myTargetAdapter, nullptr);
  spdlog::info("returning result {}", result);

  return result;
}

template<typename JSON>
void walk_internal(std::int64_t &int_sum,
  double &double_sum,
  std::size_t &string_sizes,
  int &null_count,
  int &array_count,
  int &object_count,
  const JSON &obj)
{
  if (obj.is_number_integer()) {
    int_sum += obj.template get<std::int64_t>();
  } else if (obj.is_number_float()) {
    double_sum += obj.template get<double>();
  } else if (obj.is_string()) {
    string_sizes += obj.template get<std::string_view>().size();
  } else if (obj.is_null()) {
    ++null_count;
  } else if (obj.is_array()) {
    ++array_count;
    for (const auto &child : obj) {
      walk_internal(int_sum, double_sum, string_sizes, null_count, array_count, object_count, child);
    }
  } else if (obj.is_object()) {
    ++object_count;
    for (const auto &child : obj) {
      walk_internal(int_sum, double_sum, string_sizes, null_count, array_count, object_count, child);
    }
  }
}

template<typename JSON> void walk(const JSON &objects)
{
  std::int64_t int_sum{};
  double double_sum{};
  std::size_t string_sizes{};
  int null_count{};
  int array_count{};
  int object_count{};

  spdlog::info("Starting tree walk");

  walk_internal(int_sum, double_sum, string_sizes, null_count, array_count, object_count, objects);

  spdlog::info("{} {} {} {} {} {}", int_sum, double_sum, string_sizes, null_count, array_count, object_count);
}

int main(int argc, const char **argv)
{
  try {
    CLI::App app("schema_validator version 0.0.1");

    std::string document_name;
    std::filesystem::path schema_file_name;
    std::filesystem::path document_to_validate;

    bool do_walk = false;
    bool internal = false;
    bool show_version = false;
    app.add_option("<schema_file>", schema_file_name);
    auto *doc = app.add_option("<document_to_validate>", document_to_validate);
    app.add_flag("--version", show_version, "Show version information");
    app.add_flag("--walk", do_walk, "Just walk the schema and count objects (perf test)")->excludes(doc);
    app.add_flag("--internal", internal, "Use internal schema");

    CLI11_PARSE(app, argc, argv);

    if (do_walk) {
      if (internal) {
        walk(compiled_json::energyplus_schema::get());
      } else {
        spdlog::info("Creating nlohmann::json object");
        nlohmann::json schema;
        spdlog::info("Opening json file");
        std::ifstream schema_file(schema_file_name);
        spdlog::info("Loading json file");
        schema_file >> schema;
        walk(schema);
      }
      return EXIT_SUCCESS;
    }

    if (internal) {
      validate_internal(document_to_validate);
    } else {
      validate(schema_file_name, document_to_validate);
    }

  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
