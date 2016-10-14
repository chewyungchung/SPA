//#pragma once
//#include <unordered_map>
//#include <vector>
//#include <string>
//#include <algorithm>
//
//#include "QueryTable.h"
//#include "PKB.h"
//
//using namespace std;
//
//class QueryResultProjector {
//public:
//	QueryResultProjector();
//	~QueryResultProjector();
//	QueryResultProjector(QueryTable);
//	QueryResultProjector(QueryTable, PKB);
//	list<string> getResults();
//
//	// Check between all clauses
//	int isResultShareCommonSyn(string selectSyn, QueryResult suchThatResult, QueryResult patternResult);
//	vector<string> getClauseSynonym(string clause, QueryResult);
//	// Does not involve select
//	list<string> mergeResult(QueryResult suchThatResult, QueryResult patternResult);
//	// Involve select
//	list<string> mergeResult(string selectSyn, QueryResult suchThatResult, QueryResult patternResult);
//	list<string> getListIntersection(list<string> result1, list<string> result2);
//	list<string> getListResult(vector<string> vectorResults);
//	list<string> getSynResult(string syn, string clause, QueryResult clauseResult);
//	list<string> getCommonSynonym(QueryResult suchThatResult, QueryResult patternResult);
//	list<string> checkAgain(list<string> interserctionResults, string selectSyn, string commonSyn);
//
//	unordered_map<string, list<string>> getCommonSynonymResult(QueryResult suchThatResult, QueryResult patternResult);
//	unordered_map<string, list<string>> getCommonSynonymResult(list<string> commonSyn, QueryResult suchThatResult, QueryResult patternResult);
//
//private:
//	QueryTable _qt;
//	PKB _pkb;
//	int _selectExist = -1;
//	int _suchThatExist = -1;
//	int _patternExist = -1;
//	int _suchThatNull = 1;
//	int _patternNull = 1;
//};