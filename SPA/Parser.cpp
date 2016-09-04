#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <stack>
#include <vector>

using namespace std;

#include "Parser.h"
#include "Tokenizer.h"
#include "../source/PKB.h"

Parser::Parser(FILE f)
{
	Tokenizer tk(f);
	stmtLine = 1;
	parentStack.push(NO_PARENT_FLAG);
	followsMaxNestingLevel = 1;
	followsStack.push(followsMaxNestingLevel);
}


void Parser::process()
{
	parseProgram();
}


void Parser::match(string token)
{
	if (next_token == token)
	{
		next_token = tk.getToken();
	}
	else
	{
		throw invalid_argument("SIMPLE syntax error, check SIMPLE source");
	}
}


void Parser::parseProgram()
{
	next_token = tk.getToken();
	parseProcedure();
}

void Parser::parseProcedure()
{
	match(PROCEDURE_FLAG);
	procName = next_token;
	match(procName);
	match(LEFT_BRACES);
	parseStmtLst();
	match(RIGHT_BRACES);
}

void Parser::parseStmtLst()
{
	if (next_token == WHILE_FLAG)
	{
		parseWhileStmt();
	}
	else
	{
		parseAssignStmt();
	}
	match(SEMICOLON_FLAG);
	stmtLine++;
	if (next_token == RIGHT_BRACES)
	{
		return;
	}
	else
	{
		parseStmtLst();
	}
}

void Parser::parseWhileStmt()
{
	PKB.addParent(stmtLine, parentStack.top());
	parentStack.push(stmtLine);

	PKB.addFollows(stmtLine, followsStack.top());
	followsMaxNestingLevel++;
	followsStack.push(followsMaxNestingLevel);

	match(WHILE_FLAG);
	PKB.addUsesbyVar(next_token, stmtLine);
	PKB.addUsesbyStmt(stmtLine, next_token);

	match(LEFT_BRACES);
	stmtLine++;
	parseStmtLst();
	match(RIGHT_BRACES);
	stmtLine++;

	parentStack.pop();
	followsStack.pop();
}

void Parser::parseAssignStmt()
{
	PKB.addParent(stmtLine, parentStack.top());
	PKB.addFollows(stmtLine, followsStack.top());

	string varName = next_token;
	PKB.addModifiesbyVar(next_token, stmtLine);
	PKB.addModifiesbyStmt(stmtLine, next_token);
	match(varName);
	match(EQUAL_FLAG);
	varName = next_token;
	PKB.addUsesbyVar(next_token, stmtLine);
	PKB.addUsesbyStmt(stmtLine, next_token);
}
