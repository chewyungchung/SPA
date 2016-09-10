#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

#include "Tokenizer.h"

class Parser
{

public:
	Parser(string fileName);
	void process();

private:
	Tokenizer tk;
	string next_token;
	string procName;
	int stmtLine;

	// Parent data
	int NO_PARENT_FLAG = -1;
	stack<int> parentStack;

	// Follows data
	int followsMaxNestingLevel;
	stack<int> followsStack;

	// SIMPLE language flags and stmtTypes
	string PROCEDURE_FLAG = "procedure";
	string WHILE_FLAG = "while";
	string ASSIGN_FLAG = "assign";
	string EQUAL_FLAG = "=";
	string SEMICOLON_FLAG = ";";
	string LEFT_BRACES = "{";
	string RIGHT_BRACES = "}";

	void match(string token);
	void parseProgram();
	void parseProcedure();
	void parseStmtLst();
	void parseWhileStmt();
	void parseAssignStmt();
	bool isConstant(string s);
};
