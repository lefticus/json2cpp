#include "test_json_impl.hpp"
#include <catch2/catch_test_macros.hpp>


TEST_CASE("Can read object size")
{
  constexpr auto &document = compiled_json::test_json::impl::document;// NOLINT No, I'm not going to mark this `const`

  STATIC_REQUIRE(document.size() == 1);
}

constexpr auto count_elements()
{
  constexpr auto &document = compiled_json::test_json::impl::document;// NOLINT No, I'm not going to mark this `const`

  std::size_t elements = 0;
  for (const auto &json : document) {
    // count_if is not constexpr in C++17
    // cppcheck-suppress useStlAlgorithm
    if (!json.is_null()) { ++elements; }
  }

  return elements;
}

TEST_CASE("Can iterate object") { STATIC_REQUIRE(count_elements() == compiled_json::test_json::impl::document.size()); }

TEST_CASE("Can read iterator key")
{
  constexpr auto &document = compiled_json::test_json::impl::document;// NOLINT No, I'm not going to mark this `const`

  STATIC_REQUIRE(document.begin().key() == "glossary");
}
