#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>

#include "PQLVerifyTable.h"



using namespace std;


class PQLVerifyTable {
public:
	PQLVerifyTable(vector<vector<string>> entityTable,vector<string> entities);
	//     start method
	bool check(int formatSize,vector<string> unverifiedQuery);
	
	//										 ensureSelectSomethingExist()
	//										             |
	//	------------------suchThat()------------------------------------------------pattern()--------------
	//	|        |            |             |                     			       |        |            | 
	//isSuch()  isThat() isValidClause()   isValidContent()             isPattern()  isEntity() isValidPattern() 
	bool ensureSelectSomethingExist(string firstString,string someVariable, vector<string> allEntitiesDeclarations);

	//			---------suchThat()-----------------
	//			|        |            |            |
	//		isSuch()  isThat() isValidClause() isValidContent ()
	bool PQLVerifyTable::suchThat(vector<string> unverifiedQuery, int intSuch,int intThat, int intClause, int intBracketContent);

	//			---------pattern()-----------------
	//			|        |            |            |
	//		isPattern()  isEntity() isValidPattern() 
	bool PQLVerifyTable::pattern(vector<string> unverifiedQuery, int intPatternPos, int intEntityPos, int intPatternContentPos);



	//is such that clause (content)
	bool PQLVerifyTable::isSuch(string content);
	bool PQLVerifyTable::isThat(string content);
	bool PQLVerifyTable::isValidClause(string content);
	bool PQLVerifyTable::isValidContent(string clause,string content);

	//pattern variable (pattern)
	bool PQLVerifyTable::isPattern(string content);
	bool PQLVerifyTable::isEntity(string content);
	bool PQLVerifyTable::isValidPattern(string content);

	//miscellaneous methods
	vector<string> addBoolean(vector<string> entities);
	vector<string> extractDeclaration(vector<vector<string>> extractDeclaration);
	string PQLVerifyTable::removeSpaces(string content);
	string PQLVerifyTable::removeFrontAndBack(string content);
	vector<string> PQLVerifyTable::split(string str, char delimiter);
	void PQLVerifyTable::checkAllResult();

	//check methods
	bool PQLVerifyTable::briefContentCheck(string content);
	bool PQLVerifyTable::bracketCheck(string content);
	bool PQLVerifyTable::patternBracketContentCheck(string content);
	bool PQLVerifyTable::clausesBracketContentCheck(string clause,string content);
	bool PQLVerifyTable::stmtRefCheck(string content);
	bool PQLVerifyTable::entRefCheck(string content);
	bool PQLVerifyTable::patternExpressionCheck(string content);


	//get method
	void PQLVerifyTable::getClausesList();

};
