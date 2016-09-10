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

public:
	StatementTable();
	~StatementTable();
	void addStatement(int stmtNum, string stmtType);
	int getWhileCount();
	int getAssignCount();
	int getTotalCount();
};

