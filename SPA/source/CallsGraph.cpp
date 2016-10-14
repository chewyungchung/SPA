/*
Observe here we elect to use adjacency matrix 
because we need to do reverse travel when updating mod/uses for procedures,
adjacency list for this is inefficient
*/
#pragma once

#include "CallsGraph.h"

CallsGraph::CallsGraph(int vertexCount, PKB pkb)
{
	this->vertexCount = vertexCount;
	this->pkb = pkb;
	adjM = new int*[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		adjM[i] = new int[vertexCount];
		for (int j = 0; j < vertexCount; j++)
		{
			adjM[i][j] = 0;
		}
		list<string> callees = pkb.getCallees(pkb.getProcName(i));
		list<string>::iterator it = callees.begin();
		for (; it != callees.end(); ++it)
		{
			adjM[i][pkb.getProcIndex(*it)] = 1;
		}
	}
}

CallsGraph::~CallsGraph()
{
	for (int i = 0; i < vertexCount; i++)
	{
		delete[] adjM[i];
	}
	delete[] adjM;
}

bool CallsGraph::isCallsGraphCyclic()
{
	bool *visited = new bool[vertexCount];
	bool *recStack = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		visited[i] = false;
		recStack[i] = false;
	}

	for (int i = 0; i < vertexCount; i++)
	{
		if (isCyclicHelper(i, visited, recStack))
		{
			return true;
		}
	}
	return false;
}

bool CallsGraph::isCyclicHelper(int v, bool visited[], bool* recStack)
{
	if (visited[v] == false)
	{
		visited[v] = true;
		recStack[v] = true;

		// Recur for all the vertices adjacent to this vertex
		for (int j = 0; j < vertexCount; j++)
		{
			if (adjM[v][j] == 1)
			{
				if (!visited[j] && isCyclicHelper(j, visited, recStack))
				{
					return true;
				}
				else if (recStack[j])
				{
					return true;
				}
			}
		}
	}
	recStack[v] = false;
	return false;
}

// Pre-cond: Graph must be acyclic, otherwise the update traversal will be infinite
// Essentially a reverse DFS from all 'leaf' nodes
// Observe that anytime we move up the chain, the previous node data must be carried up
void CallsGraph::updateAllProcModUses()
{	
	//revDFSVisited = new bool[vertexCount];
	for (int i = 0; i < vertexCount; i++)
	{
		bool isLeaf = true;
		for (int j = 0; j < vertexCount; j++)
		{
			if (adjM[i][j] == 1)
			{
				isLeaf = false;
			}
		}
		if (isLeaf)
		{
			//for (int j = 0; j < vertexCount; j++)
			//{
			//	revDFSVisited[j] = false;
			//}
			revDFSAndUpdate(i);
		}
	}
}

void CallsGraph::revDFSAndUpdate(int v)
{
	//revDFSVisited[v] = true;
	for (int i = 0; i < vertexCount; i++)
	{
		if (adjM[i][v] == 1)
		{
			updateParentProc(i, v);
			revDFSAndUpdate(i);
		}
	}
}

void CallsGraph::updateParentProc(int parent, int child)
{
	addAllModVar(parent, child);
	addAllUsedVar(parent, child);
}

void CallsGraph::addAllModVar(int parent, int child)
{
	string s_parent = pkb.getProcName(parent);
	string s_child = pkb.getProcName(child);

	list<string> modVarChild = pkb.getModifiedByProc(s_child);
	list<string>::iterator it;
	for (it = modVarChild.begin(); it != modVarChild.end(); ++it)
	{
		pkb.addProcMod(s_parent, *it);
	}
}

void CallsGraph::addAllUsedVar(int parent, int child)
{
	string s_parent = pkb.getProcName(parent);
	string s_child = pkb.getProcName(child);

	list<string> usesVarChild = pkb.getUsedByProc(s_child);
	list<string>::iterator it;
	for (it = usesVarChild.begin(); it != usesVarChild.end(); ++it)
	{
		pkb.addProcUses(s_parent, *it);
	}
}