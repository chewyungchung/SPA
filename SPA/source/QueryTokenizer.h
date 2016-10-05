#pragma once
#include <string>
#include "QueryToken.h"

using namespace std;

enum TOKEN_TYPE {
	OPEN_BRACKET,
	CLOSE_BRACKET,
	DOUBLE_QUOTE,
	UNDERSCORE,
	SEMICOLON,
	STAR,
	WHITESPACE,
	IDENT,
	INTEGER,
	LETTER,
	DIGIT,
	TAB,
	PROG_LINE,
	COMMA,
	ENDL,
	ERROR,
	STRING,
	DOT,
	HASH,
	EQUAL
};

class QueryTokenizer{

public:
	QueryTokenizer();
	QueryTokenizer(string queryString);
	QueryToken tokenize();


private:
	string _queryString;
	string _token;
	char _nextChar;
	int _charType;

	void addNextChar();
	void getNextChar();
};