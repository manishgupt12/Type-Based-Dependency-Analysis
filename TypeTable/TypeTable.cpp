/////////////////////////////////////////////////////////////////////
//  TypeTable.cpp -  Create typetable for dependency checks           //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Dependency check, CSE 687- Sp17	    		    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu                 //
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
//Include library and dependenc
#include "TypeTable.h"
#include <ostream>
#include <sstream> 
#include <iomanip>
/////////////////////////////////////////////////////////////////////
//Set name
void TTElement::settype(std::string type)
{
	Type = type;
}
/////////////////////////////////////////////////////////////////////
//Set namespace
void TTElement::setNamespace(std::string Namespace)
{
	NameSpace = Namespace;
}
/////////////////////////////////////////////////////////////////////
//set filename
void TTElement::setFileName(std::string filename)
{
	FileName = filename;
}
/////////////////////////////////////////////////////////////////////
//return type
std::string TTElement::gettype()
{
	return Type;
}
/////////////////////////////////////////////////////////////////////
//return namespace
std::string TTElement::getnamespace()
{
	return NameSpace;
}
/////////////////////////////////////////////////////////////////////
//return filename
std::string TTElement::getfilename()
{
	return FileName;
}
/////////////////////////////////////////////////////////////////////
//Display element
std::string TTElement::show()
{
	std::ostringstream out;
	out.setf(std::ios::adjustfield, std::ios::left);
	out << "\n    " << std::setw(8) << "Type" << " : " << Type;
	out << "\n    " << std::setw(8) << "Namespace" << " : " << NameSpace;
	out << "\n    " << std::setw(8) << "FileName" << " : " << FileName;
	out << "\n";
	return out.str();
}

/////////////////////////////////////////////////////////////////////
//TypeTabel
TypeTable::TypeTable() 
{
}
/////////////////////////////////////////////////////////////////////
//List of keys
std::vector<std::string> TypeTable::Keys()
{
	std::vector<std::string> keys;
	for (Item item : store)
	{
		keys.push_back(item.first);
	}
	return keys;
}
/////////////////////////////////////////////////////////////////////
//Add elems to tt
bool TypeTable::addElem(std::string key, TTElement elem)
{
	if (store.find(key) != store.end())
	{
		std::vector<TTElement> t = store[key];
		t.push_back(elem);
		store[key] = t;
		return true;
	}
	std::vector<TTElement> t;
	t.push_back(elem);
	store[key] = t;
	return true;
}
/////////////////////////////////////////////////////////////////////
//return value vector for element
std::vector<TTElement> TypeTable::value(std::string key)
{
	if (store.find(key) != store.end()) {
		
		return store[key];
	}
	else 
	return std::vector<TTElement>();
}

/////////////////////////////////////////////////////////////////////
//If key is present
bool TypeTable::hasKey(std::string key)
{
	if (store.find(key) != store.end()) {

		return true;
	}
	else
		return false;
}

/////////////////////////////////////////////////////////////////////
//Test stub
#ifdef TYPETABLE

int main() {
	std::cout << "testing typetable " << std::endl;
	TypeTable test;
	TTElement t1;
	t1.setFileName("test1.xml");
	t1.setNamespace("namespace1");
	t1.settype("class1");
	std::string key = "test1";
	test.addElem(key, t1);

	TTElement t2;
	t2.setFileName("test2.xml");
	t2.setNamespace("namespace2");
	t2.settype("class2");
	key = "test2";
	test.addElem(key, t2);

	test.addElem(key, t1);

	std::cout << "printing test typetable" << std::endl;
	std::vector<std::string> keys = test.Keys();
	for (size_t t = 0; t < keys.size(); t++)
	{
		std::vector<TTElement> elems = test.value(keys[t]);
		std::cout << "\n"<< keys[t] << std::endl;
		for (size_t i = 0; i < elems.size(); i++)
		{
			std::cout << elems[i].show();
		}
	}
}
#endif // TYPETABLE
