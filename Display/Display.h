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

/*
* Package Operations:
* -------------------
* This package defines Display that shows contents
* of an instance of NoSqlDb<Data> and TypeTable  It accepts a reference to a NoSqlDb<Data>
* database from which it displays records. 

Public Interface:
=================
class: Display

show									//show one element of nosql
showAll									//show all elements of nosql
showHeader								//just the header of nosql
showQuery								//show queries
showDepAnal								//show dependency analysis
showDepKey								//show one element of dep analysis
showSCAnal								//show strong component analysis
showSCKey								//show strong component

class: DisplayType
void showTypeAnal						//show type table
void showTypeKey						//show one element in typetable

Build Process:
==============
Required files
*   Display.h, Display.cpp,
*   NoSqlDb.h, NoSqlDb.cpp, CppProperties.h, CppProperties.cpp, TypeTable
*  Build Command:
*  --------------
* devenv CodeAnalyzerEx.sln /rebuild debug

* Required Files:
* ---------------


Maintenance History:
====================
* ver 1.1
* - Dependency analysis and strong component analysis added.
* ver 1.0 : 06 Feb 2017
* - first release

*/

/////////////////////////////////////////////////////////////////////
//Include dependency and c++ libraries
#include "../NoSqlDb/NoSqlDb.h"
#include "../TestTypes/TestTypes.h"
#include "../Utilities/Utilities.h"
#include "../TypeTable/TypeTable.h"
#include <string>
#include <iostream>

namespace NoSQLDB
{

