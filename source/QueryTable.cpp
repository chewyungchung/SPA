#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{

}

// Overloaded Constructor
QueryTable::QueryTable(Clause selectClause, Clause suchThatClause, Clause patternClause) {
	*_selectClause = selectClause;
	*_suchThatClause = suchThatClause;
	*_patternClause = patternClause;
}

Clause QueryTable::getSelectClause()
{
	return *_selectClause;
}

QueryResult QueryTable::getSelectResult()
{
	return *_selectResult;
}

Clause QueryTable::getSuchThatClause()
{
	return *_suchThatClause;
}

QueryResult QueryTable::getSuchThatResult()
{
	return *_suchThatResult;
}

Clause QueryTable::getPatternClause()
{
	return *_patternClause;
}

QueryResult QueryTable::getPatternResult()
{
	return *_patternResult;
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

bool QueryTable::isSelectResultEmpty()
{
	if (!(_selectResult->isArg1ResultEmpty()) && !(_selectResult->isArg2ResultEmpty())) {
		return true;
	}
	else {
		return false;
	}

}

bool QueryTable::isSuchThatResultEmpty()
{
	if (!_suchThatResult->isArg1ResultEmpty() && !_suchThatResult->isArg2ResultEmpty()) {
		return true;
	}
	else {
		return false;
	}
}

bool QueryTable::isPatternResultEmpty()
{
	if (!_patternResult->isArg1ResultEmpty() && !_patternResult->isArg2ResultEmpty()) {
		return true;
	}
	else {
		return false;
	}
}

void QueryTable::setSelectClause(Clause selectClause)
{
	*_selectClause = selectClause;
}

void QueryTable::setSuchThatClause(Clause suchThatClause)
{
	*_suchThatClause = suchThatClause;
}

void QueryTable::setPatternClause(Clause patternClause)
{
	*_patternClause = patternClause;
}

void QueryTable::setSelectResult(QueryResult selectResult)
{
	*_selectResult = selectResult;
}

void QueryTable::setSuchThatResult(QueryResult suchThatResult)
{
	*_suchThatResult = suchThatResult;
}

void QueryTable::setPatternResult(QueryResult patternResult)
{
	*_patternResult = patternResult;
}

void QueryTable::setIsSuchThatAvail(bool val) 
{
	_isSuchThatAvail = val;
}

void QueryTable::setIsPatternAvail(bool val) 
{
	_isPatternAvail = val;
}