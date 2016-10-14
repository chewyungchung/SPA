#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "Relation.h"

using namespace std;

class RelationTable {

public:
	RelationTable();
	bool IsValidRelation(string relation);
	bool IsArg1Valid(string relation, string arg_type);
	bool IsArg2Valid(string relation, string arg_type);
	bool IsCorrectNumArg(string relation, int num_of_arg);
private:
	unordered_map<string, Relation> relation_table_;
	
};