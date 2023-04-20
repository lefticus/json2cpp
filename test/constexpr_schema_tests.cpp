#include "schema_impl.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string_view>

TEST_CASE("Basic test of very large energyplus JSON schema")
{
  constexpr auto &document = compiled_json::schema::impl::document;// NOLINT No, I'm not going to mark this `const`

  using namespace std::literals::string_view_literals;

  STATIC_REQUIRE(document["properties"sv]["Version"sv]["patternProperties"sv][".*"sv]["properties"sv]
                         ["version_identifier"sv]["default"sv]
                           .get<std::string_view>()
                 == "22.1"sv);
}
