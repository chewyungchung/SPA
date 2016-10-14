#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "map"
#include "unordered_map"

#include "Clause.h"
//#include "WeightTable.h"

using namespace std;

class QueryTable {
public:

	QueryTable();
	QueryTable(int is_null);
	QueryTable(Clause select_clause, vector<Clause> with_clauses, vector<Clause> such_that_clauses, vector<Clause> pattern_clauses);

	void Optimize();
	void InitialiseWeightTable();
	void ReplaceWithClauses();
	void GroupClauses();
	void SortClausesInGroup();
	void SortGroups();
	void PopulateNonSynClauses();
	void PopulateSynClauses();
	void GroupSynClauses();

	int GetNumOfSynInClause(vector<string> arg1_type, vector<string> arg2_type);

	// Get
	Clause GetSelectClause();
	vector<Clause> GetSuchThatClauses();
	vector<Clause> GetWithClauses();
	vector<Clause> GetPatternClauses();
	vector<Clause> GetNoSynGroup();
	vector<Clause> GetNonConnectedGroup();
	vector<Clause> GetConnectedGroup();

	int IsNullQuery();
	bool IsSynonym(string arg);
	string GetSynType(string syn_type);

	// Set
	void SetSelectClause(Clause select_clause);
	void SetSynEntityMap(unordered_map<string, string> synToEntityMap);
	void AddWithClause(Clause with_clause);
	void AddSuchThatClause(Clause such_that_clause);
	void AddPatternClause(Clause pattern_clause);

private:
	unordered_map<string, string> syn_entity_map_; 
	Clause select_clause_;
	vector<Clause> such_that_clauses_;
	vector<Clause> with_clauses_;
	vector<Clause> pattern_clauses_;

	vector<Clause> no_syn_group_;
	vector<Clause> with_syn_clauses_;

	vector<vector<Clause>> connected_groups_;
	vector<Clause> non_connected_group_;

	//WeightTable _weightTable;

	int is_null_;
};