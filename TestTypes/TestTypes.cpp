// TestTypes.cpp

#include "TestTypes.h"
#include "../Utilities/Utilities.h"
#include <iostream>

#ifdef TEST_SUPERHERO

using namespace NoSQLDB;

int main()
{
  SuperHero sh;
  sh.name = "Batman";
  sh.power = 5;
  std::cout << "\n  " << sh;

  SuperHeroWrapper shw = sh;
  std::string xml = shw.save();
  std::cout << "\n  " << xml;

  SuperHeroWrapper newShw;
  newShw.restore(xml);
  SuperHero newSh = newShw;
  std::cout << "\n\n  " << newSh.name << " : " << newSh.power;
  std::cout << "\n\n";
}
#endif
