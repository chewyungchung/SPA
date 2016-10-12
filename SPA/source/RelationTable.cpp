#include "RelationTable.h"

RelationTable::RelationTable() {
	// Init _relationTable
	vector<string> arg1Type;
	vector<string> arg2Type;

	// Follow & Follow*
	arg1Type = { "stmt", "assign", "while", "if", "calls", "constant", "_", "prog_line" };
	arg2Type = { "stmt", "assign", "while", "if", "calls", "constant", "_", "prog_line" };
	Relation follow(2, arg1Type, arg2Type);
	_relationTable["follows"] = follow;
	_relationTable["follows*"] = follow;
	arg1Type.clear();
	arg2Type.clear();

	// Parent & Parent*
	arg1Type = { "stmt", "prog_line", "while", "if", "constant", "_" };
	arg2Type = { "stmt", "assign", "while", "if", "prog_line", "calls", "constant", "_" };
	Relation parent(2, arg1Type, arg2Type);
	_relationTable["parent"] = parent;
	_relationTable["parent*"] = parent;
	arg1Type.clear();
	arg2Type.clear();

	// Modifies
	arg1Type = { "stmt", "assign", "while", "if", "calls", "prog_line", "string", "constant", "procedure" };
	arg2Type = { "variable", "string", "_" };
	Relation modifies(2, arg1Type, arg2Type);
	_relationTable["modifies"] = modifies;
	arg1Type.clear();
	arg2Type.clear();

	// Uses
	arg1Type = { "stmt", "assign", "while", "if", "calls", "prog_line", "string", "constant", "procedure" };
	arg2Type = { "variable", "string", "_" };
	Relation uses(2, arg1Type, arg2Type);
	_relationTable["uses"] = uses;
	arg1Type.clear();
	arg2Type.clear();

	// assignPattern
	arg1Type = { "variable", "string", "_" };
	arg2Type = { "string", "constant", "_" };
	Relation patternAssign(2, arg1Type, arg2Type);
	_relationTable["patternAssign"] = patternAssign;
	arg1Type.clear();
	arg2Type.clear();

	// whilePattern
	arg1Type = { "variable", "string", "_" };
	arg2Type = { "_" };
	Relation patternWhile(2, arg1Type, arg2Type);
	_relationTable["patternWhile"] = patternWhile;
	arg1Type.clear();
	arg2Type.clear();

	// ifPattern
	arg1Type = { "variable", "string", "_" };
	arg2Type = { "_" };
	Relation patternIf(2, arg1Type, arg2Type);
	_relationTable["patternIf"] = patternIf;
	arg1Type.clear();
	arg2Type.clear();

	// Calls & Call*
	arg1Type = { "procedure", "string", "_" };
	arg2Type = { "procedure", "string", "_" };
	Relation calls(2, arg1Type, arg2Type);
	_relationTable["calls"] = calls;
	_relationTable["calls*"] = calls;
	arg1Type.clear();
	arg2Type.clear();

	// Next & Next*
	arg1Type = { "stmt", "assign", "while", "if", "calls", "prog_line", "constant", "_" };
	arg2Type = { "stmt", "assign", "while", "if", "calls", "prog_line", "constant", "_" };
	Relation next(2, arg1Type, arg2Type);
	_relationTable["next"] = next;
	_relationTable["next*"] = next;
	arg1Type.clear();
	arg2Type.clear();

	// With Type - Name 
	arg1Type = { "procedure", "call", "variable", "prog_line", "string" };
	arg2Type = { "procedure", "call", "variable", "prog_line", "string" };
	Relation withName(2, arg1Type, arg2Type);
	_relationTable["withName"] = withName;
	arg1Type.clear();
	arg2Type.clear();

	// With Type - Integer 
	arg1Type = { "stmt", "assign", "while", "if", "call", "prog_line", "constant" };
	arg2Type = { "stmt", "assign", "while", "if", "call", "prog_line", "constant" };
	Relation withInt(2, arg1Type, arg2Type);
	_relationTable["withInt"] = withInt;
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