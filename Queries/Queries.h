#pragma once
/////////////////////////////////////////////////////////////////////
// Queries.h - retrieve NoSqlDb contents                           //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package defines a single class, Query that provides an execute(...)
* to execute a query callable object.  It also defines types for the callable
* object queries that return keysets and non-queries that do something to
* the database, but don't return keysets.
*
* Required Files:
* ---------------
*   Display.h, Display.cpp,
*   NoSqlDb.h, NoSqlDb.cpp, CppProperties.h, CppProperties.cpp
*
* Maintenance History:
*---------------------
* ver 1.0 : 06 Feb 2017
* - first release
*/

#include "../NoSqlDb/NoSqlDb.h"
#include "../Utilities/Utilities.h"
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <regex>

namespace NoSQLDB
{
  ///////////////////////////////////////////////////////////////////
  // Query class defines four types of "query" functions.
  // - Query functions accept a set of keys and return a different
  //   set of keys, to support compound queries.
  // - Non-query functions act like queries except they don't return
  //   a set of keys.  Their purpose is to make some change to the
  //   database, like adding a record.
  // - Each of these types of functions have a version that accepts
  //   a string, and a version that doesn't.
  //
  // - Query accepts a database reference and uses that to make
  //   all its queries.
  // - I plan to replace the reference with a std::shared_ptr so that
  //   the database can be changed without making a new instance of
  //   Query class.
  //
  template<typename Data>
  class Query
  {
  public:
    
    // The type definitions, below, represent all the I/O the database needs.
    // They will normally be used to build lambdas for querying.
    // Note that a lambda will serve exactly the same purpose as a stored procedure.

    using Arg = std::string;
    using QueryTypeStr = std::function<Keys(const Arg&, const Keys& keys)>;   // queries that take string arguments
    using QueryType = std::function<Keys(const Keys& keys)>;                  // queries that take no string arguments
    using NoQueryTypeStr = std::function<void(const Arg&, const Keys& keys)>; // non-queries that take string arguments
    using NoQueryType = std::function<void(const Keys& keys)>;                // non-queries that take no string arguments

    Query(NoSqlDb<Data>& db) : db_(db) {}

    Keys execute(QueryTypeStr q, const Arg& arg, const Keys& keys);
    Keys execute(QueryType q, const Keys& keys);
    void executeNoQuery(NoQueryTypeStr q, const Arg& arg, const Keys& keys);
    void executeNoQuery(NoQueryType q, const Keys& keys);

    NoSqlDb<Data>& database() { return db_; }
  private:
    NoSqlDb<Data>& db_;
  };
  //----< returns keyset of elements that match query >----------------

  template<typename Data>
  Keys Query<Data>::execute(QueryTypeStr q, const Arg& arg, const Keys& keys)
  {
    return q(arg, keys);
  }
  //----< returns keyset of elements that match query >----------------

  template<typename Data>
  Keys Query<Data>::execute(QueryType q, const Keys& keys)
  {
    return q(keys);
  }
  //----< does something with db elements, doesn't return keyset >-----

  template<typename Data>
  typename void Query<Data>::executeNoQuery(NoQueryTypeStr q, const Arg& arg, const Keys& keys)
  {
    q(arg, keys);
  }
  //----< does something with db elements, doesn't return keyset >-----

  template<typename Data>
  typename void Query<Data>::executeNoQuery(NoQueryType q, const Keys& keys)
  {
    q(keys);
  }

  template<typename Data>
  class TestQueries
  {
  public:
    TestQueries(std::ostream& out) : out_(out) {};
    void doQueries(NoSqlDb<Data>& db);
  private:
    NoSqlDb<Data>* pDb = nullptr;
    std::ostream& out_;
    void TestQuery1(const std::string& arg);
    void TestQuery2(const std::string& arg);
    void TestQuery3(const std::string& arg);
    void TestQuery4(const std::string& arg);
    void TestQuery5(const std::string& arg);
    void TestQuery6(const std::string& arg);
    void TestQuery7(DateTime dt1, DateTime dt2);
  };

