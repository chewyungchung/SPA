#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "SharedUtility.h"
#include "Clause.h"

using namespace std;

SharedUtility* sharedUtility;

/*
* Establish relation, store arg and argtype of clauses
*/

Clause::Clause()
{

}
Clause::Clause(string relation, vector<string> arg, vector<string> argType)
{
	_relation = relation;
	_arg = arg;
	_argType = argType;
	sharedUtility =new SharedUtility();
	
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

void run()
{
	
}