#include "allof_integers_and_numbers.schema.hpp"
#include "array_doubles_10_20_30_40.hpp"
#include "array_integers_10_20_30_40.hpp"
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include <catch2/catch_test_macros.hpp>
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
#include <json2cpp/json2cpp_adapter.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>


TEST_CASE("Can load a valijson schema")
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::adapters::json2cppJsonAdapter;


  // Parse JSON schema content using valijson
  Schema mySchema;
  SchemaParser parser;
  const json2cppJsonAdapter mySchemaAdapter(compiled_json::allof_integers_and_numbers_schema::get());
  CHECK_NOTHROW(parser.populateSchema(mySchemaAdapter, mySchema));
}


TEST_CASE("Validation fails where expected")
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::adapters::json2cppJsonAdapter;


  // Parse JSON schema content using valijson
  Schema mySchema;
  SchemaParser parser;
  const json2cppJsonAdapter mySchemaAdapter(compiled_json::allof_integers_and_numbers_schema::get());
  CHECK_NOTHROW(parser.populateSchema(mySchemaAdapter, mySchema));

  Validator validator;
  const json2cppJsonAdapter myTargetAdapter(compiled_json::array_doubles_10_20_30_40::get());

  REQUIRE_FALSE(validator.validate(mySchema, myTargetAdapter, nullptr));
}


TEST_CASE("Can validate a document")
{
  using valijson::Schema;
  using valijson::SchemaParser;
  using valijson::Validator;
  using valijson::adapters::json2cppJsonAdapter;


  // Parse JSON schema content using valijson
  Schema mySchema;
  SchemaParser parser;
  const json2cppJsonAdapter mySchemaAdapter(compiled_json::allof_integers_and_numbers_schema::get());
  CHECK_NOTHROW(parser.populateSchema(mySchemaAdapter, mySchema));

  Validator validator;
  const json2cppJsonAdapter myTargetAdapter(compiled_json::array_integers_10_20_30_40::get());

  REQUIRE(validator.validate(mySchema, myTargetAdapter, nullptr));
}
