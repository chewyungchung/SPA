#include "Relation.h"
#include <vector>
#include <string>

using namespace std;

Relation::Relation() {

}

Relation::Relation(int num_of_arg, vector<string> arg1_type, vector<string> arg2_type) {
	num_of_arg_ = num_of_arg;
	arg1_type_ = arg1_type;
	arg2_type_ = arg2_type;
}

int Relation::GetNumOfArg() {
	return num_of_arg_;
}

vector<string> Relation::GetArg1Type() {
	return arg1_type_;
}

vector<string> Relation::GetArg2Type() {
	return arg2_type_;
}