#include "QueryResult.h"
#include <string>
#include <vector>

using namespace std;

const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";
const string PARAM_EMPTY_STRING = "";

// Default constructor
QueryResult::QueryResult() {
	_isExist = false;
	_isNull = false;
	_argToSynonymMapping[PARAM_ARG1] = PARAM_EMPTY_STRING;
	_argToSynonymMapping[PARAM_ARG2] = PARAM_EMPTY_STRING;
	_argToSynonymMapping[PARAM_PATTERN] = PARAM_EMPTY_STRING;
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

string QueryResult::getSynonym(string arg) {
	if (arg == PARAM_ARG1 || arg == PARAM_ARG2 || arg == PARAM_PATTERN) {
		return _argToSynonymMapping[arg];
	}
	else {
		return "wrong input";
	}
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

void QueryResult::setArgToSynonymMapping(string arg, string synonym) {
	if (arg == PARAM_ARG1 || arg == PARAM_ARG2 || arg == PARAM_PATTERN) {
		_argToSynonymMapping[arg] = synonym;
	}
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
	bool isSynonymExist = false;
	for (vector<string>::iterator i = _synonymList.begin(); i != _synonymList.end(); i++) {
		if (find(_synonymList.begin(), _synonymList.end(), syn) != _synonymList.end())
		{
			isSynonymExist = true;;
		}
	}
	return isSynonymExist;
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

bool QueryResult::isResultNull() {
	return _isNull;
}