#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "Relation.h"

using namespace std;

class RelationTable {

public:
	RelationTable();
	bool isValidRelation(string relation);
	bool isArg1Valid(string relation, string argType);
	bool isArg2Valid(string relation, string argType);
	bool isCorrectNumArg(string relation, int numOfArg);
private:
	unordered_map<string, Relation> _relationTable;
	
};