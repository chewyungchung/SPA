#include "QueryResultProjector.h"
#include "QueryTable.h"
#include "QueryResult.h"
#include <vector>
#include <string>

using namespace std;

const string CLAUSE_SELECT = "SELECT";
const string CLAUSE_SUCH_THAT = "SUCH THAT";
const string CLAUSE_PATTERN = "PATTERN";
const string PARAM_ARG1 = "ARG1";
const string PARAM_ARG2 = "ARG2";
const string PARAM_PATTERN = "PATTERN";
const string PARAM_BOOLEAN = "BOOLEAN";
const string PARAM_EMPTY_STRING = "";

QueryResultProjector::QueryResultProjector() {
	_selectExist = false;
	_suchThatExist = false;
	_patternExist = false;
}

QueryResultProjector::~QueryResultProjector() {
	delete _qt;
}

QueryResultProjector::QueryResultProjector(QueryTable qt) {
	_qt = &qt;
}
list<string> QueryResultProjector::getResults() {
	list<string> finalResult;

	// Retrieve all the results from the QueryTable
	QueryResult *selectResult = (_qt->getSelectResult());
	QueryResult *suchThatResult = (_qt->getSuchThatResult());
	QueryResult *patternResult = (_qt->getPatternResult());

	if (selectResult != NULL) {
		_selectExist = true;
	}
	if (suchThatResult != NULL) {
		_suchThatExist = true;
	}
	if (patternResult != NULL) {
		_patternExist = true;
	}

	// If no results exist for the select clause, empty result is returned regardless
	if (_selectExist) {
		return finalResult;
	}

	// Get the expected result from select clause
	string selectSyn = getClauseSynonym(CLAUSE_SELECT, selectResult).at(0);

	// If we want BOOLEAN, then merge the results of SuchThat and Pattern and check if it's non-empty. If non-empty, return TRUE. Return FALSE otherwise.
	if (selectSyn == PARAM_BOOLEAN) {
		list<string> result = mergeResult(suchThatResult, patternResult);
		if (!result.empty()) {
			finalResult.push_back("TRUE");
			return finalResult;
		}
		else {
			finalResult.push_back("FALSE");
			return finalResult;
		}
	}
	// We want something else. Check if there are common syn between select and the other clauses. 
	// If true, merge the results of SuchThat and Pattern and get the results corresponding to the select syn
	// If false, merge the results of SuchThat and Pattern and check if they are non empty
	else {
		// selectSyn is a synonym
		if (isResultShareCommonSyn(selectSyn, suchThatResult, patternResult)) {
			finalResult = mergeResult(selectSyn, suchThatResult, patternResult);
		}
		else {
			list<string> mergedResult = mergeResult(suchThatResult, patternResult);
			if (!mergedResult.empty()) {
				finalResult = getListResult(selectResult->getArg1ResultList());
			}
			return finalResult;
		}
	}
}

