#pragma once
/////////////////////////////////////////////////////////////////////
//  TypeAnal.h -   Do type analysis and store in typetable          //
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
This package containes a class which operarates on files to give typetable.
This stores values in TypeTable format.

Public Interface:
=================
class TypeAnal

populateTT						//save values to typetable
doTypeAnal						//do type analysis
getTT							//return type table
DFS								//search in abstract syntex tree

Build Process:
==============
Required files
- Parser.h, TypeTable.h, Display.h, AbstrSynTree.h, ActionAndRules.h, ConfigureParser.h"
*  Build Command:
*  --------------
* devenv CodeAnalyzerEx.sln /rebuild debug

Maintenance History:
====================
ver 1.0

*/
/////////////////////////////////////////////////////////////////////
//Include dependency and c++ libraries

#include "../Parser/ActionsAndRules.h"
#include <iostream>
#include <functional>
#include <stack>
#include "../TypeTable/TypeTable.h"
#include "../Display/Display.h"
#include "../Parser/ConfigureParser.h"
#include "../AbstractSyntaxTree/AbstrSynTree.h"

/////////////////////////////////////////////////////////////////////
//class TypeAnal
namespace CodeAnalysis
{
	class TypeAnal
	{
	public:
		using SPtr = std::shared_ptr<ASTNode*>;
		void populateTT(ASTNode* pNode, std::string namespace_name);
		TypeAnal();
		TypeAnal(AbstrSynTree&, ScopeStack<ASTNode*> stack_);
		void doTypeAnal();
		TypeTable getTT();
		void DFS(ASTNode* pNode);
	private:
		AbstrSynTree& ASTref_;
		ScopeStack<ASTNode*> scopeStack_;
		TypeTable TT;
	};
}