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

unordered_map<int, list<string>> ModUsesTablebyStmt::getModTable()
{
	return modifiesTable;
}

unordered_map<int, list<string>> ModUsesTablebyStmt::getUsesTable()
{
	return usesTable;
}

void ModUsesTablebyStmt::addModifies(int stmtNum, string var)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);
	list<string> mVarList;

	/* if key is not found, add key-pair into modifiesTable */
	if (it == modifiesTable.end()) {
		mVarList.push_back(var);
		modifiesTable.insert(pair<int, list<string>>(stmtNum, mVarList));
	}
	else {
		mVarList = it->second;
		mVarList.push_back(var);
	}
}

void ModUsesTablebyStmt::addUses(int stmtNum, string var)
{
	unordered_map<int, list<string>>::iterator it = usesTable.find(stmtNum);
	list<string> uVarList;

	/* if key is not found, add key-value pair into usesTable */
	if (it == usesTable.end()) {
		uVarList.push_back(var);
		usesTable.insert(pair<int, list<string>>(stmtNum, uVarList));
	}
	else {
		uVarList = it->second;
		uVarList.push_back(var);
	}
}

bool ModUsesTablebyStmt::isValidStmt(int stmtNum)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);

	if (it != modifiesTable.end()) {
		return true;
	}
	else {
		return false;
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

list<string> ModUsesTablebyStmt::getModifiedBy(int stmtNum)
{
	unordered_map<int, list<string>>::iterator it = modifiesTable.find(stmtNum);

	if (it != modifiesTable.end()) {
		return modifiesTable.at(stmtNum);
	}
	else {
		return list<string>();
	}
}

list<string> ModUsesTablebyStmt::getUsedBy(int stmtNum)
{
	unordered_map<int, list<string>>::iterator it = usesTable.find(stmtNum);

	if (it != usesTable.end()) {
		return usesTable.at(stmtNum);
	}
	else {
		return list<string>();
	}
}

