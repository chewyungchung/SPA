#include "RelationTable.h"

RelationTable::RelationTable() {
	// Init relation_table_
	vector<string> arg1_type;
	vector<string> arg2_type;

	// Follow & Follow*
	arg1_type = { "stmt", "assign", "while", "if", "calls", "constant", "_", "prog_line" };
	arg2_type = { "stmt", "assign", "while", "if", "calls", "constant", "_", "prog_line" };
	Relation follow(2, arg1_type, arg2_type);
	relation_table_["follows"] = follow;
	relation_table_["follows*"] = follow;
	arg1_type.clear();
	arg2_type.clear();

	// Parent & Parent*
	arg1_type = { "stmt", "prog_line", "while", "if", "constant", "_" };
	arg2_type = { "stmt", "assign", "while", "if", "prog_line", "calls", "constant", "_" };
	Relation parent(2, arg1_type, arg2_type);
	relation_table_["parent"] = parent;
	relation_table_["parent*"] = parent;
	arg1_type.clear();
	arg2_type.clear();

	// Modifies
	arg1_type = { "stmt", "assign", "while", "if", "calls", "prog_line", "string", "constant", "procedure" };
	arg2_type = { "variable", "string", "_" };
	Relation modifies(2, arg1_type, arg2_type);
	relation_table_["modifies"] = modifies;
	arg1_type.clear();
	arg2_type.clear();

	// Uses
	arg1_type = { "stmt", "assign", "while", "if", "calls", "prog_line", "string", "constant", "procedure" };
	arg2_type = { "variable", "string", "_" };
	Relation uses(2, arg1_type, arg2_type);
	relation_table_["uses"] = uses;
	arg1_type.clear();
	arg2_type.clear();

	// assignPattern
	arg1_type = { "variable", "string", "_" };
	arg2_type = { "string", "constant", "_" };
	Relation patternAssign(2, arg1_type, arg2_type);
	relation_table_["patternAssign"] = patternAssign;
	arg1_type.clear();
	arg2_type.clear();

	// whilePattern
	arg1_type = { "variable", "string", "_" };
	arg2_type = { "_" };
	Relation patternWhile(2, arg1_type, arg2_type);
	relation_table_["patternWhile"] = patternWhile;
	arg1_type.clear();
	arg2_type.clear();

	// ifPattern
	arg1_type = { "variable", "string", "_" };
	arg2_type = { "_" };
	Relation patternIf(2, arg1_type, arg2_type);
	relation_table_["patternIf"] = patternIf;
	arg1_type.clear();
	arg2_type.clear();

	// Calls & Call*
	arg1_type = { "procedure", "string", "_" };
	arg2_type = { "procedure", "string", "_" };
	Relation calls(2, arg1_type, arg2_type);
	relation_table_["calls"] = calls;
	relation_table_["calls*"] = calls;
	arg1_type.clear();
	arg2_type.clear();

	// Next & Next*
	arg1_type = { "stmt", "assign", "while", "if", "calls", "prog_line", "constant", "_" };
	arg2_type = { "stmt", "assign", "while", "if", "calls", "prog_line", "constant", "_" };
	Relation next(2, arg1_type, arg2_type);
	relation_table_["next"] = next;
	relation_table_["next*"] = next;
	arg1_type.clear();
	arg2_type.clear();

	// With Type - Name 
	arg1_type = { "procedure", "call", "variable", "prog_line", "string" };
	arg2_type = { "procedure", "call", "variable", "prog_line", "string" };
	Relation withName(2, arg1_type, arg2_type);
	relation_table_["withName"] = withName;
	arg1_type.clear();
	arg2_type.clear();

	// With Type - Integer 
	arg1_type = { "stmt", "assign", "while", "if", "call", "prog_line", "constant" };
	arg2_type = { "stmt", "assign", "while", "if", "call", "prog_line", "constant" };
	Relation withInt(2, arg1_type, arg2_type);
	relation_table_["withInt"] = withInt;
	arg1_type.clear();
	arg2_type.clear();
}

bool RelationTable::IsValidRelation(string relation) {
	unordered_map<string, Relation>::const_iterator it = relation_table_.find(relation);
	if (it == relation_table_.end()) {
		return false;
	}
	else {
		return true;
	}
}

bool RelationTable::IsArg1Valid(string relation, string argType) {
	// Pre-requisite: Relation exist in relation_table_
	Relation rel = relation_table_.find(relation)->second;
	vector<string> arg1_type = rel.GetArg1Type();
	for (vector<string>::iterator it = arg1_type.begin(); it != arg1_type.end(); it++) {
		if (argType == *it) {
			return true;
		}
	}
	return false;
}

bool RelationTable::IsArg2Valid(string relation, string argType) {
	// Pre-requisite: Relation exist in relation_table_
	Relation rel = relation_table_.find(relation)->second;
	vector<string> arg2_type = rel.GetArg2Type();
	for (vector<string>::iterator it = arg2_type.begin(); it != arg2_type.end(); it++) {
		if (argType == *it) {
			return true;
		}
	}
	return false;
}

bool RelationTable::IsCorrectNumArg(string relation, int num_of_arg) {
	// Pre-requisite: Relation exist in relation_table_
	Relation rel = relation_table_.find(relation)->second;
	return (rel.GetNumOfArg() == num_of_arg);
}