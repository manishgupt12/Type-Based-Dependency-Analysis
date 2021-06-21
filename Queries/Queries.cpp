/////////////////////////////////////////////////////////////////////
// Queries.cpp - retrieve NoSqlDb contents                         //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_QUERIES

#include "Queries.h"
#include "../NoSqlDb/NoSqlDb.h"
#include "../Display/Display.h"
#include <iostream>
#include <string>

using namespace NoSQLDB;

int main()
{
  using StrData = std::string;

  std::cout << "\n  Demonstrating NoSql Queries";
  std::cout << "\n =============================\n";

  std::cout << "\n  Creating and saving NoSqlDb elements with string data";
  std::cout << "\n -------------------------------------------------------\n";

  NoSqlDb<StrData> db;

  Element<StrData> elem1;
  elem1.name = "elem1";
  elem1.category = "test";
  elem1.data = "elem1's StrData";

  db.saveRecord(elem1.name, elem1);

  Element<StrData> elem2;
  elem2.name = "elem2";
  elem2.category = "test";
  elem2.data = "elem2's StrData";

  db.saveRecord(elem2.name, elem2);

  Element<StrData> elem3;
  elem3.name = "elem3";
  elem3.category = "test";
  elem3.data = "elem3's StrData";

  db.saveRecord(elem3.name, elem3);

  elem1.saveChild(elem2.name);
  elem1.saveChild(elem3.name);
  elem2.saveChild(elem3.name);
  db.saveValue(elem1.name, elem1);
  db.saveValue(elem2.name, elem2);

  std::cout << "\n  Displaying elements from NoSqlDb<string>";
  std::cout << "\n ------------------------------------------\n";

  Display<StrData> display(db);
  display.showAll();

  std::cout << "\n  Testing Queries";
  std::cout << "\n -----------------";

  Query<StrData> query(db);

  Query<StrData>::NoQueryType debugNoQuery = [](Keys keys)
  {
    std::cout << "\n  debugNoQuery:";
    std::cout << "\n  " << "Hello Query World!";
  };
  query.executeNoQuery(debugNoQuery, db.keys());
  std::cout << "\n";

  Query<StrData>::NoQueryType names = [&db](Keys keys)
  {
    std::cout << "\n  names NoQuery:";
    std::cout << "\n  names in db: ";
    for (Key k : keys)
    {
      std::cout << db.value(k).name << " ";
    }
  };
  query.executeNoQuery(names, db.keys());
  std::cout << "\n";

  Query<StrData>::QueryTypeStr getElem = [&db](const Query<StrData>::Arg& arg, Keys keys)
  {
    Keys returnKeys;
    std::cout << "\n  getElem Query";
    for (Key k : keys)
    {
      if ((db.value(k)).name.getValue() == arg)
        returnKeys.push_back(arg);
    }
    return returnKeys;
  };

  Keys qkeys = query.execute(getElem, "elem3", db.keys());
  std::cout << "\n  size of returned keyset = " << qkeys.size();
  std::cout << "\n  returned keys are: ";
  for (Key k : qkeys)
    std::cout << k << " ";
  std::cout << "\n";
  display.showQuery(qkeys);

  Query<StrData>::QueryType hasChildren = [&db](Keys keys)
  {
    Keys returnKeys;
    std::cout << "\n  hasChildren query:";
    for (Key k : keys)
    {
      if ((db.value(k)).children.getValue().size() > 0)
        returnKeys.push_back(k);
    }
    return returnKeys;
  };

  qkeys = query.execute(hasChildren, db.keys());
  std::cout << "\n  size of returned keyset = " << qkeys.size();
  std::cout << "\n  returned keys are: ";
  for (Key k : qkeys)
    std::cout << k << " ";
  std::cout << "\n";
  display.showQuery(qkeys);

  std::cout << "\n  Testing Compound Query - getting elem1 AND elem2 - second query will be empty";
  std::cout << "\n -------------------------------------------------------------------------------";

  qkeys = query.execute(getElem, "elem1", qkeys);
  std::cout << "\n  size of returned keyset = " << qkeys.size();
  std::cout << "\n  returned keys are: ";
  for (Key k : qkeys)
    std::cout << k << " ";
  std::cout << "\n";
  display.showQuery(qkeys);

  qkeys = query.execute(getElem, "elem2", qkeys);
  std::cout << "\n  size of returned keyset = " << qkeys.size();
  std::cout << "\n  returned keys are: ";
  for (Key k : qkeys)
    std::cout << k << " ";
  std::cout << "\n";
  display.showQuery(qkeys);

  TestQueries<std::string> tq(std::cout);
  tq.doQueries(db);
  std::cout << "\n\n";
}
#endif
