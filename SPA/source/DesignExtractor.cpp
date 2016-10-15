#pragma once

#include "DesignExtractor.h"

DesignExtractor::DesignExtractor()
{
}

DesignExtractor::DesignExtractor(PKB pkb, int numProcs)
{
	_pkb = pkb;
	vertexCount = numProcs;
}

DesignExtractor::~DesignExtractor()
{
}

void DesignExtractor::buildCallsGraph(int vertexCount)
{
	_pkb.initializeCallsGraph(vertexCount); // set nxn matrix all 0s
	for (int i = 0; i < vertexCount; i++)
	{
		list<string> callees = _pkb.getCallee(_pkb.getProcName(i));
		list<string>::iterator it = callees.begin();
		for (; it != callees.end(); ++it)
		{
			_pkb.addCallsGraphEdge(i, _pkb.getProcIndex(*it));
		}
	}
}

// Pre-cond: Graph must be acyclic, otherwise the update traversal will be infinite
// Essentially a reverse DFS from all 'leaf' nodes
// Observe that anytime we move up the chain, the previous node data must be carried up
void DesignExtractor::updateAllProcModUses()
{
	for (int i = 0; i < vertexCount; i++)
	{
		bool isLeaf = true;
		for (int j = 0; j < vertexCount; j++)
		{
			if (_pkb.hasArcCallsGraph(i, j))
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

void DesignExtractor::revDFSAndUpdate(int v)
{
	//revDFSVisited[v] = true;
	for (int i = 0; i < vertexCount; i++)
	{
		if (_pkb.hasArcCallsGraph(i, v))
		{
			updateParentProc(i, v);
			revDFSAndUpdate(i);
		}
	}
}

void DesignExtractor::updateParentProc(int parent, int child)
{
	addAllModVar(parent, child);
	addAllUsedVar(parent, child);
}

void DesignExtractor::addAllModVar(int parent, int child)
{
	string s_parent = _pkb.getProcName(parent);
	string s_child = _pkb.getProcName(child);

	list<string> modVarChild = _pkb.getModifiedByProc(s_child);
	list<string>::iterator it;
	for (it = modVarChild.begin(); it != modVarChild.end(); ++it)
	{
		_pkb.addProcMod(s_parent, *it);
	}
}

void DesignExtractor::addAllUsedVar(int parent, int child)
{
	string s_parent = _pkb.getProcName(parent);
	string s_child = _pkb.getProcName(child);

	list<string> usesVarChild = _pkb.getUsedByProc(s_child);
	list<string>::iterator it;
	for (it = usesVarChild.begin(); it != usesVarChild.end(); ++it)
	{
		_pkb.addProcUses(s_parent, *it);
	}
}