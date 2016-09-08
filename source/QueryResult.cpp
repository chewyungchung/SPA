#include "QueryResult.h"
#include <string>
#include <vector>

using namespace std;

// Default constructor
QueryResult::QueryResult() {
	_isExist = false;
	// Empty synonym list
	// Empty result list
}

vector<string> QueryResult::getSynonymList() {
	return _synonymList;
}

vector<string> QueryResult::getArg1ResultList() {
	return _arg1ResultList;
}

vector<string> QueryResult::getArg2ResultList() {
	return _arg2ResultList;
}

vector<string> QueryResult::getPatternResultList() {
	return _patternResultList;
}

void QueryResult::setArg1ResultList(vector<string> resultList) {
	_arg1ResultList = resultList;
}

void QueryResult::setArg2ResultList(vector<string> resultList) {
	_arg2ResultList = resultList;
}

void QueryResult::setPatternResultList(vector<string> resultList) {
	_patternResultList = resultList;
}

void QueryResult::insertSynonym(string syn) {
	_synonymList.push_back(syn);
}

void QueryResult::insertArg1Result(string item) {
	_arg1ResultList.push_back(item);
}

void QueryResult::insertArg2Result(string item) {
	_arg2ResultList.push_back(item);
}

void QueryResult::insertPatternResult(string item) {
	_patternResultList.push_back(item);
}

void QueryResult::setIsExist(bool input) {
	if (input == true) {
		_isExist = input;
	}
	else if (input == false)
	{
		_isExist = input;
	}
}

bool QueryResult::getIsExist() {
	return _isExist;
}

bool QueryResult::isSynonymExist(string syn) {
	for (vector<string>::iterator i = _synonymList.begin(); i != _synonymList.end(); i++) {
		if (find(_synonymList.begin(), _synonymList.end(), syn) != _synonymList.end())
		{
			return true;
		}
		else {
			return false;
		}
	}
}

bool QueryResult::isQueryExistential() {
	return _isExist;
}

bool QueryResult::isArg1ResultEmpty() {
	return _arg1ResultList.empty();
}

bool QueryResult::isArg2ResultEmpty() {
	return _arg2ResultList.empty();
}

bool QueryResult::isPatternResultEmpty() {
	return _patternResultList.empty();
}