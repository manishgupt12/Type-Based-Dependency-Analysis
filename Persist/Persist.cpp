/////////////////////////////////////////////////////////////////////
// Persist.cpp - store and retrieve NoSqlDb contents               //
// ver 2.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017       //
/////////////////////////////////////////////////////////////////////

#ifdef TEST_PERSIST

#include "Persist.h"
#include "../Display/Display.h"
#include <string>

using namespace NoSQLDB;

int main()
{
  Utilities::Title("Testing NoSqlDb Persistance");
  Utilities::putLine();

  using Element = Element<std::string>;

  NoSqlDb<std::string> db;
  
  Element test1("test1", "test");
  test1.data = "test1 string";

  Element test2("test2", "test");
  test2.data = "test2 string";
  test2.saveChild("test1");

  Element test3("test3", "test");
  test3.data = "test3 string";
  test3.saveChild("test1");
  test3.saveChild("test2");

  db.saveRecord("test1", test1);
  db.saveRecord("test2", test2);
  db.saveRecord("test3", test3);

  Utilities::subTitle("displaying test db");
  Utilities::putLines(2);

  Display<std::string> display(db);
  display.showAll();

  Persist<std::string> persist(db);
  Persist<std::string>::Xml xml = persist.save();
  Utilities::subTitle("Persisting test db to XML");
  std::cout << "\n" << xml << "\n";

  Utilities::subTitle("Persisting test db to file DbXml.txt");
  if (persist.saveToFile("DbXml.txt"))
    std::cout << "\n  db saved";
  else
    std::cout << "\n  filesave failed";
  Utilities::putLine();

  Utilities::subTitle("creating new db from persisted XML");
  Utilities::putLines(2);

  NoSqlDb<std::string> newDb;
  Persist<std::string> persist2(newDb);
  persist2.restore(xml);

  Display<std::string> display2(newDb);
  display2.showAll();

  Utilities::subTitle("creating newer db from DbXml.txt");
  Utilities::putLines(2);
  NoSqlDb<std::string> newerDb;
  Persist<std::string> persist3(newerDb);
  persist3.restoreFromFile("DbXml.Txt");
  Display<std::string> display3(newerDb);
  display3.showAll();

  std::cout << "\n\n";
}
#endif
