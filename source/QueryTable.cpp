#include "QueryTable.h"

/*
* 
*/

QueryTable::QueryTable()
{

}
Clause QueryTable::getSelectClause()
{
	return _selectClause;
}

Clause QueryTable::getSuchThatClause()
{
	return _suchThatClause;
}

Clause QueryTable::getPatternClause()
{
	return _patternClause;
}

string QueryTable::getSynType(string syn)
{
	if (_synEntityMap.find(syn) != _synEntityMap.end()) {
		return _synEntityMap.at(syn);
	}

	// Debug
	return "NO HAVE";
}

bool QueryTable::isSuchThatAvail() {
	return _isSuchThatAvail;
}

bool QueryTable::isPatternAvail() {
	return _isPatternAvail;
}