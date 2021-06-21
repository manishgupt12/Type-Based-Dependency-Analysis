#pragma once
/////////////////////////////////////////////////////////////////////
//  StrongComp.h -    Strong component analysis for code analser      //
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
This package containes a class for strong component analysis. It store the outout in NoSql data base. Where the childrens
represents the files which are circular dependents of each other

Public Interface:
=================
class: StrongComp

strcomanal();					//strong component analysis
tarjanalgo();					//Tarjan algorithm
SCCUtil						    //helper for tarjan algorithm
getStrongComp                  //return strong componant analsis


Build Process:
==============
Required files
- NoSqlDb.h; Graph.h,
*  Build Command:
*  --------------
* devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0

*/

/////////////////////////////////////////////////////////////////////
//Include dependency and c++ libraries

#include <iostream>
#include <functional>
#include <string>
#include <stack>
#include <algorithm>
#include "../NoSqlDb/NoSqlDb.h"
#include "../Graph/Graph.h"

/////////////////////////////////////////////////////////////////////
//Declare class StrongComp

#pragma warning (disable : 4101)  // disable warning re unused variable x, below
using namespace NoSQLDB;
using namespace GraphLib;
namespace CodeAnalysis
{
	typedef Graph<std::string, std::string> graph;
	class StrongComp
	{
	public:
		StrongComp(NoSqlDb<std::string> db);
		void strcomanal();
		void tarjanalgo();
		void SCCUtil(int u, int disc[], int low[], std::stack<int> *st, bool stackMember[]);
		NoSqlDb<std::string> getStrongComp();
	private:
		NoSqlDb<std::string> db_depanal;
		NoSqlDb<std::string> StrComAnal;
		std::list<int> *adj;
		graph g;
	};
}