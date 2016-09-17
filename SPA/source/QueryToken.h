#pragma once
#include <string>

using namespace std;

class QueryToken {
public:
	QueryToken();
	QueryToken(int tokenType, string tokenName);
	int getTokenType();
	string getTokenName();
private:
	int _tokenType;
	string _tokenName;
};