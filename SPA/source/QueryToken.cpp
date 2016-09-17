#include "QueryToken.h"

QueryToken::QueryToken() {

}

QueryToken::QueryToken(int tokenType, string tokenName) {
	_tokenType = tokenType;
	_tokenName = tokenName;
}

int QueryToken::getTokenType() {
	return _tokenType;
}

string QueryToken::getTokenName() {
	return _tokenName;
}