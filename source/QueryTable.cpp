#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{
	_isNull = 1;
}

QueryTable::QueryTable(int isNull) {
	_isNull = isNull;
}

// Overloaded Constructor
QueryTable::QueryTable(Clause selectClause, Clause suchThatClause, Clause patternClause) {
	_selectClause = selectClause;
	_suchThatClause = suchThatClause;
	_patternClause = patternClause;
}

// Destructor
QueryTable::~QueryTable() {
}

Clause QueryTable::getSelectClause()
{
	return _selectClause;
}

QueryResult QueryTable::getSelectResult()
{
	return _selectResult;
}

Clause QueryTable::getSuchThatClause()
{
	return _suchThatClause;
}

QueryResult QueryTable::getSuchThatResult()
{
	return _suchThatResult;
}

Clause QueryTable::getPatternClause()
{
	return _patternClause;
}

QueryResult QueryTable::getPatternResult()
{
	return _patternResult;
}

string QueryTable::getSynType(string syn)
{
	if (_synEntityMap.find(syn) != _synEntityMap.end()) {
		return _synEntityMap.at(syn);
	}

	// Debug
	return "NO HAVE";
}

int QueryTable::isSelectResultEmpty() {
	if (_selectResult.isArg1ResultEmpty() == true && _selectResult.isArg2ResultEmpty() == true) {
		return 1;
	}
	else {
		return -1;
	}
}

int QueryTable::isSuchThatResultEmpty(){
	if (_suchThatResult.isArg1ResultEmpty() == true && _suchThatResult.isArg2ResultEmpty() == true) {
		return 1;
	}
	else {
		return -1;
	}
}

int QueryTable::isPatternResultEmpty(){
	if (_patternResult.isArg1ResultEmpty() == true && _patternResult.isArg2ResultEmpty() == true && _patternResult.isPatternResultEmpty() == true) {
		return 1;
	}
	else {
		return -1;
	}
}

int QueryTable::isNullQuery() {
	return _isNull;
}

void QueryTable::setSelectClause(Clause selectClause)
{
	_selectClause = selectClause;
	_isNull = -1;
}

void QueryTable::setSuchThatClause(Clause suchThatClause)
{
	_suchThatClause = suchThatClause;
	_isNull = -1;
}

void QueryTable::setPatternClause(Clause patternClause)
{
	_patternClause = patternClause;
	_isNull = -1;
}

void QueryTable::setSelectResult(QueryResult selectResult)
{
	_selectResult = selectResult;
}

void QueryTable::setSuchThatResult(QueryResult suchThatResult)
{
	_suchThatResult = suchThatResult;
}

void QueryTable::setPatternResult(QueryResult patternResult)
{
	_patternResult = patternResult;
}