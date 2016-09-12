#include "RelationTable.h"
#include <vector>
#include <string>

using namespace std;

RelationTable::RelationTable() {
	// Init _relationTable
	vector<string> arg1Type;
	vector<string> arg2Type;

	// Follow & Follow*
	arg1Type = { "stmt", "assign", "while", "constant", "_", "prog_line" };
	arg2Type = { "stmt", "assign", "while", "constant", "_", "prog_line" };
	Relation follow(2, arg1Type, arg2Type);
	_relationTable["follows"] = follow;
	_relationTable["follows*"] = follow;
	arg1Type.clear();
	arg2Type.clear();

	// Parent & Parent*
	arg1Type = { "stmt", "prog_line", "while", "constant", "_" };
	arg2Type = { "stmt", "assign", "prog_line", "while", "constant", "_" };
	Relation parent(2, arg1Type, arg2Type);
	_relationTable["parent"] = parent;
	_relationTable["parent*"] = parent;
	arg1Type.clear();
	arg2Type.clear();

	// Modifies - Iteration 1 got '_'
	arg1Type = { "stmt", "assign", "while", "prog_line", "string", "constant", "_" };
	arg2Type = { "variable", "string", "_" };
	Relation modifies(2, arg1Type, arg2Type);
	_relationTable["modifies"] = modifies;
	arg1Type.clear();
	arg2Type.clear();

	// Uses - Iteration 1 got '_'
	arg1Type = { "stmt", "assign", "while", "prog_line", "string", "constant", "_" };
	arg2Type = { "variable", "string", "_" };
	Relation uses(2, arg1Type, arg2Type);
	_relationTable["uses"] = uses;
	arg1Type.clear();
	arg2Type.clear();

	// Pattern for assign
	arg1Type = { "variable", "string", "_" };
	arg2Type = { "string", "_" };
	Relation patternAssign(2, arg1Type, arg2Type);
	_relationTable["patternAssign"] = patternAssign;
	arg1Type.clear();
	arg2Type.clear();
}

bool RelationTable::isValidRelation(string relation) {
	unordered_map<string, Relation>::const_iterator it = _relationTable.find(relation);
	if (it == _relationTable.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool RelationTable::isArg1Valid(string relation, string argType) {
	// Pre-requisite: Relation exist in _relationTable
	Relation rel = _relationTable.find(relation)->second;
	vector<string> arg1Type = rel.getArg1Type();
	for (vector<string>::iterator it = arg1Type.begin(); it != arg1Type.end(); it++) {
		if (argType == *it) {
			return true;
		}
	}
	return false;
}

bool RelationTable::isArg2Valid(string relation, string argType) {
	// Pre-requisite: Relation exist in _relationTable
	Relation rel = _relationTable.find(relation)->second;
	vector<string> arg2Type = rel.getArg2Type();
	for (vector<string>::iterator it = arg2Type.begin(); it != arg2Type.end(); it++) {
		if (argType == *it) {
			return true;
		}
	}
	return false;
}

bool RelationTable::isCorrectNumArg(string relation, int numOfArg) {
	// Pre-requisite: Relation exist in _relationTable
	Relation rel = _relationTable.find(relation)->second;
	return (rel.getNumOfArg() == numOfArg);
}