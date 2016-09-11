#include "Relation.h"
#include <vector>
#include <string>

using namespace std;

Relation::Relation() {

}

Relation::Relation(int numOfArg, vector<string> arg1Type, vector<string> arg2Type) {
	_numOfArg = numOfArg;
	_arg1Type = arg1Type;
	_arg2Type = arg2Type;
}

int Relation::getNumOfArg() {
	return _numOfArg;
}

vector<string> Relation::getArg1Type() {
	return _arg1Type;
}

vector<string> Relation::getArg2Type() {
	return _arg2Type;
}