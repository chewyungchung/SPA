#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class StatementTable
{
private:
	list<int> assignList;
	list<int> whileList;
	map<int, string> allStmtList;
	list<int> ifList;
	list<int> callList;
	map<int, string> callProcList; // key: callstmt#, value: associated procName
	map<int, string> ctrlvarList;

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
	string getControlVarWithStmt(int stmtNum);
	list<int> getStmtList();
	int getStatementCount();
	bool isValidStmt(int stmtNum);
	string getStmtType(int stmtNum);

private:
};
