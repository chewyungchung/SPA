#pragma once
#include "QueryTable.h"

class QueryResultProjector {
public:
	QueryResultProjector();
	QueryResultProjector(QueryTable);
	list<string> getResults();

private:
	QueryTable *_qt;
};