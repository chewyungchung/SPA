#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "map"
#include "unordered_set"

#include "QueryResult.h"
#include "Clause.h"
//#include "WeightTable.h"

using namespace std;

class QueryTable {
public:

	QueryTable();
	~QueryTable();
	QueryTable(int isNull);
	QueryTable(Clause selectClause, vector<Clause> withClause, vector<Clause> suchThatClause, vector<Clause> patternClause);

	void optimize();
	void initialiseWeightTable();
	void replaceWithClauses();
	void groupClauses();
	void sortClausesInGroup();
	void sortGroups();
	void populateNonSynClauses();
	void populateSynClauses();
	void groupSynClauses();

	int getNumOfSynInClause(vector<string> arg1Type, vector<string> arg2Type);

	// Get
	Clause getSelectClause();
	vector<Clause> getSuchThatClauses();
	vector<Clause> getWithClauses();
	vector<Clause> getPatternClauses();
	QueryResult getSelectResult();
	QueryResult getSuchThatResult();
	QueryResult getPatternResult();

	int isSelectResultEmpty();
	int isSuchThatResultEmpty();
	int isPatternResultEmpty();
	int isNullQuery();

	bool isSynonym(string arg);

	string getSynType(string);

	// Set
	void setSelectClause(Clause selectClause);
	void setSuchThatClause(vector<Clause> suchThatClause);
	void setWithClause(vector<Clause> withClause);
	void setPatternClause(vector<Clause> patternClause);
	void setSelectResult(QueryResult qr);
	void setSuchThatResult(QueryResult qr);
	void setPatternResult(QueryResult qr);
	void setIsSuchThatAvail(int val);
	void setIsPatternAvail(int val);

	void setSynEntityMap(unordered_map<string, string> synToEntityMap);

private:
	
	unordered_map<string, string> _synEntityMap; 
	
	Clause _selectClause;
	vector<Clause> _suchThatClauses;
	vector<Clause> _withClauses;
	vector<Clause> _patternClauses;
	QueryResult _selectResult;
	QueryResult _suchThatResult;
	QueryResult _patternResult;

	vector<Clause> _nonSynGroup;
	vector<Clause> _withSynClauses;

	vector<vector<Clause>> _connectedGroups;
	vector<Clause> _nonConnectedGroup;

	WeightTable _weightTable;

	int _isNull;
};