#pragma once

#include <list>
#include <string>
#include <map>
#include <vector>

using namespace std;

class ProcTable
{
public:
	ProcTable();
	~ProcTable();

	void addProc(string procName);
	void addProcMod(string procName, string var);
	void addProcUses(string procName, string var);
	void addProcCalledInStmt(string procName, int stmtLine);

	string getProcName(int procIndex);
	int getProcIndex(string procName);

	bool isModifiedByProc(string procName, string varName);
	bool isUsedByProc(string procName, string varName);
	list<string> getModifiedByProc(string procName);
	list<string> getUsedByProc(string procName);
	list<string> getProcedureModifying(string varName);
	list<string> getProcedureUsing(string varName);

	bool isProcedureExist(string procName);
	list<int> getCallByProcName(string procName);
	list<string> getCalledProcNamesList();
	list<string> getProcedureList();

private:
	int PROCDATA_COLSIZE = 4;
	int PROCNAME_COL = 0;
	int MODVAR_COL = 1;
	int USEVAR_COL = 2;
	int CALLED_IN_STMT_COL = 3;

	map<string, int> procNameIndexTable; //key: procName; value: internal index (0, 1, 2, ..)
	vector<vector<list<string>>> procDataTable; // for every procIndex: col_1: procName; col_2: modified var, col_3: used var, col_4: called in stmtLine
	int maxProcIndex;
};