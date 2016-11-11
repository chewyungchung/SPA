#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class StatementTable
{
private:
	list<int> assign_list_;
	list<int> while_list_;
	map<int, string> all_stmt_list_;
	list<int> if_list_;
	list<int> call_list_;
	map<int, string> call_proc_list_; // key: callstmt#, value: associated procName
	map<int, string> ctrl_var_list_;

	public:
	StatementTable();
	~StatementTable();
	void AddStatement(int stmt_num, string stmt_type);
	void AddStatement(int stmt_num, string stmt_type, string ctrl_var);
	list<int> GetAssignList();
	list<int> GetWhileList();
	list<int> GetIfList();
	list<int> GetCallList();
	string GetProcNameByCallStmt(int call_stmt);
	list<int> GetIfListWithControlVariable(string ctrl_var);
	list<int> GetWhileListWithControlVariable(string ctrl_var);
	string GetControlVarWithStmt(int stmt_num);
	list<int> GetStmtList();
	int GetStatementCount();
	bool IsValidStmt(int stmt_num);
	string GetStmtType(int stmt_num);
	bool IsAssign(int stmt_num);
};
