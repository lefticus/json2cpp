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

/**
 * @file
 *
 * @brief   Adapter implementation for the json2cpp json parser library.
 *
 * Include this file in your program to enable support for json2cpp json.
 *
 * This file defines the following classes (not in this order):
 *  - json2cppJsonAdapter
 *  - json2cppJsonArray
 *  - json2cppJsonValueIterator
 *  - json2cppJsonFrozenValue
 *  - json2cppJsonObject
 *  - json2cppJsonObjectMember
 *  - json2cppJsonObjectMemberIterator
 *  - json2cppJsonValue
 *
 * Due to the dependencies that exist between these classes, the ordering of
 * class declarations and definitions may be a bit confusing. The best place to
 * start is json2cppJsonAdapter. This class definition is actually very small,
 * since most of the functionality is inherited from the BasicAdapter class.
 * Most of the classes in this file are provided as template arguments to the
 * inherited BasicAdapter class.
 */

#pragma once

#include "json2cpp.hpp"
#include <string>

#include <utility>
#include <valijson/exceptions.hpp>
#include <valijson/internal/adapter.hpp>
#include <valijson/internal/basic_adapter.hpp>
#include <valijson/internal/frozen_value.hpp>

namespace valijson {
namespace adapters {

  class json2cppJsonAdapter;
  class json2cppJsonArrayValueIterator;
  class json2cppJsonObjectMemberIterator;

  typedef std::pair<std::string, json2cppJsonAdapter> json2cppJsonObjectMember;

  /**
   * @brief  Light weight wrapper for a json2cppJson array value.
   *
   * This class is light weight wrapper for a json2cppJson array. It provides a
   * minimum set of container functions and typedefs that allow it to be used as
   * an iterable container.
   *
   * An instance of this class contains a single reference to the underlying
   * json2cppJson value, assumed to be an array, so there is very little overhead
   * associated with copy construction and passing by value.
   */
  class json2cppJsonArray
  {
  public:
    typedef json2cppJsonArrayValueIterator const_iterator;
    typedef json2cppJsonArrayValueIterator iterator;

    /// Construct a json2cppJsonArray referencing an empty array.
    json2cppJsonArray() : m_value(emptyArray()) {}

    /**
     * @brief   Construct a json2cppJsonArray referencing a specific json2cppJson
     *          value.
     *
     * @param   value   reference to a json2cppJson value
     *
     * Note that this constructor will throw an exception if the value is not
     * an array.
     */
    explicit json2cppJsonArray(const json2cpp::json &value) : m_value(value)
    {
      if (!value.is_array()) { throwRuntimeError("Value is not an array."); }
    }

    /**
     * @brief   Return an iterator for the first element of the array.
     *
     * The iterator return by this function is effectively the iterator
     * returned by the underlying json2cppJson implementation.
     */
    json2cppJsonArrayValueIterator begin() const;

    /**
     * @brief   Return an iterator for one-past the last element of the array.
     *
     * The iterator return by this function is effectively the iterator
     * returned by the underlying json2cppJson implementation.
     */
    json2cppJsonArrayValueIterator end() const;

    /// Return the number of elements in the array
    size_t size() const { return m_value.size(); }

  private:
    /**
     * @brief   Return a reference to a json2cppJson value that is an empty array.
     *
     * Note that the value returned by this function is a singleton.
     */
    static const json2cpp::json &emptyArray()
    {
      static const json2cpp::json array = json2cpp::json::array();
      return array;
    }

    /// Reference to the contained value
    const json2cpp::json &m_value;
  };

  /**
   * @brief  Light weight wrapper for a json2cppJson object.
   *
   * This class is light weight wrapper for a json2cppJson object. It provides a
   * minimum set of container functions and typedefs that allow it to be used as
   * an iterable container.
   *
   * An instance of this class contains a single reference to the underlying
   * json2cppJson value, assumed to be an object, so there is very little overhead
   * associated with copy construction and passing by value.
   */
  class json2cppJsonObject
  {
  public:
    typedef json2cppJsonObjectMemberIterator const_iterator;
    typedef json2cppJsonObjectMemberIterator iterator;

    /// Construct a json2cppJsonObject referencing an empty object singleton.
    json2cppJsonObject() : m_value(emptyObject()) {}

    /**
     * @brief   Construct a json2cppJsonObject referencing a specific json2cppJson
     *          value.
     *
     * @param   value  reference to a json2cppJson value
     *
     * Note that this constructor will throw an exception if the value is not
     * an object.
     */
    explicit json2cppJsonObject(const json2cpp::json &value) : m_value(value)
    {
      if (!value.is_object()) { throwRuntimeError("Value is not an object."); }
    }

