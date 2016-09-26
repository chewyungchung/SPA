/*
Parser receives and tokenizes a user-specified SIMPLE source. 
Parser then populates the PKB with program design abstractions.
This PKB is the single instance shared throughout the program for this particular SIMPLE source iteration.
*/
#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

#include "Tokenizer.h"
#include "PKB.h"

class Parser
{
public:
	Parser();
	Parser(string fileName);
	PKB process();

private:
	PKB _pkb;
	Tokenizer _tk;
	string next_token;
	string procName;
	int stmtLine;

	// Parent data	
	stack<int> parentStack;
	int NO_PARENT_FLAG = -1;

	// Follows data
	int followsMaxNestingLevel;
	stack<int> followsStack;

	// SIMPLE language flags and stmtTypes
	string PROCEDURE_FLAG = "procedure";
	string WHILE_FLAG = "while";
	string ASSIGN_FLAG = "assign";
	string CALL_FLAG = "call";
	string EQUAL_FLAG = "=";
	string PLUS_FLAG = "+";
	string SEMICOLON_FLAG = ";";
	string LEFT_BRACES = "{";
	string RIGHT_BRACES = "}";

	void match(string token);
	void parseProgram();
	void parseProcedure();
	void parseStmtLst();
	void parseWhileStmt();
	void parseCallStmt();
	void parseAssignStmt();
	void parseAssignRHS();
	bool isConstant(string s);
	void addAllParentsOfUsedVariable(string v);
	void addAllParentsOfModifiedVariable(string v);
	void addAllParentsOfUsedConstant(int c);
};
