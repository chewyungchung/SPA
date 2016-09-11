#include "QueryTokenizer.h"

QueryTokenizer::QueryTokenizer() {

}

QueryTokenizer::QueryTokenizer(string queryString) {
	_queryString = queryString;
	_nextChar = ' ';
	_token = "";
	_charType = ERROR;
	getNextChar();
}

QueryToken QueryTokenizer::tokenize() {
	_token = "";
	bool isUnderscore = false;
	// Skip all the tab + space
	while (_charType == WHITESPACE || _charType == TAB) {
		getNextChar();
	}

	if (_charType == ENDL) {
		return QueryToken(ENDL, _token);
	}
	if (_charType == ERROR) {
		addNextChar();
		getNextChar();
		return QueryToken(ERROR, _token);
	}

	if (_charType == LETTER) {
		addNextChar();
		getNextChar();
		while (_charType == UNDERSCORE || _charType == LETTER || _charType == DIGIT) {
			if (_charType == UNDERSCORE) {
				isUnderscore = true;
			}
			addNextChar();
			getNextChar();
		}
		if (isUnderscore) {
			if (_token == "prog_line") {
				return QueryToken(PROG_LINE, _token);
			}
			else {
				return QueryToken(ERROR, _token);
			}
		}
		return QueryToken(IDENT, _token);
	}
	else if (_charType == DIGIT) {
		addNextChar();
		getNextChar();
		while (_charType == DIGIT) {
			addNextChar();
			getNextChar();
		}
		return QueryToken(INTEGER, _token);
	}
	else if (_charType == STAR) {
		addNextChar();
		getNextChar();
		return QueryToken(STAR, _token);
	}
	else if (_charType == SEMICOLON) {
		addNextChar();
		getNextChar();
		return QueryToken(SEMICOLON, _token);
	}
	else if (_charType == OPEN_BRACKET) {
		addNextChar();
		getNextChar();
		return QueryToken(OPEN_BRACKET, _token);
	}
	else if (_charType == CLOSE_BRACKET) {
		addNextChar();
		getNextChar();
		return QueryToken(CLOSE_BRACKET, _token);
	}
	else if (_charType == DOUBLE_QUOTE) {
		addNextChar();
		getNextChar();
		return QueryToken(DOUBLE_QUOTE, _token);
	}
	else if (_charType == UNDERSCORE) {
		addNextChar();
		getNextChar();
		return QueryToken(UNDERSCORE, _token);
	}
	else if (_charType == COMMA) {
		addNextChar();
		getNextChar();
		return QueryToken(COMMA, _token);
	}
	else {
		return QueryToken(ERROR, _token);
	}
}

void QueryTokenizer::getNextChar() {
	// Process queryString character by character
	if (_queryString.length() > 0) {
		_nextChar = _queryString[0];
		_queryString.erase(0, 1);
	}
	else {
		_nextChar = '~';
	}
	_charType = ENDL;

	// If ascii numbers
	if (_nextChar > 47 && _nextChar < 58) {
		_charType = DIGIT;
	}
	// If ascii alphabets
	if ((_nextChar > 64 && _nextChar < 91) || (_nextChar > 96 && _nextChar < 123)) {
		_charType = LETTER;
	}
	switch (_nextChar) {
		case '(': _charType = OPEN_BRACKET; break;
		case ')': _charType = CLOSE_BRACKET; break;
		case '\"':_charType = DOUBLE_QUOTE; break;
		case '_': _charType = UNDERSCORE; break;
		case ';': _charType = SEMICOLON; break;
		case '*': _charType = STAR; break;
		case ' ': _charType = WHITESPACE; break;
		case '\t':_charType = TAB; break;
		case ',': _charType = COMMA; break;
		case '~': _charType = ENDL; break;
		default: break;
	}
}

void QueryTokenizer::addNextChar() {
	_token += _nextChar;
}
