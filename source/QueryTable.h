#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Clause.h"
#include "map";
#include "unordered_set";

using namespace std;

class QueryTable {
public:
	QueryTable();
	Clause getSelectClause();
	Clause getSuchThatClause();
	Clause getPatternClause();
	string getSynType(string);
	bool isSuchThatAvail();
	bool isPatternAvail();

private:
	
	map<string, string> _synEntityMap; // MAP<SYN, ENTITY> // Remove
	Clause _selectClause;
	Clause _suchThatClause;
	Clause _patternClause;
	bool _isSuchThatAvail;
	bool _isPatternAvail;
};