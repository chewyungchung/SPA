#include "QueryTokenizer.h"

QueryTokenizer::QueryTokenizer() {

}

QueryTokenizer::QueryTokenizer(string queryString) {
	query_string_ = queryString;
	next_char_ = ' ';
	token_ = "";
	char_type_ = ERROR;
	GetNextChar();
}

QueryToken QueryTokenizer::Tokenize() {
	token_ = "";
	bool is_underscore = false;
	bool is_hash_present = false;
	// Skip all the tab + space
	while (char_type_ == WHITESPACE || char_type_ == TAB) {
		GetNextChar();
	}

	if (char_type_ == ENDL) {
		return QueryToken(ENDL, token_);
	}
	if (char_type_ == ERROR) {
		AddNextChar();
		GetNextChar();
		return QueryToken(ERROR, token_);
	}

	if (char_type_ == LETTER) {
		AddNextChar();
		GetNextChar();
		while (char_type_ == UNDERSCORE || char_type_ == LETTER || char_type_ == DIGIT ||
				char_type_ == HASH) {
			if (char_type_ == UNDERSCORE) {
				is_underscore = true;
			}
			if (char_type_ == HASH) {
				is_hash_present = true;
			}
			AddNextChar();
			GetNextChar();
		}
		if (is_underscore == true) {
			if (token_ == "prog_line") {
				return QueryToken(PROG_LINE, token_);
			}
			else {
				return QueryToken(ERROR, token_);
			}
		}
		if (is_hash_present == true) {
			return QueryToken(HASH_IDENT, token_);
		}
		return QueryToken(IDENT, token_);
	}
	else if (char_type_ == DIGIT) {
		AddNextChar();
		GetNextChar();
		while (char_type_ == DIGIT) {
			AddNextChar();
			GetNextChar();
		}
		return QueryToken(INTEGER, token_);
	}
	else if (char_type_ == STAR) {
		AddNextChar();
		GetNextChar();
		return QueryToken(STAR, token_);
	}
	else if (char_type_ == SEMICOLON) {
		AddNextChar();
		GetNextChar();
		return QueryToken(SEMICOLON, token_);
	}
	else if (char_type_ == OPEN_BRACKET) {
		AddNextChar();
		GetNextChar();
		return QueryToken(OPEN_BRACKET, token_);
	}
	else if (char_type_ == CLOSE_BRACKET) {
		AddNextChar();
		GetNextChar();
		return QueryToken(CLOSE_BRACKET, token_);
	}
	else if (char_type_ == OPEN_ANGLE_BRACKET) {
		AddNextChar();
		GetNextChar();
		return QueryToken(OPEN_ANGLE_BRACKET, token_);
	}
	else if (char_type_ == CLOSE_ANGLE_BRACKET) {
		AddNextChar();
		GetNextChar();
		return QueryToken(CLOSE_ANGLE_BRACKET, token_);
	}
	else if (char_type_ == DOUBLE_QUOTE) {
		AddNextChar();
		GetNextChar();
		return QueryToken(DOUBLE_QUOTE, token_);
	}
	else if (char_type_ == UNDERSCORE) {
		AddNextChar();
		GetNextChar();
		return QueryToken(UNDERSCORE, token_);
	}
	else if (char_type_ == COMMA) {
		AddNextChar();
		GetNextChar();
		return QueryToken(COMMA, token_);
	}
	else if (char_type_ == DOT) {
		AddNextChar();
		GetNextChar();
		return QueryToken(DOT, token_);
	}
	else if (char_type_ == EQUAL) {
		AddNextChar();
		GetNextChar();
		return QueryToken(EQUAL, token_);
	}
	else if (char_type_ == PLUS) {
		AddNextChar();
		GetNextChar();
		return QueryToken(PLUS, token_);
	}
	else if (char_type_ == MINUS) {
		AddNextChar();
		GetNextChar();
		return QueryToken(MINUS, token_);
	}
	else {
		return QueryToken(ERROR, token_);
	}
}

void QueryTokenizer::GetNextChar() {
	// Process queryString character by character
	if (query_string_.length() > 0) {
		next_char_ = query_string_[0];
		query_string_.erase(0, 1);
	}
	else {
		next_char_ = '~';
	}
	char_type_ = ENDL;

	// If ascii numbers
	if (next_char_ > 47 && next_char_ < 58) {
		char_type_ = DIGIT;
	}
	// If ascii alphabets
	if ((next_char_ > 64 && next_char_ < 91) || (next_char_ > 96 && next_char_ < 123)) {
		char_type_ = LETTER;
	}
	switch (next_char_) {
		case '(': char_type_ = OPEN_BRACKET; break;
		case ')': char_type_ = CLOSE_BRACKET; break;
		case '<': char_type_ = OPEN_ANGLE_BRACKET; break;
		case '>': char_type_ = CLOSE_ANGLE_BRACKET; break;
		case '\"':char_type_ = DOUBLE_QUOTE; break;
		case '_': char_type_ = UNDERSCORE; break;
		case ';': char_type_ = SEMICOLON; break;
		case '*': char_type_ = STAR; break;
		case ' ': char_type_ = WHITESPACE; break;
		case '\t':char_type_ = TAB; break;
		case ',': char_type_ = COMMA; break;
		case '.': char_type_ = DOT; break;
		case '#': char_type_ = HASH; break;
		case '=': char_type_ = EQUAL; break;
		case '+': char_type_ = PLUS; break;
		case '-': char_type_ = MINUS; break;
		case '~': char_type_ = ENDL; break;
		default: break;
	}
}

void QueryTokenizer::AddNextChar() {
	token_ += next_char_;
}
