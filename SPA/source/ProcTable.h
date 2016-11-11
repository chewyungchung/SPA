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

	void AddProc(string proc_name);
	void AddProcMod(string proc_name, string var);
	void AddProcUses(string proc_name, string var);
	void AddProcCalledInStmt(string proc_name, int stmtLine);

	string GetProcName(int proc_index);
	int GetProcIndex(string proc_name);

	bool IsModifiedByProc(string proc_name, string var_name);
	bool IsUsedByProc(string proc_name, string var_name);
	list<string> GetModifiedByProc(string proc_name);
	list<string> GetUsedByProc(string proc_name);
	list<string> GetProcedureModifying(string var_name);
	list<string> GetProcedureUsing(string var_name);

	bool IsProcedureExist(string proc_name);
	list<int> GetCallByProcName(string proc_name);
	list<string> GetCalledProcNamesList();
	list<string> GetProcedureList();

private:
	int PROCDATA_COLSIZE = 4;
	int PROCNAME_COL = 0;
	int MODVAR_COL = 1;
	int USEVAR_COL = 2;
	int CALLED_IN_STMT_COL = 3;

	map<string, int> proc_name_index_table_; //key: procName; value: internal index (0, 1, 2, ..)
	vector<vector<list<string>>> proc_data_table_; // for every procIndex: col_1: procName; col_2: modified var, col_3: used var, col_4: called in stmtLine
	int max_proc_index_;
};