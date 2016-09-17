#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "Clause.h"

using namespace std;

/*
* Establish relation, store arg and argtype of clauses
*/

Clause::Clause()
{
	_isNull = 1;
}
Clause::Clause(string relation, vector<string> arg, vector<string> argType)
{
	_relation = relation;
	_arg = arg;
	_argType = argType;
	_isNull = -1;
}

string Clause::getRelation()
{
	return _relation;
}

vector<string> Clause::getArg()
{
	return _arg;
}

vector<string> Clause::getArgType()
{
	return _argType;
}


int Clause::isClauseNull() {
	return _isNull;
}