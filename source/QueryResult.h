#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class QueryResult {

public:
	QueryResult();
	QueryResult(vector<string> _result);
	vector<string> getSynonymList();
	vector<int> getResultList();
	void setResultList(vector<int> resultList);
	void setIsExist(bool input);
	bool getIsExist();
	void insertSynonym(string synonym);
	void insertResult(int item);
	bool isQueryExistential();
	bool isSynonymExist(string syn);
	bool isResultEmpty();

private:
	vector<string> _synonymList;
	vector<int> _resultList;
	bool _isExist;

};