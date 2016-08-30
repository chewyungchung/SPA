#pragma once

#include <string>
#include <vector>
#include <iostream>

using namespace std;

class QueryEvaluator {

public:
	// Default constructor
	QueryEvaluator();
	//Overloaded Constructor with QueryTree structure
	//QueryEvaluator(QueryTree);
	vector<string> evaluate();

private:
	vector<string> result;

	vector<string> processSuchThat();
	vector<string> processPattern();

	vector<string> processFollows();
	vector<string> processFollowsT();
	vector<string> processParent();
	vector<string> processParentT();
	vector<string> processUses();
	vector<string> processModifies();
};