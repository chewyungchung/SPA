#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;


class Clause {
public:

	Clause();
	Clause(string relation, vector<string> arg, vector<string> argType);
	//Clause(string withType, vector<string> arg1, vector<string> arg2);
	string getRelation();
	vector<string> getArg();
	vector<string> getArgType();
	int isClauseNull();

private:
	string _relation;
	vector<string> _arg;
	vector<string> _argType;
	string _withType;
	vector<string> _arg1;
	vector<string> _arg2;

	int _isNull;
};