	/////////////////////////////////////////////////////////////////////
	//Diplay class
  template <typename Data>
  class Display
  {
  public:
    enum Style { summary, detailed };
    Display(NoSqlDb<Data>& db) : db_(db) {}
    void show(Key key, Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showAll(Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showHeader(Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
    void showQuery(Keys keys, Style style = summary, size_t fieldwidth = 10, std::ostream& out = std::cout);
	void showDepAnal(size_t fieldwidth = 100, std::ostream& out = std::cout);
	void showDepKey(Key key,size_t fieldwidth = 100, std::ostream& out = std::cout);
	void showSCAnal(size_t fieldwidth = 100, std::ostream& out = std::cout);
	void showSCKey(Key key, size_t fieldwidth = 100, std::ostream& out = std::cout);
  private:
    void showSummary(Key key, size_t fieldwidth, std::ostream& out);
    void showDetails(Key key, size_t fieldwidth, std::ostream& out);
    NoSqlDb<Data>& db_;
  };
  //----< show single record in one-line format >----------------------

  template<typename Data>
  void Display<Data>::showSummary(Key key, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    Element<Data> elem = db_.value(key);
    int save = out.setf(std::ios::left, std::ios::adjustfield);
    out << "  ";
    out << std::setw(fieldwidth) << key.substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << static_cast<std::string>(elem.name).substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << static_cast<std::string>(elem.category).substr(0, fieldwidth) << " ";
    DateTime dt = elem.dateTime;
    out << std::setw(fieldwidth) << static_cast<std::string>(dt).substr(0, fieldwidth) << " ";
    std::string temp = Utilities::Convert<Data>::toString(elem.data.getValue());
    out << std::setw(fieldwidth) << temp.substr(0, fieldwidth) << " ";
    if (elem.children.getValue().size() > 0)
    {
      for (Key k : elem.children.getValue())
        out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
    }
    out << "\n";
    out.setf(save);
  }
  //----< show single record with detailed, multi-line, format >-------

  template<typename Data>
  void Display<Data>::showDetails(Key key, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    Element<Data> elem = db_.value(key);
    int save = out.setf(std::ios::left, std::ios::adjustfield);
    out << "  ";
    out << "key      : " << key << "\n  ";
    out << "Name     : " << static_cast<std::string>(elem.name) << "\n  ";
    out << "Category : " << static_cast<std::string>(elem.category) << "\n  ";
    DateTime dt = elem.dateTime;
    out << "DateTime : " << static_cast<std::string>(dt) << "\n  ";
    out << "Data     : " << elem.data.getValue() << "\n  ";
    out << "Children :\n    ";
    if (elem.children.getValue().size() > 0)
    {
      for (Key k : elem.children.getValue())
        out << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth) << " ";
    }
    out << "\n";
    out.setf(save);
  }
  //----< show single record >-----------------------------------------
  /*
   * Style: Display<Data>::summary or Display<Data>::detailed
   * fieldwidth: number of characters for each field
   * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::show(Key key, Style style, size_t fieldwidth, std::ostream& out)
  {
    if (!db_.hasKey(key))
      return;
    if (style == Display<Data>::Style::summary)
      showSummary(key, fieldwidth, out);
    else
      showDetails(key, fieldwidth, out);
  }
  //----< show all records >-------------------------------------------
  /*
  * Style: Display<Data>::summary or Display<Data>::detailed
  * fieldwidth: number of characters for each field
  * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::showAll(Style style, size_t fieldwidth, std::ostream& out)
  {
    Keys keys = db_.keys();
    if (style == Display<Data>::Style::summary)
    {
      showHeader(style, fieldwidth, out);
      for (Key k : keys)
        showSummary(k, fieldwidth, out);
    }
    else
    {
      for (Key k : keys)
        showDetails(k, fieldwidth, out);
    }
  }
  //----< show all records in query >----------------------------------
  /*
  * Style: Display<Data>::summary or Display<Data>::detailed
  * fieldwidth: number of characters for each field
  * out: stream for output - typically std::cout (default) or std::ostringstream
  */
  template<typename Data>
  void Display<Data>::showQuery(Keys keys, Style style, size_t fieldwidth, std::ostream& out)
  {
    if (style == Display<Data>::Style::summary)
    {
      showHeader(style, fieldwidth, out);
      for (Key k : keys)
        showSummary(k, fieldwidth, out);
    }
    else
    {
      for (Key k : keys)
        showDetails(k, fieldwidth, out);
    }
  }
  //----< shows underlined names of each field for summary output >----

  template<typename Data>
  void Display<Data>::showHeader(Style style, size_t fieldwidth, std::ostream& out)
  {
    size_t numChildren = 0;
    Keys keys = db_.keys();
    for (Key k : keys)
    {
      size_t size = db_.value(k).children.getValue().size();
      if (numChildren < size)
        numChildren = size;
    }
    int save = out.setf(std::ios::left, std::ios::adjustfield);
    out << "  ";
    out << std::setw(fieldwidth) << std::string("Key").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Name").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Category").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("TimeDate").substr(0, fieldwidth) << " ";
    out << std::setw(fieldwidth) << std::string("Data").substr(0, fieldwidth) << " ";
    if (numChildren > 0)
    {
      for (size_t i = 0; i < numChildren; ++i)
        out << std::setw(fieldwidth) << std::string("Child").substr(0, fieldwidth) << " ";
    }
    out << "\n  ";
    for (size_t i = 0; i < numChildren + 5; ++i)
      out << std::setw(fieldwidth) << std::string("----------").substr(0, fieldwidth) << " ";
    out << "\n";
    out.setf(save);
  }
  /////////////////////////////////////////////////////////////////////
  //show one element of dependency analysis
  template<typename Data>
  void Display<Data>::showDepKey(Key key, size_t fieldwidth, std::ostream & out)
  {
	  if (!db_.hasKey(key))
		  return;
	  Element<Data> elem = db_.value(key);
	  int save = out.setf(std::ios::left, std::ios::adjustfield);
	  out << "  ";
	  out << "\n  File : " << key << "\n  ";
	  out << "Depends on :\n";
	  if (elem.children.getValue().size() > 0)
	  {
		  for (Key k : elem.children.getValue())
			  out << "\t"<<std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth*4) << "\n ";
	  }
	  out << "\n";
	  out.setf(save);
  }
  /////////////////////////////////////////////////////////////////////
  //show one componenet of strong component analysis
  template<typename Data>
  void Display<Data>::showSCKey(Key key, size_t fieldwidth, std::ostream & out)
  {
	  if (!db_.hasKey(key))
		  return;
	  Element<Data> elem = db_.value(key);
	  int save = out.setf(std::ios::left, std::ios::adjustfield);
	  out << "  ";
	  out << "\n  Strong Component : " << key << "\n  ";
	  out << "Circular dependency between :\n";
	  if (elem.children.getValue().size() > 0)
	  {
		  for (Key k : elem.children.getValue())
			  out << "\t" << std::setw(fieldwidth) << static_cast<std::string>(k).substr(0, fieldwidth * 4) << "\n ";
	  }
	  out << "\n";
	  out.setf(save);
  }
  /////////////////////////////////////////////////////////////////////
  //show strong component analysis and biggest strong component 
  template<typename Data>
  void Display<Data>::showSCAnal(size_t fieldwidth, std::ostream & out)
  {
	  size_t biggestSC = 0;
	  Keys keys = db_.keys();
	  if (keys.size() > 0)
	  {
		  Key bigSC = keys[0];
		  for (Key k : keys)
		  {
			  showSCKey(k, fieldwidth, out);
			  Element<Data> elem = db_.value(k);
			  if (elem.children.getValue().size() > biggestSC)
			  {
				  biggestSC = elem.children.getValue().size();
				  bigSC = k;
			  }
		  }
		  out << "\n  The biggest Strong Component is : " << "\n  ";
		  showSCKey(bigSC, fieldwidth, out);
	  }
  }
  /////////////////////////////////////////////////////////////////////
  //show dependency analysis
  template<typename Data>
  void Display<Data>::showDepAnal(size_t fieldwidth, std::ostream & out)
  {
	  Keys keys = db_.keys();
	  for (Key k : keys)
		  showDepKey(k, fieldwidth, out);
  }
}


class DisplayType
{
public:
	DisplayType(TypeTable & tt) : tt_(tt) {}
	void showTypeAnal(size_t fieldwidth=15, std::ostream& out = std::cout);
	void showTypeKey(std::string key, size_t fieldwidth = 10, std::ostream& out = std::cout);
private:
	//std::unordered_map<std::string, std::vector<TTElement>>& tt_;
	TypeTable tt_;
};
