#pragma once
/////////////////////////////////////////////////////////////////////
//  NoSqlDb.h - key/value pair in-memory database                  //
//  ver 1.3                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Key/value Database, CSE 687- Sp17			    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu    
//  Source:         Jim Fawcett, CSE687 - Object Oriented Design, Spring 2017
/////////////////////////////////////////////////////////////////////

/*
* Package Operations:
* -------------------
* This package defines two classes that support key/value storage:
* - Element<Data> defines a single value stored in the database.
*   It provides metadata properties:
*   - nam
* - manual information
* - maintenance information
* - query mechanism
* - persistance mechanism
* - TestExecutive that demonstrates the requirements you meet


Public Interface:
=================
Element class:
show:							        //print element on the screen
NoSqlDb class:				
saveRecord:								//add element to database
removeRecord							//delete element from databse
hasKey									//check if key is present
saveValue								//save updated element
addchildren:						    //add dependency
value:				                    //return element of db



Build Process:
==============
Required files
- CppProperties.h; XmlDocument.h; XmlElement.h; Convert.h; StrHelper.h
*  Build Command:
*  --------------
* devenv CodeAnalyzerEx.sln /rebuild debugs

* Maintenance History:
*---------------------
*ver 1.3 : 12 Mar 2017
* - add childs in existing element
* ver 1.2 : 06 Feb 2017
* - added children
* - modified Element<Data>::show()
* - added these comments
* ver 1.1 : 28 Jan 2017
* - fixed bug in NoSqlDb::count() by replacing
*   unordered_map<key,Value>::count() with
*   unordered_map<key,Value>::size();
* Ver 1.0 : 25 Jan 2017
* - first release
*/

/////////////////////////////////////////////////////////////////////
//Include dependency and c++ libraries
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include "../CppProperties/CppProperties.h"
#include "../DateTime/DateTime.h"

/////////////////////////////////////////////////////////////////////
//namespace defination
namespace NoSQLDB
{
	using Key = std::string;
	using Keys = std::vector<Key>;
	using Name = std::string;
	using Category = std::string;
	using Children = std::vector<Key>;

	/////////////////////////////////////////////////////////////////////
	// Element class represents a data record in our NoSql database
	// - in our NoSql database that is just the value in a key/value pair
	// - it needs to store child data, something for you to implement
	//
	template<typename Data>
	class Element
	{
	public:
		Element() {}
		Element(Name aName, Category aCat) : name(aName), category(aCat) {};
		Property<Name> name;                  // metadata
		Property<Category> category;          // metadata
		Property<DateTime> dateTime;          // metadata
		Property<Children> children;          // metadata
		Property<Data> data;                  // data
		void saveChild(Key);
		std::string show();
	};
	/////////////////////////////////////////////////////////////////////
	//save child
	template<typename Data>
	void Element<Data>::saveChild(Key key)
	{
		children.getValue().push_back(key);
	}
	/////////////////////////////////////////////////////////////////////
	//display element
	template<typename Data>
	std::string Element<Data>::show()
	{
		std::ostringstream out;
		out.setf(std::ios::adjustfield, std::ios::left);
		out << "\n    " << std::setw(8) << "name" << " : " << name;
		out << "\n    " << std::setw(8) << "category" << " : " << category;
		out << "\n    " << std::setw(8) << "dateTime" << " : " << DateTime(dateTime).time();
		Children children_ = static_cast<Children>(children);
		if (children_.size() > 0)
		{
			out << "\n    " << std::setw(8) << "children" << " : ";
			for (size_t i = 0; i < children_.size(); ++i)
			{
				out << children_[i];
				if (i < children_.size())
					out << ", ";
			}
		}
		out << "\n    " << std::setw(8) << "data" << " : " << data;
		out << "\n";
		return out.str();
	}

	/////////////////////////////////////////////////////////////////////
	// NoSqlDb class is a key/value pair in-memory database
	// - stores and retrieves elements
	// - you will need to add query capability
	//   That should probably be another class that uses NoSqlDb
	//
	template<typename Data>
	class NoSqlDb
	{
	public:
		using Key = std::string;
		using Keys = std::vector<Key>;
		Keys keys();
		bool hasKey(Key key);
		bool saveRecord(Key key, Element<Data> elem);
		bool saveValue(Key key, Element<Data> elem);
		bool removeRecord(Key key);
		bool addchildren(Key parentkey, Key childkey);
		Element<Data> value(Key key);
		size_t count();
	private:
		using Item = std::pair<Key, Element<Data>>;
		std::unordered_map<Key, Element<Data>> store;
	};
	/////////////////////////////////////////////////////////////////////
	//return list of keyes
	template<typename Data>
	typename NoSqlDb<Data>::Keys NoSqlDb<Data>::keys()
	{
		Keys keys;
		for (Item item : store)
		{
			keys.push_back(item.first);
		}
		return keys;
	}
	/////////////////////////////////////////////////////////////////////
	//check for presence  of key
	template<typename Data>
	bool NoSqlDb<Data>::hasKey(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		return true;
	}
	/////////////////////////////////////////////////////////////////////
	//save new record
	template<typename Data>
	bool NoSqlDb<Data>::saveRecord(Key key, Element<Data> elem)
	{
		if (store.find(key) != store.end())
			return false;
		store[key] = elem;
		return true;
	}
	/////////////////////////////////////////////////////////////////////
	//remove record
	template<typename Data>
	bool NoSqlDb<Data>::removeRecord(Key key)
	{
		if (store.find(key) == store.end())
			return false;
		size_t numErased = store.erase(key);
		if (numErased == 1)
		{
			// remove key from all other element's children collections
			for (Key k : keys())
			{
				Keys& children = store[k].children.getValue();  // note Keys& - we don't want copy of children
				Keys::iterator iter;
				for (iter = children.begin(); iter != children.end(); ++iter)
				{
					if ((*iter) == key)
					{
						children.erase(iter);
						break;
					}
				}
			}
			return true;
		}
		return false;
	}
	/////////////////////////////////////////////////////////////////////
	//update record
	template<typename Data>
	bool NoSqlDb<Data>::saveValue(Key key, Element<Data> elem)
	{
		if (store.find(key) == store.end())
			return false;
		store[key] = elem;
		return true;
	}
	/////////////////////////////////////////////////////////////////////
	//find element
	template<typename Data>
	Element<Data> NoSqlDb<Data>::value(Key key)
	{
		if (store.find(key) != store.end())
			return store[key];
		return Element<Data>();
	}
	/////////////////////////////////////////////////////////////////////
	//Size of Sql
	template<typename Data>
	size_t NoSqlDb<Data>::count()
	{
		return store.size();
	}
	/////////////////////////////////////////////////////////////////////
	//Addd childrens to existing element in NoSql
	template<typename Data>
	bool NoSQLDB::NoSqlDb<Data>::addchildren(Key parentkey, Key childkey)
	{
		bool addchildFlag = true;
		Element<std::string> elem = store[parentkey];
		for (Key k : elem.children.getValue())
		{
			if (k == childkey)
				addchildFlag = false;
		}
		if (addchildFlag)
		{
			Keys& children = store[parentkey].children.getValue();
			children.push_back(childkey);
			addchildFlag = true;
		}
		return addchildFlag;
	}
}
