#pragma once

#include <vector>
#include <string>

using namespace std;

class Relation {

public:
	Relation();
	Relation(int num_of_arg, vector<string> arg1_type, vector<string> arg2_type);
	int GetNumOfArg();
	vector<string> GetArg1Type();
	vector<string> GetArg2Type();
private:
	int num_of_arg_;
	vector<string> arg1_type_;
	vector<string> arg2_type_;

};