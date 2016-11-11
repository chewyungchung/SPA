#pragma once

using namespace std;

#include "PKB.h"

class DesignExtractor
{
public:
	DesignExtractor();
	DesignExtractor(PKB& pkb, int num_procs);
	~DesignExtractor();

	void BuildCallsGraph(int vertex_count);
	void UpdateAllProcModUses();
	void UpdateAllCallStmtModUses();

private:
	PKB* pkb_;
	int vertex_count_;

	void RevDFSAndUpdate(int v);
	void UpdateParentProc(int parent, int child);
	void AddAllModVar(int parent, int child);
	void AddAllUsedVar(int parent, int child);
};