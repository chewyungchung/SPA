#pragma once
#include "QueryTable.h"
#include <unordered_map>

class QueryResultProjector {
public:
	QueryResultProjector();
	~QueryResultProjector();
	QueryResultProjector(QueryTable);
	list<string> getResults();

	// Check between all clauses
	bool isResultShareCommonSyn(string selectSyn, QueryResult* suchThatResult, QueryResult* patternResult);
	vector<string> getClauseSynonym(string clause, QueryResult*);
	// Does not involve select
	list<string> mergeResult(QueryResult* suchThatResult, QueryResult* patternResult);
	// Involve select
	list<string> mergeResult(string selectSyn, QueryResult* suchThatResult, QueryResult* patternResult);
	list<string> getListIntersection(list<string> result1, list<string> result2);
	list<string> getListResult(vector<string> vectorResults);
	list<string> getSynResult(string syn, string clause, QueryResult* clauseResult);
	list<string> getCommonSynonym(QueryResult* suchThatResult, QueryResult* patternResult);

	unordered_map<string, list<string>> getCommonSynonymResult(QueryResult* suchThatResult, QueryResult* patternResult);
	unordered_map<string, list<string>> getCommonSynonymResult(list<string> commonSyn, QueryResult* suchThatResult, QueryResult* patternResult);

private:
	QueryTable *_qt;
	bool _selectExist;
	bool _suchThatExist;
	bool _patternExist;
};