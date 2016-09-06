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
	void addModifies(int stmtNum, list<string> var);
	void addUses(int stmtNum, list<string> var);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<string> getModified(int stmtNum);
	list<string> getUsed(int stmtNum);
};


