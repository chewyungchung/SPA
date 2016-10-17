#pragma once

using namespace std;

#include "PKB.h"

class DesignExtractor
{
public:
	DesignExtractor();
	DesignExtractor(PKB pkb, int numProcs);
	~DesignExtractor();

	void buildCallsGraph(int vertexCount);
	void updateAllProcModUses();
	void updateAllCallStmtModUses();

private:
	PKB _pkb;
	int vertexCount;

	void revDFSAndUpdate(int v);
	void updateParentProc(int parent, int child);
	void addAllModVar(int parent, int child);
	void addAllUsedVar(int parent, int child);
};