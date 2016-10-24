#pragma once

#include <string>

#include "QueryToken.h"

using namespace std;

enum TOKEN_TYPE {
	OPEN_BRACKET,
	CLOSE_BRACKET,
	OPEN_ANGLE_BRACKET,
	CLOSE_ANGLE_BRACKET,
	DOUBLE_QUOTE,
	UNDERSCORE,
	SEMICOLON,
	STAR,
	WHITESPACE,
	IDENT,
	HASH_IDENT,
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
	EQUAL,
	PLUS,
	MINUS
};

class QueryTokenizer{
public:
	QueryTokenizer();
	QueryTokenizer(string queryString);
	QueryToken Tokenize();

private:
	void AddNextChar();
	void GetNextChar();

	string query_string_;
	string token_;
	char next_char_;
	int char_type_;
};