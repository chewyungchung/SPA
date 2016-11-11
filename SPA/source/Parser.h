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
	Parser(string file_name);
	PKB Process();

private:
	PKB pkb_;
	Tokenizer tokenizer_;
	DesignExtractor design_extractor_;
	string next_token_;
	string proc_name_;
	int curr_stmt_line_;
	int curr_first_stmt_of_proc_;

	// Parent data	
	stack<int> parent_stack_;
	int NO_PARENT_FLAG = -1;

	// Follows data
	int follows_max_nest_lvl_;
	stack<int> follows_stack_;

	// RHS data
	stack<string> bracket_stack_;
	string curr_rhs_;

	// SIMPLE language flags and stmtTypes
	string PROCEDURE_FLAG = "procedure";
	string WHILE_FLAG = "while";
	string ASSIGN_FLAG = "assign";
	string CALL_FLAG = "call";
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

	void Match(string token);
	void ParseProgram();
	void ParseProcedure();
	void ParseStmtLst();
	void ParseWhileStmt();
	void ParseIfStmt();
	void ParseElseStmt();
	void ParseCallStmt();
	void ParseAssignStmt();
	void ParseAssignRHS();
	void ParseExpression();
	void ParseTerm();
	void ParseFactor();
	bool IsConstant(string s);
	void AddAllParentsOfCurrStmt(int stmt_line);
	void AddAllParentsOfUsedVariable(string v);
	void AddAllParentsOfModifiedVariable(string v);
	void AddAllParentsOfUsedConstant(int c);
};
