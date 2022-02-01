#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>

#include <docopt/docopt.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

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

std::string to_string(const nlohmann::json &value, std::size_t &obj_count, std::vector<std::string> &lines)
{
  const auto current_object_number = obj_count++;

  const auto json_string = [](const auto &str) { return fmt::format("R\"json_string({})json_string\"", str); };

  if (value.is_object()) {
    std::vector<std::string> pairs;
    for (auto itr = value.begin(); itr != value.end(); ++itr) {
      pairs.push_back(fmt::format(
        "value_pair_t{{std::string_view{{{}}}, {}}},", json_string(itr.key()), to_string(*itr, obj_count, lines)));
    }

    lines.push_back(fmt::format(
      "static constexpr std::array<value_pair_t, {}> object_data_{} = {{", pairs.size(), current_object_number));

    std::transform(pairs.begin(), pairs.end(), std::back_inserter(lines), [](const auto &pair) {
      return fmt::format("  {}", pair);
    });

    lines.emplace_back("};");

    return fmt::format("json{{object_data_{}}}", current_object_number);
  } else if (value.is_array()) {
    std::vector<std::string> entries;
    std::transform(value.begin(), value.end(), std::back_inserter(entries), [&](const auto &child) {
      return fmt::format("{},", to_string(child, obj_count, lines));
    });


    lines.push_back(
      fmt::format("static constexpr std::array<json, {}> object_data_{} = {{", entries.size(), current_object_number));

    std::transform(entries.begin(), entries.end(), std::back_inserter(lines), [](const auto &entry) {
      return fmt::format("  {}", entry);
    });

    lines.emplace_back("};");

    return fmt::format("json{{object_data_{}}}", current_object_number);


  } else if (value.is_number_float()) {
    return fmt::format("json{{data_t{{double{{{}}}}}}}", value.get<double>());
  } else if (value.is_number_unsigned()) {
    return fmt::format("json{{data_t{{std::uint64_t{{{}}}}}}}", value.get<std::uint64_t>());
  } else if (value.is_number() && !value.is_number_unsigned()) {
    return fmt::format("json{{data_t{{std::int64_t{{{}}}}}}}", value.get<std::uint64_t>());
  } else if (value.is_boolean()) {
    return fmt::format("json{{data_t{{bool{{{}}}}}}}", value.get<bool>());
  } else if (value.is_string()) {
    return fmt::format("json{{data_t{{std::string_view{{{}}}}}}}", json_string(value.get<std::string>()));
  }

  return "unhandled";
}


int main(int argc, const char **argv)
{
  try {
    std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
      { std::next(argv), std::next(argv, argc) },
      true,// show help if requested
      "json_compiler 0.0.1");// version string

    std::filesystem::path filename = args.at("<file_name>").asString();
    spdlog::info("Loading file: '{}'", filename.native());

    std::ifstream input(filename);
    nlohmann::json document;
    input >> document;

    spdlog::info("File loaded");


    std::size_t obj_count{ 0 };
    std::vector<std::string> lines;

    const auto str = to_string(document, obj_count, lines);

    for (const auto &line : lines) { std::cout << line << '\n'; }

    std::cout << "static constexpr auto document = " << str << ";\n";

  } catch (const std::exception &e) {
    spdlog::error("Unhandled exception in main: {}", e.what());
  }
}
