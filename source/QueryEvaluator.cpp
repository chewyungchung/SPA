#include "QueryEvaluator.h"
#include <string>

const string REL_FOLLOWS = "FOLLOWS";
const string REL_FOLLOWS_STAR = "FOLLOWS*";
const string REL_PARENT = "PARENT";
const string REL_PARENT_STAR = "PARENT*";
const string REL_MODIFIES = "MODIFIES";
const string REL_USES = "USES";

// Default Constructor
QueryEvaluator::QueryEvaluator() {

}

QueryEvaluator::QueryEvaluator(QueryTable qt) {

}

vector<string> QueryEvaluator::evaluate() {
	// Traverse query table and build result

	// 

	return result;
}

vector<string> QueryEvaluator::processSuchThat() {
	return result;
}

vector<string> QueryEvaluator::processPattern() {
	return result;
}

vector<string> QueryEvaluator::processFollows() {
	return result;
}

vector<string> QueryEvaluator::processFollowsT() {
	return result;
}

vector<string> QueryEvaluator::processParent() {
	return result;
}

vector<string> QueryEvaluator::processParentT() {
	return result;
}

vector<string> QueryEvaluator::processModifies() {
	return result;
}

vector<string> QueryEvaluator::processUses() {
	return result;
}