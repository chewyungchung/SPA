#pragma once

#include <list>
#include <string>

using namespace std;

class StatementTable
{
private:
	list<int> assignList;
	list<int> whileList;
	list<int> allStmtList;
	list<int> ifList;

public:
	StatementTable();
	~StatementTable();
	void addStatement(int stmtNum, string stmtType);
	list<int> getAssignList();
	list<int> getWhileList();
	list<int> getIfList();
	list<int> getStmtList();
	int getStatementCount();
	bool isValidStmt(int stmtNum);
};

