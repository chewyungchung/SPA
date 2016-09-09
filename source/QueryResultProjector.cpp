#include "QueryResultProjector.h"
#include "QueryTable.h"
#include "QueryResult.h"
#include <vector>
#include <string>

using namespace std;

QueryResultProjector::QueryResultProjector() {

}

QueryResultProjector::QueryResultProjector(QueryTable qt) {
	_qt = &qt;
}
list<string> QueryResultProjector::getResults() {
	list<string> results;

	// Retrieve all the results from the QueryTable
	QueryResult *selectResults = &(_qt->getSelectResult());
	QueryResult *suchThatResults = &(_qt->getSuchThatResult());
	QueryResult *patternResults = &(_qt->getPatternResult());

	bool selectExists = selectResults -> getIsExist();
	bool suchThatExists = suchThatResults -> getIsExist();
	bool patternExists = patternResults -> getIsExist();

	return results;
}

