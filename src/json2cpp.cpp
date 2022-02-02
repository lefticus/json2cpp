#include "json2cpp.hpp"
#include <fstream>

std::string compile(const nlohmann::json &value, std::size_t &obj_count, std::vector<std::string> &lines)
{
  const auto current_object_number = obj_count++;

  const auto json_string = [](const auto &str) { return fmt::format("R\"string({})string\"", str); };

  if (value.is_object()) {
    std::vector<std::string> pairs;
    for (auto itr = value.begin(); itr != value.end(); ++itr) {
      pairs.push_back(fmt::format(
        "constexpr_json::value_pair_t{{{}, {{{}}}}},", json_string(itr.key()), compile(*itr, obj_count, lines)));
    }

    lines.push_back(fmt::format(
      "static constexpr std::array<constexpr_json::value_pair_t, {}> object_data_{} = {{", pairs.size(), current_object_number));

    std::transform(pairs.begin(), pairs.end(), std::back_inserter(lines), [](const auto &pair) {
      return fmt::format("  {}", pair);
    });

    lines.emplace_back("};");

    return fmt::format("constexpr_json::object_t{{object_data_{}}}", current_object_number);
  } else if (value.is_array()) {
    std::vector<std::string> entries;
    std::transform(value.begin(), value.end(), std::back_inserter(entries), [&](const auto &child) {
      return fmt::format("{},", compile(child, obj_count, lines));
    });


    lines.push_back(
      fmt::format("static constexpr std::array<constexpr_json::json, {}> object_data_{} = {{{{", entries.size(), current_object_number));

    std::transform(entries.begin(), entries.end(), std::back_inserter(lines), [](const auto &entry) {
      return fmt::format("  {}", entry);
    });

    lines.emplace_back("}};");

    return fmt::format("constexpr_json::array_t{{object_data_{}}}", current_object_number);


  } else if (value.is_number_float()) {
    return fmt::format("double{{{}}}", value.get<double>());
  } else if (value.is_number_unsigned()) {
    return fmt::format("std::uint64_t{{{}}}", value.get<std::uint64_t>());
  } else if (value.is_number() && !value.is_number_unsigned()) {
    return fmt::format("std::int64_t{{{}}}", value.get<std::uint64_t>());
  } else if (value.is_boolean()) {
    return fmt::format("bool{{{}}}", value.get<bool>());
  } else if (value.is_string()) {
    return fmt::format("std::string_view{{{}}}", json_string(value.get<std::string>()));
  }

  return "unhandled";
}

compile_results compile(const std::string_view document_name, const nlohmann::json &json)
{

  std::size_t obj_count{ 0 };

  compile_results results;

  results.hpp.push_back(fmt::format("#ifndef {}_COMPILED_JSON", document_name));
  results.hpp.push_back(fmt::format("#define {}_COMPILED_JSON", document_name));

  results.hpp.emplace_back("#include <json2cpp/constexpr_json.hpp>");

  results.hpp.push_back(fmt::format("namespace compiled_json::{} {{", document_name));
  results.hpp.push_back(fmt::format("  constexpr_json::json get_{}();", document_name));
  results.hpp.emplace_back("}");

  results.hpp.emplace_back("#endif");


  results.impl.push_back(fmt::format(
    "// Just in case the user wants to use the entire document in a constexpr context, it can be included safely"));
  results.impl.push_back(fmt::format("#ifndef {}_COMPILED_JSON_IMPL", document_name));
  results.impl.push_back(fmt::format("#define {}_COMPILED_JSON_IMPL", document_name));

  results.impl.emplace_back("#include <json2cpp/constexpr_json.hpp>");

  results.impl.push_back(fmt::format("namespace compiled_json::{} {{", document_name));


  const auto last_obj_name = compile(json, obj_count, results.impl);

  results.impl.push_back(fmt::format("static constexpr auto document = constexpr_json::json{{{}}};", last_obj_name));

  results.impl.push_back(fmt::format("constexpr_json::json get_{}() {{ return document; }}", document_name));
  results.impl.emplace_back("}");
  results.impl.emplace_back("#endif");

  spdlog::info("{} JSON objects processed.", obj_count);

  return results;
}


compile_results compile(const std::string_view document_name, const std::filesystem::path &filename)
{
  spdlog::info("Loading file: '{}'", filename.native());

  std::ifstream input(filename);
  nlohmann::json document;
  input >> document;

  spdlog::info("File loaded");

  return compile(document_name, document);
}

void write_compilation([[maybe_unused]] std::string_view document_name,
  const compile_results &results,
  const std::filesystem::path &base_output)
{

  const auto append_extension = [](std::filesystem::path name, std::string_view ext) { return name += ext; };


  const auto hpp_name = append_extension(base_output, ".hpp");
  const auto cpp_name = append_extension(base_output, ".cpp");
  const auto impl_name = append_extension(base_output, "_impl.hpp");

  std::ofstream hpp(hpp_name);
  for (const auto &line : results.hpp) { hpp << line << '\n'; }

  std::ofstream impl(impl_name);
  for (const auto &line : results.impl) { impl << line << '\n'; }

  std::ofstream cpp(cpp_name);
  cpp << fmt::format("#include \"{}\"\n", impl_name.filename().string());
}

void compile_to(const std::string_view document_name,
  const nlohmann::json &json,
  const std::filesystem::path &base_output)
{
  write_compilation(document_name, compile(document_name, json), base_output);
}


void compile_to(const std::string_view document_name,
  const std::filesystem::path &filename,
  const std::filesystem::path &base_output)
{
  write_compilation(document_name, compile(document_name, filename), base_output);
}