bool QueryResultProjector::isResultShareCommonSyn(string selectSyn, QueryResult* suchThatResult, QueryResult* patternResult) {
	if (_suchThatExist && _patternExist) {
		vector<string> suchThatSyn = getClauseSynonym(CLAUSE_SUCH_THAT, suchThatResult);
		vector<string> patternSyn = getClauseSynonym(CLAUSE_PATTERN, patternResult);
		bool foundInSuchThat = find(suchThatSyn.begin(), suchThatSyn.end(), selectSyn) != suchThatSyn.end();
		bool foundInPattern = find(patternSyn.begin(), patternSyn.end(), selectSyn) != patternSyn.end();
		if (foundInSuchThat || foundInPattern) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (_suchThatExist) {
		vector<string> suchThatSyn = getClauseSynonym(CLAUSE_SUCH_THAT, suchThatResult);
		bool foundInSuchThat = find(suchThatSyn.begin(), suchThatSyn.end(), selectSyn) != suchThatSyn.end();
		if (foundInSuchThat) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (_patternExist) {
		vector<string> patternSyn = getClauseSynonym(CLAUSE_PATTERN, patternResult);
		bool foundInPattern = find(patternSyn.begin(), patternSyn.end(), selectSyn) != patternSyn.end();
		if (foundInPattern) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		// Both 'Such That' and 'Pattern' do not exist
		return false;
	}
}

vector<string> QueryResultProjector::getClauseSynonym(string clause, QueryResult* clauseResult) {
	vector<string> synList;
	if (clause == CLAUSE_SELECT) {
		synList.push_back(clauseResult->getSynonym(PARAM_ARG1));
	}
	else if (clause == CLAUSE_SUCH_THAT) {
		synList.push_back(clauseResult->getSynonym(PARAM_ARG1));
		synList.push_back(clauseResult->getSynonym(PARAM_ARG2));
	}
	else if (clause == CLAUSE_PATTERN) {
		synList.push_back(clauseResult->getSynonym(PARAM_ARG1));
		synList.push_back(clauseResult->getSynonym(PARAM_ARG2));
		synList.push_back(clauseResult->getSynonym(PARAM_PATTERN));
	}
	return synList;
}

// Does not involve select, check if result is non empty after merging
list<string> QueryResultProjector::mergeResult(QueryResult* suchThatResult, QueryResult* patternResult) {
	list<string> mergedResult;
	
	// Gotta check if suchThatResult or patternResult null
	if (_suchThatExist && _patternExist) {
		list<string> commonSyn = getCommonSynonym(suchThatResult, patternResult);
		// If no common synonyms, check that both queries are existential. If not, return an empty list to signify that whole query should be false
		if (commonSyn.empty()) {
			if (suchThatResult->getIsExist() && patternResult->getIsExist()) {
				mergedResult.push_back("dummyItem");
			}
			return mergedResult;
		}
		// Else, check that all the common synonym results once merged, are non empty
		else {
			bool nonEmpty = true;
			unordered_map<string, list<string>> commonSynResult = getCommonSynonymResult(commonSyn, suchThatResult, patternResult);
			for (list<string>::iterator it = commonSyn.begin(); it != commonSyn.end(); it++) {
				list<string> synResult = commonSynResult[*it];
				if (synResult.empty()) {
					nonEmpty = false;
					break;
				}
			}
			if (nonEmpty) {
				mergedResult.push_back("dummyItem");
			}
			return mergedResult;
		}
	}
	else if (_suchThatExist) {
		if (suchThatResult->getIsExist()) {
			mergedResult.push_back("dummyItem");
		}
		return mergedResult;
	}
	else if (_patternExist) {
		if (patternResult->getIsExist()) {
			mergedResult.push_back("dummyItem");
		}
		return mergedResult;
	}
	// Both SuchThat and Pattern results don't exist, return empty result to signify query is false
	else {
		return mergedResult;
	}
}

// Involves select
list<string> QueryResultProjector::mergeResult(string selectSyn, QueryResult* suchThatResult, QueryResult* patternResult) {
	list<string> mergedResult;

	// Gotta check if suchThatResult or patternResult null
	if (_suchThatExist && _patternExist) {
		list<string> commonSyn = getCommonSynonym(suchThatResult, patternResult);
		// If no common synonyms, check that both queries are existential. If yes, gotta get all the map
		// If not, return an empty list to signify that whole query should be false
		if (commonSyn.empty()) {
			if (suchThatResult->getIsExist() && patternResult->getIsExist()) {
				// Get a map with all the synonym results
				unordered_map<string, list<string>> synResultMap = getCommonSynonymResult(suchThatResult, patternResult);
				mergedResult = synResultMap[selectSyn];
			}
			return mergedResult;
		}
		// Else, once the commonSyn results are merged between SuchThat and Pattern, retrieve the results that correspond to select's syn
		else {
			unordered_map<string, list<string>> commonSynResult = getCommonSynonymResult(commonSyn, suchThatResult, patternResult);
			mergedResult = commonSynResult[selectSyn];
			return mergedResult;
		}
	}
	else if (_suchThatExist) {
		// Since isResultShareCommonSyn was TRUE, select clause definitely shared a syn with SuchThatResults
		if (suchThatResult->getIsExist()) {
			if (suchThatResult->getSynonym(PARAM_ARG1) == selectSyn) {
				mergedResult = getListResult(suchThatResult->getArg1ResultList());
			}
			else if (suchThatResult->getSynonym(PARAM_ARG2) == selectSyn) {
				mergedResult = getListResult(suchThatResult->getArg2ResultList());
			}
		}
		return mergedResult;
	}
	else if (_patternExist) {
		// Since isResultShareCommonSyn was TRUE, select clause definitely shared a syn with SuchThatResults
		if (patternResult->getIsExist()) {
			if (patternResult->getSynonym(PARAM_ARG1) == selectSyn) {
				mergedResult = getListResult(patternResult->getArg1ResultList());
			}
			else if (patternResult->getSynonym(PARAM_PATTERN) == selectSyn) {
				mergedResult = getListResult(patternResult->getPatternResultList());
			}
		}
		return mergedResult;
	}
	// Both SuchThat and Pattern results don't exist, return empty result to signify query is false
	else {
		return mergedResult;
	}
}

// Use for merging the common syn results
list<string> QueryResultProjector::getListIntersection(list<string> result1, list<string> result2) {
	list<string> intersection;
	// Remove possible dupes;
	result1.unique();
	result1.sort();
	result2.unique();
	result2.sort();

	for (list<string>::iterator it1 = result1.begin(); it1 != result1.end(); it1++) {
		for (list<string>::iterator it2 = result2.begin(); it2 != result2.end(); it2++) {
			if (*it1 == *it2) {
				intersection.push_back(*it1);
				break;
			}
		}
	}
	return intersection;
}

// Retrieve the common synonyms between SuchThat and Pattern clauses
list<string> QueryResultProjector::getCommonSynonym(QueryResult* suchThatResult, QueryResult* patternResult) {
	list<string> commonSyn;
	list<string> suchThatSyn;
	list<string> patternSyn;

	// Get all synonyms from both clauses
	if (suchThatResult->getSynonym(PARAM_ARG1) != PARAM_EMPTY_STRING) {
		suchThatSyn.push_back(suchThatResult->getSynonym(PARAM_ARG1));
	}
	if (suchThatResult->getSynonym(PARAM_ARG2) != PARAM_EMPTY_STRING) {
		suchThatSyn.push_back(suchThatResult->getSynonym(PARAM_ARG2));
	}
	if (patternResult->getSynonym(PARAM_ARG1) != PARAM_EMPTY_STRING) {
		suchThatSyn.push_back(patternResult->getSynonym(PARAM_ARG1));
	}
	if (patternResult->getSynonym(PARAM_PATTERN) != PARAM_EMPTY_STRING) {
		suchThatSyn.push_back(patternResult->getSynonym(PARAM_PATTERN));
	}

	// Iterate through both list and compare, if common, add to list
	for (list<string>::iterator it1 = suchThatSyn.begin(); it1 != suchThatSyn.end(); it1++) {
		for (list<string>::iterator it2 = patternSyn.begin(); it2 != patternSyn.end(); it2++) {
			if (*it1 == *it2) {
				commonSyn.push_back(*it1);
			}
		}
	}

	// Remove possible duplicates in commonSyn
	commonSyn.unique();
	return commonSyn;
}

// Used when suchThatResult and patternResult share no common synonyms
// Assert suchThatResult && patternResult != NULL
unordered_map<string, list<string>> QueryResultProjector::getCommonSynonymResult(QueryResult* suchThatResult, QueryResult* patternResult) {
	unordered_map<string, list<string>> synToResultMap;
	if (suchThatResult->getSynonym(PARAM_ARG1) != PARAM_EMPTY_STRING) {
		synToResultMap[suchThatResult->getSynonym(PARAM_ARG1)] = getListResult(suchThatResult->getArg1ResultList());
	}
	if (suchThatResult->getSynonym(PARAM_ARG2) != PARAM_EMPTY_STRING) {
		synToResultMap[suchThatResult->getSynonym(PARAM_ARG2)] = getListResult(suchThatResult->getArg2ResultList());
	}
	if (patternResult->getSynonym(PARAM_ARG1) != PARAM_EMPTY_STRING) {
		synToResultMap[patternResult->getSynonym(PARAM_ARG1)] = getListResult(patternResult->getArg1ResultList());
	}
	if (patternResult->getSynonym(PARAM_PATTERN) != PARAM_EMPTY_STRING) {
		synToResultMap[patternResult->getSynonym(PARAM_PATTERN)] = getListResult(patternResult->getPatternResultList());
	}
	return synToResultMap;
}

// Used when suchThatResult and patternResult share common synonyms
// Assert suchThatResult && patternResult != NULL
unordered_map<string, list<string>> QueryResultProjector::getCommonSynonymResult(list<string> commonSyn, QueryResult* suchThatResult, QueryResult* patternResult) {
	unordered_map<string, list<string>> commonSynMap;
	for (list<string>::iterator it = commonSyn.begin(); it != commonSyn.end(); it++) {
		list<string> suchThatCommonSynResult = getSynResult(*it, CLAUSE_SUCH_THAT, suchThatResult);
		list<string> patternCommonSynResult = getSynResult(*it, CLAUSE_PATTERN, patternResult);
		list<string> synResultIntersection = getListIntersection(suchThatCommonSynResult, patternCommonSynResult);
		commonSynMap[*it] = synResultIntersection;
	}
	return commonSynMap;
}

// Retrieve the result list corresponding to the syn and clause
list<string> QueryResultProjector::getSynResult(string syn, string clause, QueryResult* clauseResult) {
	if (clause == CLAUSE_SUCH_THAT) {
		if (clauseResult->getSynonym(PARAM_ARG1) == syn) {
			return getListResult(clauseResult->getArg1ResultList());
		}
		else if (clauseResult->getSynonym(PARAM_ARG2) == syn) {
			return getListResult(clauseResult->getArg2ResultList());
		}
	}
	else if (clause == CLAUSE_PATTERN) {
		if (clauseResult->getSynonym(PARAM_ARG1) == syn) {
			return getListResult(clauseResult->getArg1ResultList());
		}
		else if (clauseResult->getSynonym(PARAM_PATTERN) == syn) {
			return getListResult(clauseResult->getPatternResultList());
		}
	}
}

// Convert vector results into list results
list<string> QueryResultProjector::getListResult(vector<string> vectorResults) {
	list<string> returnList;
	for (vector<string>::iterator it = vectorResults.begin(); it != vectorResults.end(); it++) {
		returnList.push_back(*it);
	}
	return returnList;
}