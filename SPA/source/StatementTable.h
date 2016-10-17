#pragma once

#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class StatementTable
{
private:
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
	list<int> ifList;
	list<int> callList;
	unordered_map<int, string> callProcList; // key: callstmt#, value: associated procName
	unordered_map<int, string> ctrlvarList;

	public:
	StatementTable();
	~StatementTable();
	void addStatement(int stmtNum, string stmtType);
	void addStatement(int stmtNum, string stmtType, string ctrlvar);
	list<int> getAssignList();
	list<int> getWhileList();
	list<int> getIfList();
	list<int> getCallList();
	string getProcNameByCallStmt(int callStmt);
	list<int> getIfListWithControlVariable(string ctrlvar);
	list<int> getWhileListWithControlVariable(string ctrlvar);
	list<int> getStmtList();
	int getStatementCount();
	bool isValidStmt(int stmtNum);

private:
};