  template<typename Data>
  void TestQueries<Data>::doQueries(NoSqlDb<Data>& db)
  {
    pDb = &db;
    Utilities::subTitle("Testing Required Queries");
    Utilities::putLine();
    Display<Data> display(*pDb);
    display.showAll();

    TestQuery1("elem1");
    TestQuery2("elem1");
    std::string regEx = ".*2.*";
    TestQuery3(regEx);
    regEx = ".*el.*";
    TestQuery4(regEx);
    regEx = "test";
    TestQuery5(regEx);
    regEx = ".*(2'|3').*";
    TestQuery6(regEx);
    DateTime dt = DateTime().now();
    DateTime::Duration dur = DateTime::makeDuration(1,0,0,0);
    TestQuery7(dt + dur, dt - dur);
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery1(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getKeyIfExists = [](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    getKeyIfExists Query for \"" << arg << "\"";
      for (NoSQLDB::Key k : keys)
      {
        if (k == arg)
        {
          returnKeys.push_back(static_cast<std::string>(k));
          break;
        }
      }
      return returnKeys;
    };

    std::cout << "\n  results of query #1:";
    Keys keys = pDb->keys();
    keys = getKeyIfExists(arg, keys);
    if (keys.size() == 1)
      std::cout << "\n    found key \"" << arg << "\"";
    else
      std::cout << "\n    didn't find key \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery2(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getChildren = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    getChildren Query for \"" << arg << "\"";
      for (NoSQLDB::Key k : keys)
      {
        if (k == arg)
        {
          returnKeys = pDb->value(k).children;
          break;
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #2:";
    Keys keys = pDb->keys();
    keys = getChildren(arg, keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    found children : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find children for \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery3(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getRegexKeyMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    key pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        if (std::regex_match(k, regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #3:";
    Keys keys = pDb->keys();
    keys = getRegexKeyMatch(arg, keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    matches : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find match for \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery4(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getRegexNameMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    name pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        if (std::regex_match(static_cast<std::string>(elem.name), regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #4:";
    Keys keys = pDb->keys();
    keys = getRegexNameMatch(arg, keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    matches : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find match for \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery5(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getRegexCategoryMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    category pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        if (std::regex_match(static_cast<std::string>(elem.category), regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #5:";
    Keys keys = pDb->keys();
    keys = getRegexCategoryMatch(arg, keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    matches : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find match for \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery6(const std::string& arg)
  {
    Query<Data>::QueryTypeStr getRegexDataMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      std::cout << "\n    data pattern match Query for \"" << arg << "\"";
      std::regex regx(arg);
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        std::string data = elem.data;
        if (std::regex_match(data, regx))
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #6:";
    Keys keys = pDb->keys();
    keys = getRegexDataMatch(arg, keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    matches : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find match for \"" << arg << "\"";
  }

  template<typename Data>
  void TestQueries<Data>::TestQuery7(DateTime dt1, DateTime dt2)
  {
    Query<Data>::QueryTypeStr getDateIntervalMatch = [&](const std::string& arg, Keys keys)
    {
      NoSQLDB::Keys returnKeys;
      Element<Data> elem1 = pDb->value(keys[0]);
      DateTime dt;
      elem1.dateTime = (dt + DateTime::makeDuration(2,0,0,0));
      pDb->saveValue(keys[0], elem1);
      std::cout << "\n    set dateTime of " << keys[0] << " to " << static_cast<DateTime>(elem1.dateTime).time();

      std::cout << "\n    date interval match Query for dates between:";
      std::cout << "\n    " << dt1.time() << " and " << dt2.time();
      
      for (NoSQLDB::Key k : keys)
      {
        Element<Data> elem = pDb->value(k);
        DateTime dt = elem.dateTime;
        std::cout << "\n    key : " << k << " has dateTime : " << dt.time();
        if (dt2 < dt && dt < dt1)
        {
          returnKeys.push_back(k);
        }
      }
      return returnKeys;
    };

    std::cout << "\n\n  results of query #6:";
    Keys keys = pDb->keys();
    keys = getDateIntervalMatch("", keys);
    if (keys.size() > 0)
    {
      std::cout << "\n    matches : ";
      for (Key k : keys)
        std::cout << k << " ";
    }
    else
      std::cout << "\n    didn't find match for DateTime interval";
  }
}
