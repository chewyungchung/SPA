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
	QueryTable(bool isNull);
	QueryTable(Clause selectClause, Clause suchThatClause, Clause patternClause);

	// Get
	Clause getSelectClause();
	Clause getSuchThatClause();
	Clause getPatternClause();
	QueryResult getSelectResult();
	QueryResult getSuchThatResult();
	QueryResult getPatternResult();

	bool isSuchThatAvail();
	bool isPatternAvail();
	bool isSelectResultEmpty();
	bool isSuchThatResultEmpty();
	bool isPatternResultEmpty();
	bool isNullQuery();

	string getSynType(string);

	// Set
	void setSelectClause(Clause selectClause);
	void setSuchThatClause(Clause suchThatClause);
	void setPatternClause(Clause patternClause);
	void setSelectResult(QueryResult qr);
	void setSuchThatResult(QueryResult qr);
	void setPatternResult(QueryResult qr);
	void setIsSuchThatAvail(bool val);
	void setIsPatternAvail(bool val);

private:
	
	map<string, string> _synEntityMap; // MAP<SYN, ENTITY> // Remove
	
	Clause _selectClause;
	Clause _suchThatClause;
	Clause _patternClause;
	QueryResult _selectResult;
	QueryResult _suchThatResult;
	QueryResult _patternResult;

	bool _isSuchThatAvail;
	bool _isPatternAvail;
	bool _isNull;
};