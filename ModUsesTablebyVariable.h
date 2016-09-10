#pragma once

#include <list>
#include <string>
#include <unordered_map>

using namespace std;

class ModUsesTablebyVariable
{
private:
	unordered_map<string, list<int>> varModTable;
	unordered_map<string, list<int>> varUsesTable;

public:
	ModUsesTablebyVariable();
	~ModUsesTablebyVariable();
	unordered_map<string, list<int>> ModUsesTablebyVariable::getModTable();
	unordered_map<string, list<int>> ModUsesTablebyVariable::getUsesTable();
	void addModified(string var, int stmtNum);
	void addUses(string var, int stmtNum);
	bool isValidVar(string varName);
	bool isModified(int stmtNum, string varName);
	bool isUsed(int stmtNum, string varName);
	list<int> getModifiedBy(string varName);
	list<int> getUsedBy(string varName);
	list<string> getAllModVar();
	list<string> getAllUsedVar();
	list<string> getVarList();
};