    /**
     * @brief   Return an iterator for this first object member
     *
     * The iterator return by this function is effectively a wrapper around
     * the iterator value returned by the underlying json2cppJson implementation.
     */
    json2cppJsonObjectMemberIterator begin() const;

    /**
     * @brief   Return an iterator for an invalid object member that indicates
     *          the end of the collection.
     *
     * The iterator return by this function is effectively a wrapper around
     * the iterator value returned by the underlying json2cppJson implementation.
     */
    json2cppJsonObjectMemberIterator end() const;

    /**
     * @brief   Return an iterator for the object member with the specified
     *          property name.
     *
     * If an object member with the specified name does not exist, the iterator
     * returned will be the same as the iterator returned by the end() function.
     *
     * @param   propertyName  property name to search for
     */
    json2cppJsonObjectMemberIterator find(const std::string_view propertyName) const;

    /// Returns the number of members belonging to this object.
    size_t size() const { return m_value.size(); }

  private:
    /**
     * @brief   Return a reference to a json2cppJson value that is empty object.
     *
     * Note that the value returned by this function is a singleton.
     */
    static const json2cpp::json &emptyObject()
    {
      static const json2cpp::json object = json2cpp::json::object();
      return object;
    }

    /// Reference to the contained object
    const json2cpp::json &m_value;
  };


  /**
   * @brief   Stores an independent copy of a json2cppJson value.
   *
   * This class allows a json2cppJson value to be stored independent of its original
   * document. json2cppJson makes this easy to do, as it does not perform any
   * custom memory management.
   *
   * @see FrozenValue
   */
  class json2cppJsonFrozenValue : public FrozenValue
  {
  public:
    /**
     * @brief  Make a copy of a json2cppJson value
     *
     * @param  source  the json2cppJson value to be copied
     */
    explicit json2cppJsonFrozenValue(json2cpp::json source) : m_value(std::move(source)) {}

    FrozenValue *clone() const override { return new json2cppJsonFrozenValue(m_value); }

    bool equalTo(const Adapter &other, bool strict) const override;

  private:
    /// Stored json2cppJson value
    json2cpp::json m_value;
  };


  /**
   * @brief   Light weight wrapper for a json2cppJson value.
   *
   * This class is passed as an argument to the BasicAdapter template class,
   * and is used to provide access to a json2cppJson value. This class is responsible
   * for the mechanics of actually reading a json2cppJson value, whereas the
   * BasicAdapter class is responsible for the semantics of type comparisons
   * and conversions.
   *
   * The functions that need to be provided by this class are defined implicitly
   * by the implementation of the BasicAdapter template class.
   *
   * @see BasicAdapter
   */
  class json2cppJsonValue
  {
  public:
    /// Construct a wrapper for the empty object singleton
    json2cppJsonValue() : m_value(emptyObject()) {}

    /// Construct a wrapper for a specific json2cppJson value
    explicit json2cppJsonValue(const json2cpp::json &value) : m_value(value) {}

    /**
     * @brief   Create a new json2cppJsonFrozenValue instance that contains the
     *          value referenced by this json2cppJsonValue instance.
     *
     * @returns pointer to a new json2cppJsonFrozenValue instance, belonging to the
     *          caller.
     */
    FrozenValue *freeze() const { return new json2cppJsonFrozenValue(m_value); }

    /**
     * @brief   Optionally return a json2cppJsonArray instance.
     *
     * If the referenced json2cppJson value is an array, this function will return
     * a std::optional containing a json2cppJsonArray instance referencing the
     * array.
     *
     * Otherwise it will return an empty optional.
     */
    opt::optional<json2cppJsonArray> getArrayOptional() const
    {
      if (m_value.is_array()) { return opt::make_optional(json2cppJsonArray(m_value)); }

      return {};
    }

    /**
     * @brief   Retrieve the number of elements in the array
     *
     * If the referenced json2cppJson value is an array, this function will
     * retrieve the number of elements in the array and store it in the output
     * variable provided.
     *
     * @param   result  reference to size_t to set with result
     *
     * @returns true if the number of elements was retrieved, false otherwise.
     */
    bool getArraySize(size_t &result) const
    {
      if (m_value.is_array()) {
        result = m_value.size();
        return true;
      }

      return false;
    }

    bool getBool(bool &result) const
    {
      if (m_value.is_boolean()) {
        result = m_value.get<bool>();
        return true;
      }

      return false;
    }

    bool getDouble(double &result) const
    {
      if (m_value.is_number_float()) {
        result = m_value.get<double>();
        return true;
      }

      return false;
    }

