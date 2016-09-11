#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>

using namespace std;

class QueryResult {

public:
	QueryResult();

	// Getter
	vector<string> getSynonymList();
	vector<string> getArg1ResultList();
	vector<string> getArg2ResultList();
	vector<string> getPatternResultList();
	string getSynonym(string arg);
	bool getIsExist();
	bool isQueryExistential();
	bool isSynonymExist(string syn);
	bool isArg1ResultEmpty();
	bool isArg2ResultEmpty();
	bool isPatternResultEmpty();

	// Setter
	void setArgToSynonymMapping(string arg, string synonym);
	void setArg1ResultList(vector<string> resultList);
	void setArg2ResultList(vector<string> resultList);
	void setPatternResultList(vector<string> resultList);
	void setIsExist(bool input);
	void insertSynonym(string synonym);
	void insertArg1Result(string item);
	void insertArg2Result(string item);
	void insertPatternResult(string item);

private:
	vector<string> _synonymList;
	vector<string> _arg1ResultList;
	vector<string> _arg2ResultList;
	vector<string> _patternResultList;
	unordered_map<string, string> _argToSynonymMapping;

	bool _isExist;

};