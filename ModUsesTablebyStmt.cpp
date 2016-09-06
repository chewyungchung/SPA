#include "ModUsesTablebyStmt.h"

using namespace std;

ModUsesTablebyStmt::ModUsesTablebyStmt()
{
	modifiesTable = unordered_map<int, list<string>>();
	usesTable = unordered_map<int, list<string>>();
}

ModUsesTablebyStmt::~ModUsesTablebyStmt()
{
}

void ModUsesTablebyStmt::addModifies(int stmtNum, list<string> var)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);

	/* if key is not found, add key-pair into modifiesTable */
	if (it == modifiesTable.end()) {
		modifiesTable.insert(pair<int, list<string>>(stmtNum, var));
	}
}

void ModUsesTablebyStmt::addUses(int stmtNum, list<string> var)
{
	unordered_map<int, list<string>>::iterator it = usesTable.find(stmtNum);

	/* if key is not found, add key-value pair into usesTable */
	if (it == usesTable.end) {
		usesTable.insert(pair<int, list<string>>(stmtNum, var));
	}
}

bool ModUsesTablebyStmt::isModified(int stmtNum, string varName)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);

	if (it != modifiesTable.end()) {
		list<string> varModified = it->second;
		return find(varModified.begin(), varModified.end(), varName) != varModified.end();
	}
	else {
		return false;
	}
}

bool ModUsesTablebyStmt::isUsed(int stmtNum, string varName)
{
	unordered_map<int, list<string>>::iterator it = usesTable.find(stmtNum);

	if (it != usesTable.end()) {
		list<string> varUsed = it->second;
		return find(varUsed.begin(), varUsed.end(), varName) != varUsed.end();
	}
	else {
		return false;
	}
}

list<string> ModUsesTablebyStmt::getModified(int stmtNum)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);

	if (it != modifiesTable.end()) {
		//return it->second;
		list<string> modVarList = (it->second.begin, it->second.end);
		return modVarList;
	}
	else {
		return list<string>();
	}
}

list<string> ModUsesTablebyStmt::getUsed(int stmtNum)
{
	unordered_map<int, list<string>>::iterator it = usesTable.find(stmtNum);

	if (it != usesTable.end()) {
		//return it->second;
		list<string> usedVarList = (it->second.begin, it->second.end);
		return usedVarList;
	}
	else {
		return list<string>();
	}
}
