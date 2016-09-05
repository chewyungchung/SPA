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

QueryResult::QueryResult(vector<string> synList) {
	_synonymList = synList;
}

vector<string> QueryResult::getSynonymList() {
	return _synonymList;
}

vector<int> QueryResult::getResultList() {
	return _resultList;
}

void QueryResult::setResultList(vector<int> resultList) {
	_resultList = resultList;
}

void QueryResult::insertSynonym(string syn) {
	_synonymList.push_back(syn);
}

void QueryResult::insertResult(int item) {
	_resultList.push_back(item);
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

bool QueryResult::isResultEmpty() {
	return _resultList.empty();
}