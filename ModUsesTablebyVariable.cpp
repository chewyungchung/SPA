#include "ModUsesTablebyVariable.h"

using namespace std;

ModUsesTablebyVariable::ModUsesTablebyVariable() 
{
	varModTable = unordered_map<string, list<int>>();
	varUsesTable = unordered_map<string, list<int>>();
}

ModUsesTablebyVariable::~ModUsesTablebyVariable()
{
}

unordered_map<string, list<int>> ModUsesTablebyVariable::getModTable() 
{
	return varModTable;
}

unordered_map<string, list<int>> ModUsesTablebyVariable::getUsesTable() 
{
	return varUsesTable;
}

void ModUsesTablebyVariable::addModified(string varName, int stmtNum) 
{
	unordered_map<string, list<int>>::iterator it = varModTable.find(varName);
	list<int> mStmtList;

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == varModTable.end()) {
		mStmtList.push_back(stmtNum);
		varModTable.insert(pair<string, list<int>>(varName, mStmtList));
	}
	else {
		mStmtList = it->second;
		mStmtList.push_back(stmtNum);
	}
}

void ModUsesTablebyVariable::addUses(string varName, int stmtNum)
{
	unordered_map<string, list<int>>::iterator it = varUsesTable.find(varName);
	list<int> uStmtList;

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == varUsesTable.end()) {
		uStmtList.push_back(stmtNum);
		varModTable.insert(pair<string, list<int>>(varName, uStmtList));
	}
	else {
		uStmtList = it->second;
	}
	uStmtList.push_back(stmtNum);
}

bool ModUsesTablebyVariable::isValidVar(string varName) 
{
	unordered_map<string, list<int>>::iterator it1 = varUsesTable.find(varName);
	unordered_map<string, list<int>>::iterator it2 = varModTable.find(varName);

	if (it1 != varModTable.end() && it2 != varUsesTable.end) {
		return true;
	}
	else {
		return false;
	}
}

bool ModUsesTablebyVariable::isModified(int stmtNum, string varName) 
{
	unordered_map<string, list<int>>::iterator it = varModTable.find(varName);

	if (it != varModTable.end()) {
		list<int> modifiedVarStmtLst = it->second;
		return find(modifiedVarStmtLst.begin(), modifiedVarStmtLst.end(), varName) != modifiedVarStmtLst.end();
	}
	else {
		return false;
	}
}

bool ModUsesTablebyVariable::isUsed(int stmtNum, string varName) 
{
	unordered_map<string, list<int>>::iterator it = varUsesTable.find(varName);

	if (it != varUsesTable.end()) {
		list<int> usedVarStmtLst = it->second;
		return find(usedVarStmtLst.begin(), usedVarStmtLst.end(), varName) != usedVarStmtLst.end();
	}
	else {
		return false;
	}
}

list<int> ModUsesTablebyVariable::getModifiedBy(string varName) 
{
	unordered_map<string, list<int>>::iterator it = varModTable.find(varName);

	if (it != varModTable.end()) {
		list<int> modStmtList = (it->second.begin, it->second.end);
		return modStmtList;
	}
	else {
		return list<int>();
	}
}

list<int> ModUsesTablebyVariable::getUsedBy(string varName) 
{
	unordered_map<string, list<int>>::iterator it = varUsesTable.find(varName);

	if (it != varUsesTable.end()) {
		list<int> usedStmtList = (it->second.begin, it->second.end);
		return usedStmtList;
	}
	else {
		return list<int>();
	}
}

list<string> ModUsesTablebyVariable::getAllModVar() 
{
	unordered_map<string, list<int>>::iterator it;
	list<string> allModVarList;

	if (varModTable.empty()) {
		return list<string>();
	}

	for (it = varModTable.begin(); it != varModTable.end(); ++it) {
		allModVarList.push_back(it->first);
	}
	return allModVarList;
}

list<string> ModUsesTablebyVariable::getAllUsedVar() 
{
	unordered_map<string, list<int>>::iterator it;
	list<string> allUsedVarList;

	if (varUsesTable.empty()) {
		return list<string>();
	}

	for (it = varUsesTable.begin(); it != varUsesTable.end(); ++it) {
		allUsedVarList.push_back(it->first);
	}
	return allUsedVarList;
}

list<string> ModUsesTablebyVariable::getVarList() 
{
	unordered_map<string, list<int>>::iterator it1;
	unordered_map<string, list<int>>::iterator it2;
	list<string> allVarList;

	for (it1 = varModTable.begin(); it1 != varModTable.end(); ++it1) {
		allVarList.push_back(it1->first);
	}

	for (it2 = varUsesTable.begin(); it2 != varUsesTable.end(); ++it2) {
		allVarList.push_back(it2->first);
	}

	allVarList.unique();

	return allVarList;
}
