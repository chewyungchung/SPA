#pragma once
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <thread>

#include "Parser.h"
#include "PKB.h"
#include "QueryValidator.h"
#include "QueryResultProjector.h"
#include "ResultTable.h"

using namespace std;

// TODO: Refactor all names to Google cpp coding standard
class QueryEvaluator {

public:
	QueryEvaluator();
	~QueryEvaluator();
	QueryEvaluator(QueryTable, PKB);
	ResultTable evaluate();

private:
	ResultTable intermediate_result_;
	vector<string> _result;
	QueryTable _qt;
	PKB _pkb;

	// TODO: Modify all process clauses return type and implementation
	QueryResult processSuchThat(Clause suchThatClause);
	QueryResult processSelect(Clause selectClause);
	ResultTable ProcessPattern(Clause pattern_clause);
	
	// Iteration 2
	ResultTable ProcessFollows(Clause follow_clause);
	ResultTable ProcessFollowsT(Clause follow_star_clause);
	ResultTable ProcessParent(Clause parent_clause);
	ResultTable ProcessParentT(Clause parent_star_clause);
	ResultTable ProcessUses(Clause uses_clause);
	ResultTable ProcessModifies(Clause modifies_clause);
	ResultTable ProcessNext(Clause next_clause);
	ResultTable ProcessNextT(Clause next_star_clause);
	ResultTable ProcessCalls(Clause calls_clause);
	ResultTable ProcessCallsStar(Clause calls_star_clause);
	ResultTable ProcessPatternAssign(Clause pattern_assign_clause);
	ResultTable ProcessPatternWhile(Clause pattern_while_clause);
	ResultTable ProcessPatternIf(Clause pattern_if_clause);

	bool ProcessNoSynGroup();
	bool ProcessNonConnectedGroup();
	void ProcessConnectedGroup();

	int isInList(list<int> inList, int item);
	int isListEmpty(list<int> intList);

	list<int> getList(string arg_type);
};