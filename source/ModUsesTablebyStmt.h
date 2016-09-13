#pragma once

#include <list>
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
	unordered_map<int, list<string>> getModTable();
	unordered_map<int, list<string>> getUsesTable();
	void addModifies(int stmtNum, string var);
	void addUses(int stmtNum, string var);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<string> getModifiedBy(int stmtNum);
	list<string> getUsedBy(int stmtNum);
};



