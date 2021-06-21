#pragma once
/////////////////////////////////////////////////////////////////////
//  TypeTable.h -  Create typetable for dependency checks           //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Dependency check, CSE 687- Sp17	    		    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu                 //
/////////////////////////////////////////////////////////////////////

/*
Module Operations:
==================
This package containes class which stores the type table for dependency analysis.
It store the outout in unordered map and stores name, typename, namespace and filepath

Public Interface:
=================
class TTElement

settype								//set type of token
setNamespace						//set namespace
setFileName							//set filename
gettype								//get name
getnamespace						//get namespace
getfilename							//get filename
show								//show element

class TypeTable:
Keys								//list of keys in typetable
addElem								//add element in typetable
hasKey								//if key is present
value								//return vector of elements for specified key

Build Process:
==============
Required files
- Parser.h
*  Build Command:
*  --------------
* devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0

*/
/////////////////////////////////////////////////////////////////////
//Include dependency and c++ libraries
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include "../Parser/Parser.h" 

/////////////////////////////////////////////////////////////////////
//Element class
class TTElement
{
public:
	TTElement() {}
	void settype(std::string);
	void setNamespace(std::string);
	void setFileName(std::string);
	std::string gettype();
	std::string getnamespace();
	std::string getfilename();
	std::string show();
private:
	std::string Type;
	std::string NameSpace;
	std::string FileName;
};

/////////////////////////////////////////////////////////////////////
//TypeTable class
class TypeTable
{
public:
	std::vector<std::string> Keys();
	TypeTable();
	bool addElem(std::string key, TTElement elem);
	bool hasKey(std::string key);
	std::vector<TTElement> value(std::string key);
private:
	using Item = std::pair<std::string, std::vector<TTElement>>;
	std::unordered_map<std::string, std::vector<TTElement>> store;
};
