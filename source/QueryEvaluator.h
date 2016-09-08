#pragma once

#include "Clause.h"
#include "QueryTable.h"
#include "QueryResult.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class QueryEvaluator {

public:
	// Default constructor
	QueryEvaluator();
	QueryEvaluator(QueryTable);
	QueryTable evaluate();

private:
	vector<string> _result;

	QueryTable _qt;

	bool processSuchThat(Clause suchThatClause);
	
	QueryResult processSelect(Clause selectClause);
	QueryResult processPattern(Clause patternClause);
	QueryResult processFollows(Clause followClause);
	QueryResult processFollowsT(Clause followTClause);
	QueryResult processParent(Clause parentClause);
	QueryResult processParentT(Clause parentTClause);
	QueryResult processUses(Clause usesClause);
	QueryResult processModifies(Clause modifiesClause);

	bool isInList(list<int> inList, int item);
	bool isListEmpty(list<int> intList);

	list<int> getList(string argType);
};