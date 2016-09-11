#pragma once
#include <vector>
#include <string>

using namespace std;

class Relation {

public:
	Relation();
	Relation(int numOfArg, vector<string> arg1Type, vector<string> arg2Type);
	int getNumOfArg();
	vector<string> getArg1Type();
	vector<string> getArg2Type();
private:
	int _numOfArg;
	vector<string> _arg1Type;
	vector<string> _arg2Type;

};