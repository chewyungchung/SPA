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
	is_null_ = true;
}
Clause::Clause(string relation, vector<string> arg, vector<string> arg_type)
{
	relation_ = relation;
	arg_ = arg;
	arg_type_ = arg_type;
	is_null_ = false;
}

Clause::Clause(string relation, vector<string> arg, vector<string> arg_type, int priority)
{
	relation_ = relation;
	arg_ = arg;
	arg_type_ = arg_type;
	priority_ = priority;
	is_null_ = false;
}

string Clause::GetRelation()
{
	return relation_;
}

vector<string> Clause::GetArg()
{
	return arg_;
}

vector<string> Clause::GetArgType()
{
	return arg_type_;
}

void Clause::SetArg(int index, string arg)
{
	if (index < 0 || index > 1) return;
	arg_.at(index) = arg;
}

void Clause::SetArgType(int index, string arg_type)
{
	if (index < 0 || index > 1) return;
	arg_type_.at(index) = arg_type;
}


bool Clause::IsClauseNull() {
	return is_null_;
}

int Clause::GetPriority()
{
	return priority_;
}
