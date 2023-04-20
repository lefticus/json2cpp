#include "test_json.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Can read object size")
{
  const auto &document = compiled_json::test_json::get();

  REQUIRE(document.size() == 1);
}


TEST_CASE("Can iterate object")
{
  const auto &document = compiled_json::test_json::get();

  std::size_t elements = 0;
  for (const auto &json : document) {
    REQUIRE(!json.is_null());
    ++elements;
  }

  REQUIRE(elements == document.size());
}

TEST_CASE("Can read iterator key")
{
  const auto &document = compiled_json::test_json::get();
  REQUIRE(document.begin().key() == "glossary");
}
