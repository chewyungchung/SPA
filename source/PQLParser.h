#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>

#include "PQLParser.h"
#include "Clause.h"
#include "PQLVerifyTable.h"

using namespace std;


class PQLParser {
public:
	void Start(string declaration, vector<string> queries);
	vector<vector< string>> declarationAnalyzer(string stringDeclaration);
	void queryAnalyzer(string stringQuery);
	void identifyVariablesInBracket(string input);
	void printEntityTable(vector<vector<string>> entityTable);
	void printQueryTable(vector<vector<string>> unverifiedTable);
	void verifyQuery(vector<vector<string>>univerifyQuery, vector<vector<string>> entityTable, vector<string> allEntities, vector<Clause> clauses);
	void printAllUnverifyQueries(vector<vector<string>>univerifyQuery);
	pair<bool, vector<string>> pattern(vector<string> currentQuery, vector<string> allEntities, int startPos);
	pair<bool, vector<string>> suchThat(vector<string> currentQuery, vector<string> allEntities, int startPos);
	string printWholeArray(vector<string> arrayOfString);
	bool checkStringIsEntity(string input, vector<string> allEntities);
	void printAllEntities(vector<string> allEntities);

	string removeSpaces(string input);
	vector< string> split(string str, char delimiter);
	vector<vector< string>> getEntityTable();
	pair<bool, string> patternCheckFirst(string input, vector<string> allEntities);
	pair<bool, string> patternCheckSecond(string input, vector<string> allEntities);
	pair<bool, string> PQLParser::clausesCheckFirst(string input, vector<string> allEntities);
	pair<bool, string> PQLParser::clausesCheckSecond(string input, vector<string> allEntities);


	vector<vector< string >> entityTable;
	vector< string> pdeProcedureList;
	vector< string> pdeStmtListList;
	vector< string> pdeStmtList;
	vector< string> pdeAssignmentList;
	vector< string> pdeAssignList;
	vector< string> pdeCallList;
	vector< string> pdeWhileList;
	vector< string> pdeIfList;
	vector< string> pdeVariableList;
	vector< string> pdeConstantList;
	vector< string> pdeProg_LineList;
	vector< string> tempResultQuery;
	vector< vector<string> > unverifyQuery;

};
