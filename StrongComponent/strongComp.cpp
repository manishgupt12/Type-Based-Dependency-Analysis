#pragma once
/////////////////////////////////////////////////////////////////////
//  StrongComp.cpp -    Strong component analysis for code analser      //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2015, SP1                            //
//  Platform:      HP EliteBook, Windows 10                        //
//  Application:   Dependency check, CSE 687- Sp17	    		    //
//  Author:        Manish Gupta, Syracuse University               //
//                 (315) 412-8140, magupta@syr.edu                 //
/////////////////////////////////////////////////////////////////////
#include "strongComp.h"
#include <stack>
#include <list>

using namespace GraphLib;
using namespace NoSQLDB;
typedef Graph<std::string, std::string> graph;
typedef Vertex<std::string, std::string> vertex;
typedef Display<std::string, std::string> display;
namespace CodeAnalysis
{
	using Key = std::string;
	using Keys = std::vector<std::string>;
	CodeAnalysis::StrongComp::StrongComp(NoSqlDb<std::string> db): db_depanal(db)
	{
	}


	/////////////////////////////////////////////////////////////////////
	//Strong  component and making graph
	void StrongComp::strcomanal()
	{
		for (Key k : db_depanal.keys())  //add vertex
		{
			vertex v(k);
			g.addVertex(v);
		}
		adj = new std::list<int>[g.size()];
		graph::iterator iter = g.begin();
		while (iter != g.end())
		{
			vertex v = *iter;
			Element<std::string> elem = db_depanal.value(v.value());
			Keys childrenlist = elem.children.getValue();
			//add edges
			if (elem.children.getValue().size() > 0)
			{
				graph::iterator iterchild = g.begin();
				while (iterchild != g.end())
				{
					vertex vchild = *iterchild;
					if (std::find(childrenlist.begin(), childrenlist.end(), vchild.value()) != childrenlist.end())
					{
						g.addEdge(" ", v, vchild);
						adj[static_cast<int>(v.id())].push_back(static_cast<int>(vchild.id()));
					}
					iterchild++;
				}
			}
			iter++;
		}
		tarjanalgo(); //call tarjan algo
	}

	/////////////////////////////////////////////////////////////////////
	//Tarjan algo for strong component
	void StrongComp::tarjanalgo()
	{
		int V = static_cast<int>(g.size());
		int *disc = new int[V];
		int *low = new int[V];
		bool *stackMember = new bool[V];
		std::stack<int> *st = new std::stack<int>();
		// Initialize disc and low, and stackMember arrays
		for (int i = 0; i < V; i++)
		{
			disc[i] = -1;
			low[i] = -1;
			stackMember[i] = false;
		}
		// Call the recursive helper function to find strongly
		// connected components in DFS tree with vertex 'i'
		for (int i = 0; i < V; i++)
			if (disc[i] == -1)
				SCCUtil(i, disc, low, st, stackMember);
	}
	/////////////////////////////////////////////////////////////////////
	//helper function for Strong  component analysis
	void StrongComp::SCCUtil(int u, int disc[], int low[], std::stack<int>* st, bool stackMember[])
	{
		static int time = 0;
		// Initialize discovery time and low value
		disc[u] = low[u] = ++time;
		st->push(u);
		stackMember[u] = true;
		// Go through all vertices adjacent to this
		std::list<int>::iterator i;
		for (i = adj[u].begin(); i != adj[u].end(); ++i)
		{
			int v = *i;  // v is current adjacent of 'u' 
			if (disc[v] == -1) // If v is not visited yet, then recur for it
			{
				SCCUtil(v, disc, low, st, stackMember);
				low[u] = std::min(low[u], low[v]);
			}
			else if (stackMember[v] == true)
				low[u] = std::min(low[u], disc[v]);
		}
		int w = 0;  // To store stack extracted vertices StrComAnal
		static int strongcomponant = 1;
		std::string strComp = "Strong Componant: " + std::to_string(strongcomponant);
		Element<std::string> elem;
		if (low[u] == disc[u])
		{
			while (st->top() != u)
			{
				w = (int)st->top();
				elem.saveChild(g[w].value());
				stackMember[w] = false;
				st->pop();
			}
			w = (int)st->top();
			elem.saveChild(g[w].value());
			stackMember[w] = false;
			st->pop();
		}
		if (elem.children.getValue().size() != 0)
		{
			StrComAnal.saveRecord(strComp, elem);
			strongcomponant++;
		}
	}
	/////////////////////////////////////////////////////////////////////
	//return strong component
	NoSqlDb<std::string> StrongComp::getStrongComp()
	{
		return StrComAnal;
	}

}

/////////////////////////////////////////////////////////////////////
//test strong component
#ifdef TEST_STRONGCOMP

using namespace CodeAnalysis;
int main()
{

	NoSqlDb<std::string> db;

	Element<std::string> elem1;
	elem1.name = "elem1";
	elem1.category = "test";
	elem1.data = "elem1's StrData";

	db.saveRecord(elem1.name, elem1);

	StrongComp sc(db);
	sc.strcomanal();

}
#endif
