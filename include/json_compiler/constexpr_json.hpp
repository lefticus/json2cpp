#ifndef CONSTEXPR_JSON_HPP_INCLUDED
#define CONSTEXPR_JSON_HPP_INCLUDED

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string_view>
#include <variant>
#include <cassert>

namespace constexpr_json {
template<typename T> struct span
{
  template<std::size_t Size>
  constexpr span(const std::array<T, Size> &input) : begin_{ &*input.begin() }, end_{ &*input.end() }
  {}

  constexpr const T *begin() const { return begin_; }

  constexpr const T *end() const { return end_; }

  constexpr std::size_t size() const { return static_cast<std::size_t>(std::distance(begin_, end_)); }

  const T *begin_;
  const T *end_;
};

struct json;

using array_t = span<json>;
using object_t = span<std::pair<std::string_view, json>>;
using binary_t = span<std::uint8_t>;

using data_t = std::
  variant<std::monostate, bool, binary_t, array_t, object_t, std::int64_t, std::uint64_t, double, std::string_view>;

struct json
{
  struct iterator
  {
    constexpr iterator(const json &value, bool end = false)
      : parent_value_(&value), is_object_{ value.is_object() }, is_array_{ value.is_array() }
    {
      if (end == false) {
        if (is_array_) {
          index_ = value.array().size();
        } else if (is_object_) {
          index_ = value.object().size();
        }
      }
    }

    constexpr const json &operator*() const
    {
      if (is_array_) {
        return (*parent_value_)[index_];
      } else if (is_object_) {
        return std::next(parent_value_->object().begin(), index_)->second;
      } else {
        throw std::runtime_error("json value is neither object nor array, it cannot be iterated");
      }
    }

    constexpr std::size_t index() const { return index_; }

    constexpr std::string_view key() const
    {
      if (is_object_) {
        return std::next(parent_value_->object().begin(), index_)->first;
      } else {
        throw std::runtime_error("json value is neither object nor array, it cannot be iterated");
      }
    }

    constexpr bool operator==(const iterator &other) const
    {
      return other.parent_value_ == parent_value_ && other.index_ == index_;
    }

    constexpr bool operator<(const iterator &other) const
    {
      assert(other.parent_value_ == parent_value_);
      return index_ < other.index_;
    }

    constexpr iterator &operator++()
    {
      ++index_;
      return *this;
    }

    constexpr iterator operator++(int)
    {
      iterator result{ *this };
      index_++;
      return result;
    }

    const json *parent_value_{ nullptr };

    std::size_t index_{ 0 };
    bool is_object_{ false };
    bool is_array_{ false };
  };

  constexpr iterator begin() const { return iterator{ *this }; }

  constexpr iterator end() const { return iterator{ *this, true }; }

  constexpr iterator cbegin() const { return begin(); }

  constexpr iterator cend() const { return end(); }

  constexpr const json &operator[](const std::size_t idx) const
  {
    if (const auto &children = array(); children.size() < idx) {
      return *std::next(children.begin(), idx);
    } else {
      throw std::runtime_error("index out of range");
    }
  }

  constexpr const json &operator[](const std::string_view key) const
  {
    const auto &children = object();

    for (const auto &value : children) {
      if (value.first == key) { return value.second; }
    }

    throw std::runtime_error("Key not found");
  }

  constexpr const array_t &array() const
  {
    if (const auto *result = std::get_if<array_t>(&data); result != nullptr) {
      return *result;
    } else {
      throw std::runtime_error("value is not an array type");
    }
  }

  constexpr const object_t &object() const
  {
    if (const auto *result = std::get_if<object_t>(&data); result != nullptr) {
      return *result;
    } else {
      throw std::runtime_error("value is not an object type");
    }
  }

  constexpr bool is_object() const noexcept { return std::holds_alternative<object_t>(data); }

  constexpr bool is_array() const noexcept { return std::holds_alternative<array_t>(data); }

  constexpr bool is_string() const noexcept { return std::holds_alternative<std::string_view>(data); }

  constexpr bool is_boolean() const noexcept { return std::holds_alternative<bool>(data); }

  constexpr bool is_structured() const noexcept { return is_object() || is_array(); }

  constexpr bool is_number() const noexcept { return is_number_integer() || is_number_float(); }

  constexpr double as_number_float() const
  {
    if (const double *value = std::get_if<double>(&data); value != nullptr) { return *value; }
  }
  constexpr double as_boolean() const
  {
    if (const bool *value = std::get_if<bool>(&data); value != nullptr) { return *value; }
  }

  constexpr std::string_view as_string() const
  {
    if (const auto *value = std::get_if<std::string_view>(&data); value != nullptr) { return *value; }
  }

  constexpr operator double() const { return as_number_float(); }

  constexpr bool is_number_integer() const noexcept { return is_number_signed() || is_number_unsigned(); }

  constexpr bool is_number_signed() const noexcept { return std::holds_alternative<std::int64_t>(data); }

  constexpr bool is_number_unsigned() const noexcept { return std::holds_alternative<std::uint64_t>(data); }

  constexpr bool is_number_float() const noexcept { return std::holds_alternative<double>(data); }

  constexpr bool is_null() const noexcept { return std::holds_alternative<std::monostate>(data); }

  constexpr bool is_binary() const noexcept { return std::holds_alternative<binary_t>(data); }

  constexpr bool is_primitive() const noexcept
  {
    return is_null() || is_string() || is_boolean() || is_number() || is_binary();
  }

  data_t data;
};

using value_pair_t = std::pair<std::string_view, json>;
}// namespace constexpr_json

#endif
