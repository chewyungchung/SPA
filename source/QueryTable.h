#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "QueryResult.h"
#include "Clause.h"
#include "map"
#include "unordered_set"

using namespace std;

class QueryTable {
public:

	QueryTable();
	~QueryTable();
	QueryTable(int isNull);
	QueryTable(Clause selectClause, Clause suchThatClause, Clause patternClause);

	// Get
	Clause getSelectClause();
	Clause getSuchThatClause();
	Clause getPatternClause();
	QueryResult getSelectResult();
	QueryResult getSuchThatResult();
	QueryResult getPatternResult();

	int isSelectResultEmpty();
	int isSuchThatResultEmpty();
	int isPatternResultEmpty();
	int isNullQuery();

	string getSynType(string);

	// Set
	void setSelectClause(Clause selectClause);
	void setSuchThatClause(Clause suchThatClause);
	void setPatternClause(Clause patternClause);
	void setSelectResult(QueryResult qr);
	void setSuchThatResult(QueryResult qr);
	void setPatternResult(QueryResult qr);
	void setIsSuchThatAvail(int val);
	void setIsPatternAvail(int val);

private:
	
	map<string, string> _synEntityMap; // MAP<SYN, ENTITY> // Remove
	
	Clause _selectClause;
	Clause _suchThatClause;
	Clause _patternClause;
	QueryResult _selectResult;
	QueryResult _suchThatResult;
	QueryResult _patternResult;

	int _isNull;
};