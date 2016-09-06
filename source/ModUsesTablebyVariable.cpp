#include "ModUsesTablebyVariable.h"

using namespace std;

ModUsesTablebyVariable::ModUsesTablebyVariable()
{
	varModifiesTable = unordered_map<string, list<int>>();
	varUsesTable = unordered_map<string, list<int>>();
}

ModUsesTablebyVariable::~ModUsesTablebyVariable()
{
}

void ModUsesTablebyVariable::addModified(string varName, list<int> stmtLst)
{
	unordered_map<string, list<int>>::iterator it = varModifiesTable.find(varName);

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == varModifiesTable.end()) {
		varModifiesTable.insert(pair<string, list<int>>(varName, stmtLst));
	}
}

void ModUsesTablebyVariable::addUses(string varName, list<int> stmtLst)
{
	unordered_map<string, list<int>>::iterator it = varUsesTable.find(varName);

	/* if key is not found, add key-value pair into modifiesTable */
	if (it == varUsesTable.end()) {
		varUsesTable.insert(pair<string, list<int>>(varName, stmtLst));
	}
	/* else if a key is found, update the existing stmtLst that corresponds to the varName key */
	else {

	}
}

bool ModUsesTablebyVariable::isModified(int stmtNum, string varName)
{
	unordered_map<string, list<int>>::iterator it = varModifiesTable.find(varName);

	if (it != varModifiesTable.end()) {
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
	unordered_map<string, list<int>>::iterator it = varModifiesTable.find(varName);

	if (it != varModifiesTable.end()) {
		//return it->second;
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
		//return it->second;
		list<int> usedStmtList = (it->second.begin, it->second.end);
		return usedStmtList;
	}
	else {
		return list<int>();
	}
}