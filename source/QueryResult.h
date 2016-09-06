#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class QueryResult {

public:
	QueryResult();

	// Getter
	vector<string> getSynonymList();
	vector<string> getArg1ResultList();
	vector<string> getArg2ResultList();
	bool getIsExist();
	bool isQueryExistential();
	bool isSynonymExist(string syn);
	bool isArg1ResultEmpty();
	bool isArg2ResultEmpty();

	// Setter
	void setArg1ResultList(vector<string> resultList);
	void setArg2ResultList(vector<string> resultList);
	void setIsExist(bool input);
	void insertSynonym(string synonym);
	void insertArg1Result(string item);
	void insertArg2Result(string item);

private:
	vector<string> _synonymList;
	vector<string> _arg1ResultList;
	vector<string> _arg2ResultList;

	bool _isExist;

};