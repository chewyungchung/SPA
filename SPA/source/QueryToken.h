#pragma once

#include <string>

using namespace std;

class QueryToken {
public:
	QueryToken();
	QueryToken(int token_type, string token_name);
	
	/* Getters */
	int GetTokenType();
	string GetTokenName();

private:
	int token_type_;
	string token_name_;
};