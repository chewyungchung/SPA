#pragma once

#include <list>
#include <string>
#include <map>

using namespace std;

class ModUsesTablebyVariable
{
public:
	ModUsesTablebyVariable();
	~ModUsesTablebyVariable();
	map<string, list<int>> getModTable();
	map<string, list<int>> getUsesTable();
	void addModifies(string var, int stmtNum);
	void addUses(string var, int stmtNum);
	bool isValidVar(string varName);
	list<int> getModifiedBy(string varName);
	list<int> getUsedBy(string varName);
	list<string> getAllModVar();
	list<string> getAllUsedVar();
	list<string> getVarList();

private:
	map<string, list<int>> varModTable;
	map<string, list<int>> varUsesTable;
};