    bool getInteger(int64_t &result) const
    {
      if (m_value.is_number_integer()) {
        result = m_value.get<int64_t>();
        return true;
      }
      return false;
    }

    /**
     * @brief   Optionally return a json2cppJsonObject instance.
     *
     * If the referenced json2cppJson value is an object, this function will return a
     * std::optional containing a json2cppJsonObject instance referencing the
     * object.
     *
     * Otherwise it will return an empty optional.
     */
    opt::optional<json2cppJsonObject> getObjectOptional() const
    {
      if (m_value.is_object()) { return opt::make_optional(json2cppJsonObject(m_value)); }

      opt::optional<json2cppJsonObject> emptyreturn{};
      return emptyreturn;
    }

    /**
     * @brief   Retrieve the number of members in the object
     *
     * If the referenced json2cppJson value is an object, this function will
     * retrieve the number of members in the object and store it in the output
     * variable provided.
     *
     * @param   result  reference to size_t to set with result
     *
     * @returns true if the number of members was retrieved, false otherwise.
     */
    bool getObjectSize(size_t &result) const
    {
      if (m_value.is_object()) {
        result = m_value.size();
        return true;
      }

      return false;
    }

    bool getString(std::string &result) const
    {
      if (m_value.is_string()) {
        result = m_value.get<std::string_view>();
        return true;
      }

      return false;
    }

    static bool hasStrictTypes() { return true; }

    bool isArray() const { return m_value.is_array(); }

    bool isBool() const { return m_value.is_boolean(); }

    bool isDouble() const { return m_value.is_number_float(); }

    bool isInteger() const { return m_value.is_number_integer(); }

    bool isNull() const { return m_value.is_null(); }

    bool isNumber() const { return m_value.is_number(); }

    bool isObject() const { return m_value.is_object(); }

    bool isString() const { return m_value.is_string(); }

  private:
    /// Return a reference to an empty object singleton
    static const json2cpp::json &emptyObject()
    {
      static const json2cpp::json object = json2cpp::json::object();
      return object;
    }

    /// Reference to the contained json2cppJson value.
    const json2cpp::json &m_value;
  };

  /**
   * @brief   An implementation of the Adapter interface supporting json2cppJson.
   *
   * This class is defined in terms of the BasicAdapter template class, which
   * helps to ensure that all of the Adapter implementations behave consistently.
   *
   * @see Adapter
   * @see BasicAdapter
   */
  class json2cppJsonAdapter
    : public BasicAdapter<json2cppJsonAdapter,
        json2cppJsonArray,
        json2cppJsonObjectMember,
        json2cppJsonObject,
        json2cppJsonValue>
  {
  public:
    /// Construct a json2cppJsonAdapter that contains an empty object
    json2cppJsonAdapter() : BasicAdapter() {}

    /// Construct a json2cppJsonAdapter containing a specific json2cpp Json object
    explicit json2cppJsonAdapter(const json2cpp::json &value) : BasicAdapter(json2cppJsonValue{ value }) {}
  };

  /**
   * @brief   Class for iterating over values held in a JSON array.
   *
   * This class provides a JSON array iterator that dereferences as an instance of
   * json2cppJsonAdapter representing a value stored in the array. It has been
   * implemented using the boost iterator_facade template.
   *
   * @see json2cppJsonArray
   */
  class json2cppJsonArrayValueIterator
  {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = json2cppJsonAdapter;
    using difference_type = json2cppJsonAdapter;
    using pointer = json2cppJsonAdapter *;
    using reference = json2cppJsonAdapter &;

    /**
     * @brief   Construct a new json2cppJsonArrayValueIterator using an existing
     *          json2cppJson iterator.
     *
     * @param   itr  json2cppJson iterator to store
     */
    explicit json2cppJsonArrayValueIterator(const json2cpp::json::const_iterator &itr) : m_itr(itr) {}

    /// Returns a json2cppJsonAdapter that contains the value of the current
    /// element.
    json2cppJsonAdapter operator*() const { return json2cppJsonAdapter(*m_itr); }

    DerefProxy<json2cppJsonAdapter> operator->() const { return DerefProxy<json2cppJsonAdapter>(**this); }

    /**
     * @brief   Compare this iterator against another iterator.
     *
     * Note that this directly compares the iterators, not the underlying
     * values, and assumes that two identical iterators will point to the same
     * underlying object.
     *
     * @param   other  iterator to compare against
     *
     * @returns true   if the iterators are equal, false otherwise.
     */
    bool operator==(const json2cppJsonArrayValueIterator &other) const { return m_itr == other.m_itr; }

    bool operator!=(const json2cppJsonArrayValueIterator &other) const { return !(m_itr == other.m_itr); }

