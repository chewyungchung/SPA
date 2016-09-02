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
	relation_ = relation;
	arg_ = arg;
	argType_ = argType;
	sharedUtility =new SharedUtility();
	
}

string Clause::getRelation()
{
	return NULL;
}

vector<string>* Clause::getArg()
{
	return NULL;
}

vector<string>* Clause::getArgType()
{
	return NULL;
}

void run()
{
	
}