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
#include "DesignExtractor.h"
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
	DesignExtractor _de;
	string next_token;
	string procName;
	int stmtLine;

	// Parent data	
	stack<int> parentStack;
	int NO_PARENT_FLAG = -1;

	// Follows data
	int followsMaxNestingLevel;
	stack<int> followsStack;

	// RHS data
	stack<string> bracketStack;
	string currRHS;

	// SIMPLE language flags and stmtTypes
	string PROCEDURE_FLAG = "procedure";
	string WHILE_FLAG = "while";
	string ASSIGN_FLAG = "assign";
	string CALL_FLAG = "calls";
	string IF_FLAG = "if";
	string THEN_FLAG = "then";
	string ELSE_FLAG = "else";
	string EQUAL_FLAG = "=";
	string PLUS_FLAG = "+";
	string MINUS_FLAG = "-";
	string TIMES_FLAG = "*";
	string SEMICOLON_FLAG = ";";
	string LEFT_BRACES = "{";
	string RIGHT_BRACES = "}";
	string LEFT_PARENTHESIS = "(";
	string RIGHT_PARENTHESIS = ")";

	void match(string token);
	void parseProgram();
	void parseProcedure();
	void parseStmtLst();
	void parseWhileStmt();
	void parseIfStmt();
	void parseElseStmt();
	void parseCallStmt();
	void parseAssignStmt();
	void parseAssignRHS();
	bool isConstant(string s);
	void addAllParentsOfUsedVariable(string v);
	void addAllParentsOfModifiedVariable(string v);
	void addAllParentsOfUsedConstant(int c);
};
