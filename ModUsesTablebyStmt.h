#pragma once

#include <list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class ModUsesTablebyStmt
{
private:
	unordered_map<int, list<string>> modifiesTable;
	unordered_map<int, list<string>> usesTable;

public:
	ModUsesTablebyStmt();
	~ModUsesTablebyStmt();
	unordered_map<int, list<string>> ModUsesTablebyStmt::getModTable();
	unordered_map<int, list<string>> ModUsesTablebyStmt::getUsesTable();
	void addModifies(int stmtNum, string var);
	void addUses(int stmtNum, string var);
	bool isValidStmt(int stmtNo);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<string> getModified(int stmtNum);
	list<string> getUsed(int stmtNum);
	list<int> getStatementList();
};



