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
	Clause getSelectClause();
	Clause getSuchThatClause();
	Clause getPatternClause();
	string getSynType(string);
	bool isSuchThatAvail;
	bool isPatternAvail;

private:
	
	map<string, unordered_set<Clause>> hashMap; // MAP<SYN, ENTITY>
	string selectClause;
	string suchThatClause;
	string patternClause;
};