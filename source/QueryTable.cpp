#include "QueryTable.h"

// Default
QueryTable::QueryTable()
{
	/*_selectClause = NULL;
	_suchThatClause = NULL;
	_patternClause = NULL;
	_selectResult = NULL;
	_suchThatResult = NULL;
	_patternResult = NULL;*/
}

QueryTable::QueryTable(bool isNull) {
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
	// Free all pointers
	/*delete _selectClause;
	delete _suchThatClause;
	delete _patternClause;
	delete _selectResult;
	delete _suchThatResult;
	delete _patternResult;*/
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

bool QueryTable::isSuchThatAvail() {
	/*if (_suchThatClause != NULL) {
		return true;
	}
	else {
		return false;
	}*/
	return true;
}

bool QueryTable::isPatternAvail() {
	/*if (_patternClause != NULL) {
		return true;
	}
	else {
		return false;
	}*/
	return true;
}

bool QueryTable::isSelectResultEmpty() {
	/*if (_selectResult != NULL) {
		return true;
	}
	else {
		return false;
	}*/
	/*
	if (!(_selectResult->isArg1ResultEmpty()) && !(_selectResult->isArg2ResultEmpty())) {
		return true;
	}
	else {
		return false;
	}
	*/
	return true;
}

bool QueryTable::isSuchThatResultEmpty()
{
	/*if (_suchThatResult != NULL) {
		return true;
	}
	else {
		return false;
	}*/
	/*
	if (!_suchThatResult->isArg1ResultEmpty() && !_suchThatResult->isArg2ResultEmpty()) {
		return true;
	}
	else {
		return false;
	}
	*/
	return true;
}

bool QueryTable::isPatternResultEmpty()
{
	/*if (_patternResult != NULL) {
		return true;
	}
	else {
		return false;
	}*/
	/*
	if (!_patternResult->isArg1ResultEmpty() && !_patternResult->isArg2ResultEmpty()) {
		return true;
	}
	else {
		return false;
	}
	*/
	return true;
}

bool QueryTable::isNullQuery() {
	return _isNull;
}

void QueryTable::setSelectClause(Clause selectClause)
{
	_selectClause = selectClause;
	_isNull = false;
}

void QueryTable::setSuchThatClause(Clause suchThatClause)
{
	_suchThatClause = suchThatClause;
	_isNull = false;
}

void QueryTable::setPatternClause(Clause patternClause)
{
	_patternClause = patternClause;
	_isNull = false;
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

void QueryTable::setIsSuchThatAvail(bool val) 
{
	_isSuchThatAvail = val;
}

void QueryTable::setIsPatternAvail(bool val) 
{
	_isPatternAvail = val;
}