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
	map<int, list<string>> GetModTable();
	map<int, list<string>> GetUsesTable();
	void AddModifies(int stmt_num, string var);
	void AddUses(int stmt_num, string var);
	bool IsModified(int stmt_num, string var_name);
	bool IsUsed(int stmt_num, string var_name);
	list<string> GetModifiedBy(int stmt_num);
	list<string> GetUsedBy(int stmt_num);

private:
	map<int, list<string>> modifies_table_;
	map<int, list<string>> uses_table_;
};