    // cppcheck-suppress functionConst
    const json2cppJsonArrayValueIterator &operator++()
    {
      ++m_itr;

      return *this;
    }

    // cppcheck-suppress functionConst
    json2cppJsonArrayValueIterator operator++(int)
    {
      json2cppJsonArrayValueIterator iterator_pre(m_itr);
      ++(*this);
      return iterator_pre;
    }

    // cppcheck-suppress functionConst
    const json2cppJsonArrayValueIterator &operator--()
    {
      --m_itr;

      return *this;
    }

    // cppcheck-suppress functionStatic
    void advance(std::ptrdiff_t n) { m_itr += n; }

  private:
    json2cpp::json::const_iterator m_itr;
  };


  /**
   * @brief   Class for iterating over the members belonging to a JSON object.
   *
   * This class provides a JSON object iterator that dereferences as an instance
   * of json2cppJsonObjectMember representing one of the members of the object. It
   * has been implemented using the boost iterator_facade template.
   *
   * @see json2cppJsonObject
   * @see json2cppJsonObjectMember
   */
  class json2cppJsonObjectMemberIterator
  {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using value_type = json2cppJsonObjectMember;
    using difference_type = json2cppJsonObjectMember;
    using pointer = json2cppJsonObjectMember *;
    using reference = json2cppJsonObjectMember &;

    /**
     * @brief   Construct an iterator from a json2cppJson iterator.
     *
     * @param   itr  json2cppJson iterator to store
     */
    explicit json2cppJsonObjectMemberIterator(const json2cpp::json::const_iterator &itr) : m_itr(itr) {}

    /**
     * @brief   Returns a json2cppJsonObjectMember that contains the key and value
     *          belonging to the object member identified by the iterator.
     */
    json2cppJsonObjectMember operator*() const { return json2cppJsonObjectMember(m_itr.key(), m_itr.value()); }

    DerefProxy<json2cppJsonObjectMember> operator->() const { return DerefProxy<json2cppJsonObjectMember>(**this); }

    /**
     * @brief   Compare this iterator with another iterator.
     *
     * Note that this directly compares the iterators, not the underlying
     * values, and assumes that two identical iterators will point to the same
     * underlying object.
     *
     * @param   other  Iterator to compare with
     *
     * @returns true if the underlying iterators are equal, false otherwise
     */
    bool operator==(const json2cppJsonObjectMemberIterator &other) const { return m_itr == other.m_itr; }

    bool operator!=(const json2cppJsonObjectMemberIterator &other) const { return !(m_itr == other.m_itr); }

    // cppcheck-suppress functionConst
    const json2cppJsonObjectMemberIterator &operator++()
    {
      ++m_itr;

      return *this;
    }

    // cppcheck-suppress functionConst
    json2cppJsonObjectMemberIterator operator++(int)
    {
      json2cppJsonObjectMemberIterator iterator_pre(m_itr);
      ++(*this);
      return iterator_pre;
    }

    // cppcheck-suppress functionConst
    const json2cppJsonObjectMemberIterator &operator--()
    {
      --m_itr;

      return *this;
    }

  private:
    /// Iternal copy of the original json2cppJson iterator
    json2cpp::json::const_iterator m_itr;
  };

  /// Specialisation of the AdapterTraits template struct for json2cppJsonAdapter.
  template<> struct AdapterTraits<valijson::adapters::json2cppJsonAdapter>
  {
    typedef json2cpp::json DocumentType;

    static std::string adapterName() { return "json2cppJsonAdapter"; }
  };

  inline bool json2cppJsonFrozenValue::equalTo(const Adapter &other, bool strict) const
  {
    return json2cppJsonAdapter(m_value).equalTo(other, strict);
  }

  inline json2cppJsonArrayValueIterator json2cppJsonArray::begin() const
  {
    return json2cppJsonArrayValueIterator{ m_value.begin() };
  }

  inline json2cppJsonArrayValueIterator json2cppJsonArray::end() const
  {
    return json2cppJsonArrayValueIterator{ m_value.end() };
  }

  inline json2cppJsonObjectMemberIterator json2cppJsonObject::begin() const
  {
    return json2cppJsonObjectMemberIterator{ m_value.begin() };
  }

  inline json2cppJsonObjectMemberIterator json2cppJsonObject::end() const
  {
    return json2cppJsonObjectMemberIterator{ m_value.end() };
  }

  inline json2cppJsonObjectMemberIterator json2cppJsonObject::find(const std::string_view propertyName) const
  {
    return json2cppJsonObjectMemberIterator{ m_value.find(propertyName) };
  }

}// namespace adapters
}// namespace valijson
