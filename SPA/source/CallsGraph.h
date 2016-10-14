#pragma once

#include "PKB.h"

using namespace std;

class CallsGraph
{
public:
	CallsGraph(int vertexCount, PKB pkb);
	~CallsGraph();
	bool isCallsGraphCyclic();
	void updateAllProcModUses();

private:
	PKB pkb;
	int** adjM;
	bool* revDFSVisited;
	int vertexCount;
	bool isCyclicHelper(int v, bool visited[], bool* recStack);
	void revDFSAndUpdate(int v);
	void updateParentProc(int parent, int child);
	void addAllModVar(int parent, int child);
	void addAllUsedVar(int parent, int child);
};
