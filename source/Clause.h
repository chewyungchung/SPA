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
	string getRelation();
	vector<string> getArg();
	vector<string> getArgType();

private:
	string _relation;
	vector<string> _arg;
	vector<string> _argType;

	 
};
