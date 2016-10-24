#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Clause {
public:
	Clause();
	Clause(string relation, vector<string> arg, vector<string> arg_type);
	Clause(string relation, vector<string> arg, vector<string> arg_type, int priority);

	/* Getters */
	string GetRelation();
	vector<string> GetArg();
	vector<string> GetArgType();
	int GetPriority();
	bool IsClauseNull();

	/* Setters */
	void SetArg(int index, string arg);
	void SetArgType(int index, string arg_type);
	
private:
	string relation_;
	vector<string> arg_;
	vector<string> arg_type_;
	int priority_;
	bool is_null_;
};
