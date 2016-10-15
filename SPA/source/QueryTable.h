#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include "map"
#include "unordered_map"

#include "Clause.h"
//#include "WeightTable.h"

using namespace std;

class QueryTable {
public:
	struct ClauseComparator {
		bool operator() (Clause lhs, Clause rhs) {
			return lhs.GetPriority() > rhs.GetPriority();
		}
	};

	// Constructors
	QueryTable();
	QueryTable(bool is_null);
	QueryTable(Clause select_clause, vector<Clause> with_clauses, vector<Clause> such_that_clauses, vector<Clause> pattern_clauses);

	// Optimization - Public
	void Optimize();

	// Getters
	Clause GetSelectClause();
	vector<Clause> GetSuchThatClauses();
	vector<Clause> GetWithClauses();
	vector<Clause> GetPatternClauses();
	vector<Clause> GetNoSynGroup();
	vector<Clause> GetNonConnectedGroup();
	vector<Clause> GetConnectedGroup();
	vector<vector<Clause>> GetConnectedGroups();
	string GetSynType(string syn_type);
	bool IsNullQuery();

	// Setters
	void SetSelectClause(Clause select_clause);
	void SetSynEntityMap(unordered_map<string, string> synToEntityMap);
	void AddWithClause(Clause with_clause);
	void AddSuchThatClause(Clause such_that_clause);
	void AddPatternClause(Clause pattern_clause);

private:
	// Optimization - Private
	void CategorizeClauses();
	void BuildAllClause();
	void PopulateWithConstantClauses();
	void PopulateNoSynClauses();
	void PopulateOneSynClauses();
	void PopulateTwoSynClauses();
	void PopulateNonConnectedGroup();
	void EvaluateWithConstantClauses();
	void GroupClauses();
	int GetNumOfSynInClause(Clause clause);
	void ReplaceWithClauses(); // Not used yet

	// Getters - Private
	string GetSecondSynonym(Clause clause, string select_syn);
	bool IsConstantWithClause(Clause with_clause);
	bool HasSelectedSynonym(Clause clause);
	bool IsSynonym(string arg);
	bool IsSynFound(Clause clause, string synonym);
	bool IsSynSelected(Clause select_clause);

	unordered_map<string, string> syn_entity_map_;
	bool is_null_;

	// Original Clauses
	Clause select_clause_;
	vector<Clause> such_that_clauses_;
	vector<Clause> pattern_clauses_;
	vector<Clause> with_clauses_;

	// Groups
	vector<Clause> no_syn_group_;
	vector<Clause> non_connected_group_;
	vector<Clause> connected_group_;
	vector<vector<Clause>> connected_groups_;

	// Main Structures
	vector<Clause> all_clauses_;
	vector<Clause> one_syn_clauses_;
	vector<Clause> two_syn_clauses_;
	vector<Clause> with_constant_clauses_;
};