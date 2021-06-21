/////////////////////////////////////////////////////////////////////
//  TypeAnal.cpp -   Do type analysis and store in typetable          //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Dependency check, CSE 687- Sp17	    		    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu                 //
/////////////////////////////////////////////////////////////////////

#include "TypeAnal.h"
namespace CodeAnalysis
{
	TypeAnal::TypeAnal() :
		ASTref_(Repository::getInstance()->AST()),
		scopeStack_(Repository::getInstance()->scopeStack())
	{	}                                              
	/////////////////////////////////////////////////////////////////////
	//This constructor is used only for test executive of typeanal package.
	TypeAnal::TypeAnal(AbstrSynTree& ast_ref, ScopeStack<ASTNode*> stack_) : ASTref_(ast_ref),
		scopeStack_(stack_)
	{	}

	/////////////////////////////////////////////////////////////////////
	//select these from ast
	bool doDisplay(ASTNode* pNode)
	{
		static std::string toDisplay[] = {
			"function", "lambda", "class", "struct", "enum", "alias", "typedef"
		};
		
		for (std::string type : toDisplay)
		{
			
			if (pNode->type_ == type)
				return true;
		}
		return false;
	}
	/////////////////////////////////////////////////////////////////////
	//search through ast and populate TT
	void TypeAnal::DFS(ASTNode* pNode)
	{
		static std::stack<std::string> stack;
		bool memberfunc = false;
		static std::string path = "";
		if (pNode->path_ != path)
		{
			//std::cout << "\n    -- " << pNode->path_ << "\\" << pNode->package_;
			path = pNode->path_;
		}
		if (pNode->type_ == "namespace")
		{
			stack.push(pNode->name_);
		}
		if (doDisplay(pNode))
		{
			std::string namespace_name;
			if (stack.size() != 0)
				namespace_name = stack.top();
			else
				namespace_name = "Not found";
			if(pNode->name_ != "main")
				populateTT(pNode, namespace_name);
		}
		for (auto pChild : pNode->children_)
		{   //search only for global functions stop DFS inside class struct or main function.
			if (pNode->type_ == "class" || pNode->type_ == "struct" ||pNode->name_ == "main")
				memberfunc = true;
			if (!memberfunc)
				DFS(pChild);
		}
		if (pNode->type_ == "namespace")
		{
			stack.pop();
		}
	}
	/////////////////////////////////////////////////////////////////////
	//Do type analysis
	void TypeAnal::doTypeAnal()
	{
		std::cout << "\n  starting type analysis:\n";
		std::cout << "\n -----------------------------------------------";
		//std::cout << "\n Files processed : \n";
		ASTNode* pRoot = ASTref_.root();
		DFS(pRoot);
	}
	/////////////////////////////////////////////////////////////////////
	//save data to typetable
	void TypeAnal::populateTT(ASTNode * pNode, std::string namespace_name)
	{
		TTElement value1;
		value1.setFileName(pNode->path_);
		std::string name = pNode->name_;
		value1.setNamespace(namespace_name);
		value1.settype(pNode->type_);
		TT.addElem(name, value1);
	}
	/////////////////////////////////////////////////////////////////////
	//return typetable
	TypeTable TypeAnal::getTT()
	{
		return TT;
	}
}



#ifdef TEST_TYPEANAL
using Utils = Utilities::StringHelper;
using namespace CodeAnalysis;

int main(int argc, char* argv[])
{
	
	try
	{
		Utils::Title("Testing Type Table and Type Analysis");

		ScopeStack<ASTNode*> stack_;
		AbstrSynTree ast(stack_);
		ASTNode* pX = new ASTNode("class", "X");
		ast.add(pX);                                        // add X scope
		ASTNode* pf1 = new ASTNode("function", "f1");
		ast.add(pf1);                                       // add f1 scope
		ASTNode* pc1 = new ASTNode("control", "if");
		ast.add(pc1);                                       // add c1 scope
		ast.pop();                                          // end c1 scope
		ast.pop();                                          // end f1 scope
		ASTNode* pf2 = new ASTNode("function", "f2");
		ast.add(pf2);                                       // add f2 scope
		ast.pop();                                          // end f2 scope
		ast.pop();                                          // end X scope

		std::function<void(ASTNode*, size_t)> co = [](ASTNode* pNode, size_t indentLevel)
		{
			std::cout << "\n  " << std::string(2 * indentLevel, ' ') << pNode->show();
		};

		ASTWalk(ast.root(), co);
		std::cout << "\n";
		TypeAnal ta(ast, stack_) ;
		ta.doTypeAnal();
		ta.displayTT();
	}
	catch (std::exception& except)
	{
		std::cout << "\n\n  caught exception in Executive::main: " + std::string(except.what()) + "\n\n";
		return 1;
	}
}
#endif
