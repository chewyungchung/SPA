#pragma once

#include "Clause.h"
#include "QueryTable.h"
#include "QueryResult.h"
#include "QueryValidator.h"
#include "QueryResultProjector.h"
#include "PKB.h"
#include "Parser.h"
#include <string>
#include <vector>
#include <iostream>
#include <chrono>
#include <thread>


using namespace std;

class QueryEvaluator {

public:
	QueryEvaluator();
	~QueryEvaluator();
	QueryEvaluator(QueryValidator, PKB, list<string>*);
	QueryEvaluator(QueryTable, PKB);
	QueryTable evaluate();

private:
	vector<string> _result;
	QueryTable _qt;
	PKB _pkb;

	QueryResult processSuchThat(Clause suchThatClause);
	QueryResult processSelect(Clause selectClause);
	QueryResult processPattern(Clause patternClause);
	QueryResult processFollows(Clause followClause);
	QueryResult processFollowsT(Clause followTClause);
	QueryResult processParent(Clause parentClause);
	QueryResult processParentT(Clause parentTClause);
	QueryResult processUses(Clause usesClause);
	QueryResult processModifies(Clause modifiesClause);

	int isInList(list<int> inList, int item);
	int isListEmpty(list<int> intList);

	list<int> getList(string argType);
};