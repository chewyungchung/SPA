#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class ModUsesTablebyStmt
{
public:
	ModUsesTablebyStmt();
	~ModUsesTablebyStmt();
	map<int, list<string>> getModTable();
	map<int, list<string>> getUsesTable();
	void addModifies(int stmtNum, string var);
	void addUses(int stmtNum, string var);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<string> getModifiedBy(int stmtNum);
	list<string> getUsedBy(int stmtNum);

private:
	map<int, list<string>> modifiesTable;
	map<int, list<string>> usesTable;
};



