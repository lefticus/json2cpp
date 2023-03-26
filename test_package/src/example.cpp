#include "test_json_impl.hpp"

int main() {
  constexpr auto &document = compiled_json::test_json::impl::document;
  static_assert(document["glossary"]["title"].get<std::string>() == "example glossary");

  return 0;
}
