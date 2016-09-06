#pragma once

#include <list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class ModUsesTablebyVariable
{
private:
	unordered_map<string, list<int>> varModifiesTable;
	unordered_map<string, list<int>> varUsesTable;

public:
	ModUsesTablebyVariable();
	~ModUsesTablebyVariable();
	void addModified(string var, list<int> stmtNum);
	void addUses(string var, list<int> stmtNum);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<int> getModifiedBy(string varName);
	list<int> getUsedBy(string varName);
};

