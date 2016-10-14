#include "QueryToken.h"

QueryToken::QueryToken() {

}

QueryToken::QueryToken(int token_type, string token_name) {
	token_type_ = token_type;
	token_name_ = token_name;
}

int QueryToken::GetTokenType() {
	return token_type_;
}

string QueryToken::GetTokenName() {
	return token_name_;
}