#pragma once
/////////////////////////////////////////////////////////////////////
//  DepAnal.h -    Dependency analysis for code analser            //
//  ver 1.1                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Dependency check, CSE 687- Sp17	    		    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu 
// Source: Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017//
/////////////////////////////////////////////////////////////////////

#include "Display.h"


/////////////////////////////////////////////////////////////////////
//Include Typetable display
void DisplayType::showTypeAnal(size_t fieldwidth, std::ostream & out)
{
	std::vector<std::string> keys = tt_.Keys();
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	out << " \n";
	out << "  ";
	out << std::setw(fieldwidth) << std::string("Name").substr(0, fieldwidth) << " ";
	out << std::setw(fieldwidth) << std::string("Type Name").substr(0, fieldwidth) << " ";
	out << std::setw(fieldwidth) << std::string("Name Space").substr(0, fieldwidth) << " ";
	out << std::setw(fieldwidth) << std::string("File Path").substr(0, 10*fieldwidth ) << " ";
	out << "\n\n";
	out.setf(save);
	
	for (std::string k : keys)
		showTypeKey(k, fieldwidth, out);
}

/////////////////////////////////////////////////////////////////////
//Typetable element display
void DisplayType::showTypeKey(std::string key, size_t fieldwidth, std::ostream & out)
{
	if (!tt_.hasKey(key))
		return;
	std::vector<TTElement> elems = tt_.value(key);
	int save = out.setf(std::ios::left, std::ios::adjustfield);
	//out << " \n";
	out << "  ";
	out << std::setw(fieldwidth) << key.substr(0, fieldwidth) << " ";
	for (size_t i = 0; i < elems.size(); i++)
	{
		std::string typename_ = elems[i].gettype();
		std::string namespace_ = elems[i].getnamespace();
		std::string filename_ = elems[i].getfilename();
		out << std::setw(fieldwidth) << static_cast<std::string>(typename_).substr(0, fieldwidth) << " ";
		out << std::setw(fieldwidth) << static_cast<std::string>(namespace_).substr(0, fieldwidth) << " ";
		out << std::setw(fieldwidth) << static_cast<std::string>(filename_).substr(0, 10* fieldwidth) << " ";
		if(i!=elems.size()-1)
			out << std::setw(fieldwidth) << "\n" << "    ";
	}
	out << "\n";
	out.setf(save);
}


/////////////////////////////////////////////////////////////////////
//Test
#ifdef TEST_DISPLAY

using namespace NoSQLDB;
using StrData = std::string;
using intData = int;

int main()
{
  std::cout << "\n  Demonstrating Display Package";
  std::cout << "\n ===============================\n";

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
  Display<StrData> display(db);
  display.showHeader();
  display.show(elem1.name);
  std::cout << "\n";
}
#endif

