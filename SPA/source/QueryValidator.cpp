#include "QueryValidator.h"

QueryValidator::QueryValidator() {
	query_table_ = QueryTable(false);
}

QueryValidator::QueryValidator(string query) {
	query_string_ = query;
	tokenizer_ = QueryTokenizer(query);
	
}

QueryTable QueryValidator::Parse() {
	try {
		MatchDeclaration();
		MatchSelect();
	}
	catch (QueryException& e) {
		cout << e.what() << endl;
		return QueryTable(true);
	}
	// query_table_.Optimize();
	return query_table_;
}

QueryToken QueryValidator::GetToken() {
	return tokenizer_.Tokenize();
}

void QueryValidator::Match(string token) {
	if (next_token_.GetTokenName() == token) {
		next_token_ = GetToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token '" + next_token_.GetTokenName() + "; Expected token '" + token));
	}
}

void QueryValidator::Match(int token_type) {
	if (next_token_.GetTokenType() == token_type) {
		next_token_ = GetToken();
	}
	else {
		throw(QueryException("Invalid query: Unexpected token type'" + to_string(next_token_.GetTokenType()) + "; Expected tokenType '" + to_string(token_type)));
	}
}

void QueryValidator::MatchDeclaration() {
	next_token_ = GetToken();
	if (next_token_.GetTokenName() != "Select") {
		// Iteration 1: 'stmt' | 'assign' | 'while' | 'variable' | 'constant' | 'prog_line'
		// Iteration 2: 'calls' | 'if' | 'procedure'
		for (vector<string>::const_iterator it = DESIGN_ENTITIES.begin(); it != DESIGN_ENTITIES.end();) {
			if (next_token_.GetTokenName() == *it) {
				Match(*it);
				MatchDeclarationVar(*it);
				it = DESIGN_ENTITIES.begin();
			}
			else {
				it++;
			}
		}
	}
}

void QueryValidator::MatchDeclarationVar(string entity) {
	string synonym = next_token_.GetTokenName();

	// If declared variable name is a keyword, reject
	if (find(DESIGN_ENTITIES.begin(), DESIGN_ENTITIES.end(), synonym) != DESIGN_ENTITIES.end()) {
		throw(QueryException("Invalid Query : Declared synonym cannot be a design_entity keyword!"));
	}

	// Make sure synonym is IDENT, else invalid synonym name
	if (next_token_.GetTokenType() != IDENT) {
		throw(QueryException("Invalid Query : Unexpected synonym that begins with '" + next_token_.GetTokenName() + "'"));
	}

	// Check if synonym has already been declared
	if (syn_to_entity_map_[synonym] != "") {
		throw(QueryException("Invalid Query : Synonym '" + synonym + "' has been redeclared"));
	}

	// Update syn to entity map
	syn_to_entity_map_[synonym] = entity;
	Match(synonym);

	// Check if next token is end of declaration for this entity type. If not, get more
	if (next_token_.GetTokenType() != SEMICOLON) {
		Match(COMMA);
		MatchDeclarationVar(entity);
	}
	else {
		// End declaration for this entity type
		Match(SEMICOLON);
	}
	query_table_.SetSynEntityMap(syn_to_entity_map_);
}

void QueryValidator::MatchSelect() {
	Match("Select");
	MatchResultClause();
	// Match 'Such That' | 'Pattern' | 'And' | 'With' |

	while (next_token_.GetTokenName() == "such" || next_token_.GetTokenName() == "pattern" || next_token_.GetTokenName() == "with") {
		MatchClause();
	}
	if (next_token_.GetTokenName() != "") {
		throw (QueryException("Invalid Query! : Not in PQL Grammar: " + next_token_.GetTokenName()));
	}
}

void QueryValidator::MatchResultClause() {
	// "Select" result-cl (suchthat-cl | with-cl | pattern-cl 
	// result-cl : tuple | "BOOLEAN" // Iteration 2: result-cl : synonym | "BOOLEAN"
	// tuple : elem | "<" elem ("," elem)* ">"
	// elem : synonym | attrRef
	if (next_token_.GetTokenName() == "BOOLEAN") {
		vector<string> select_arg({ "BOOLEAN" });
		Clause select_clause("SELECT", select_arg, select_arg);
		query_table_.SetSelectClause(select_clause);
		Match("BOOLEAN");
	}
	else {
		MatchTupleResult();
	}
}

void QueryValidator::MatchTupleResult()
{
	if (next_token_.GetTokenType() == OPEN_ANGLE_BRACKET) {
		Match(OPEN_ANGLE_BRACKET);
		MatchResultClauseElement(true);
		Match(CLOSE_ANGLE_BRACKET);
	}
	else {
		MatchResultClauseElement(false);
	}

	Clause select_clause("select", select_arg_, select_arg_type_);
	query_table_.SetSelectClause(select_clause);
}

void QueryValidator::MatchResultClauseElement(bool is_tuple)
{
	string result_syn, result_syn_type;

	if (next_token_.GetTokenType() == IDENT) {
		result_syn = next_token_.GetTokenName();
		result_syn_type = syn_to_entity_map_[result_syn];
		if (syn_to_entity_map_.count(result_syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + result_syn + "' in Select clause"));
		}

		Match(result_syn);

		// Match tuple element here
		if (next_token_.GetTokenType() == DOT) {
			Match(DOT);
			string attribute_type = next_token_.GetTokenName();
			Match(attribute_type);
			if (IsAttributeMatchSynType(result_syn_type, attribute_type) == false) {
				throw(QueryException("Invalid Query : Unexpected attribute for synonym '" + attribute_type + "' in Select clause"));
			}
			else {
				if (result_syn_type == "call") {
					if (GetWithTypeByAttrName(attribute_type) == "name") {
						// Name
						result_syn_type = "call_name";
					}
					else {
						// Number
						result_syn_type = "call_number";
					}
				}
			}
		}

		select_arg_.push_back(result_syn);
		if (result_syn_type == "constant") {
			select_arg_type_.push_back("value");
		}
		else {
			select_arg_type_.push_back(result_syn_type);
		}
	}

	if ((is_tuple == true) && (next_token_.GetTokenType() == COMMA)) {
		Match(COMMA);
		MatchResultClauseElement(true);
	}
}

void QueryValidator::MatchClause() {
	if (next_token_.GetTokenName() == "such") {
		MatchSuchThat();
	}
	else if (next_token_.GetTokenName() == "pattern") {
		MatchPatternClause();
	}
	else if (next_token_.GetTokenName() == "with") {
		MatchWith();
	}
	else {
		// No SUCHTHAT, PATTERN, or WITH clause, query should be invalid
		throw(QueryException("Invalid Query: Unexpected token '" + next_token_.GetTokenName() + "'; Expecting 'SUCH THAT', 'PATTERN', or 'WITH'"));
	}
}

void QueryValidator::MatchSuchThat() {
	Match("such");
	Match("that");
	MatchRelation();
	while (next_token_.GetTokenName() == "and") {
		Match("and");
		MatchRelation();
	}
}

void QueryValidator::MatchPatternClause()
{
	Match("pattern");
	MatchPattern();
	while (next_token_.GetTokenName() == "and") {
		Match("and");
		MatchPattern();
	}
}

void QueryValidator::MatchPattern() { // TODO: Update Match for pattern-if and pattern-while
	string pattern_syn = next_token_.GetTokenName();
	string pattern_syn_type = syn_to_entity_map_[pattern_syn];
	if (syn_to_entity_map_.count(pattern_syn) == 0) {
		throw(QueryException("Invalid Query : Unexpected synonym '" + pattern_syn + "' in Pattern clause"));
	}

	if (pattern_syn_type == "assign") {
		MatchPatternAssign();
	}
	else if (pattern_syn_type == "while") {
		MatchPatternWhile();
	}
	else if (pattern_syn_type == "if") {
		MatchPatternIf();
	}
	else {
		throw(QueryException("Invalid Query : Unexpected synonym type '" + pattern_syn_type + "' in Pattern clause"));
	}
}

void QueryValidator::MatchWith() {
	Match("with");
	MatchAttrCompare();
	while (next_token_.GetTokenName() == "and") {
		Match("and");
		MatchAttrCompare();
	}
}

void QueryValidator::MatchRelation() {
	string relation = next_token_.GetTokenName();
	next_token_ = GetToken();
	if (next_token_.GetTokenType() == STAR) {
		relation += "*";
		next_token_ = GetToken();
	}
	if (relation == "Follows") {
		MatchFollow();
	}
	else if (relation == "Follows*") {
		MatchFollowStar();
	}
	else if (relation == "Parent") {
		MatchParent();
	}
	else if (relation == "Parent*") {
		MatchParentStar();
	}
	else if (relation == "Modifies") {
		MatchModifies();
	}
	else if (relation == "Uses") {
		MatchUses();
	}
	else if (relation == "Calls") {
		MatchCalls();
	}
	else if (relation == "Calls*") {
		MatchCallsStar();
	}
	else if (relation == "Next") {
		MatchNext();
	}
	else if (relation == "Next*") {
		MatchNextStar();
	}
	else if (relation == "Affects") {
		MatchAffects();
	}
	else if (relation == "Affects*") {
		MatchAffectsStar();
	}
	else {
		// If its none of the above relation, ERROR!!!!!!!!!!!!!
		throw(QueryException("Invalid Query : Unexpected relation '" + next_token_.GetTokenName() + "'; Expecting 'Follow'|'Follow*'|'Parent'|'Parent*'|'Modifies'|'Uses'"));
	}
}

void QueryValidator::MatchFollow() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);
	// Validate arg1 and arg2
	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("follows", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("follows", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("follows", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("follows", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("follows", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("follows", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> follows_arg({ arg1.second,arg2.second });
		vector<string> folllows_arg_type({ arg1_type,arg2_type });
		Clause follows_clause("follows", follows_arg, folllows_arg_type, GetClausePriority("follows"));
		query_table_.AddSuchThatClause(follows_clause);

	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for follows"));
	}

}

void QueryValidator::MatchFollowStar() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("follows*", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("follows*", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("follows*", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("follows*", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("follows*", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("follows*", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> follows_star_arg({ arg1.second,arg2.second });
		vector<string> follows_star_arg_type({ arg1_type,arg2_type });
		Clause follows_star_clause("follows*", follows_star_arg, follows_star_arg_type, GetClausePriority("follows*"));
		query_table_.AddSuchThatClause(follows_star_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for follows*"));
	}
}

void QueryValidator::MatchParent() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("parent", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("parent", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("parent", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("parent", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("parent", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("parent", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> parent_arg({ arg1.second,arg2.second });
		vector<string> parent_arg_type({ arg1_type,arg2_type });
		Clause parent_clause("parent", parent_arg, parent_arg_type, GetClausePriority("parent"));
		query_table_.AddSuchThatClause(parent_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent"));
	}
}

void QueryValidator::MatchParentStar() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("parent*", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("parent*", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("parent*", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("parent*", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("parent*", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("parent*", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> parent_star_arg({ arg1.second,arg2.second });
		vector<string> parent_star_arg_type({ arg1_type,arg2_type });
		Clause parent_star_clause("parent*", parent_star_arg, parent_star_arg_type, GetClausePriority("parent*"));
		query_table_.AddSuchThatClause(parent_star_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for parent*"));
	}
}
void QueryValidator::MatchModifies() {
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchEntRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchEntRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("modifies", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("modifies", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("modifies", "constant");
		arg1_type = "constant";
	}
	else if (arg1.first == STRING) {
		is_arg1_valid = rel_table_.IsArg1Valid("modifies", "string");
		arg1_type = "string";
	}

	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("modifies", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("modifies", "_");
		arg2_type = "any";
	}
	else if (arg2.first == STRING) {
		is_arg2_valid = rel_table_.IsArg2Valid("modifies", "string");
		arg2_type = "string";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> modifies_arg({ arg1.second,arg2.second });
		vector<string> modifies_arg_type({ arg1_type,arg2_type });
		Clause modifies_clause("modifies", modifies_arg, modifies_arg_type, GetClausePriority("modifies"));
		query_table_.AddSuchThatClause(modifies_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for modifies"));
	}
}

void QueryValidator::MatchUses() {
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchEntRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchEntRef();
	Match(CLOSE_BRACKET);

	// Validate arg1 and arg2
	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("uses", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("uses", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("uses", "constant");
		arg1_type = "constant";
	}
	else if (arg1.first == STRING) {
		is_arg1_valid = rel_table_.IsArg1Valid("uses", "string");
		arg1_type = "string";
	}

	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("uses", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("uses", "_");
		arg2_type = "any";
	}
	else if (arg2.first == STRING) {
		is_arg2_valid = rel_table_.IsArg2Valid("uses", "string");
		arg2_type = "string";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> uses_arg({ arg1.second,arg2.second });
		vector<string> uses_arg_type({ arg1_type,arg2_type });
		Clause uses_clause("uses", uses_arg, uses_arg_type, GetClausePriority("uses"));
		query_table_.AddSuchThatClause(uses_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for uses"));
	}
}

void QueryValidator::MatchCalls() {
	// Calls:"Calls""("entRef", "entRef")" 
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchEntRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchEntRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("calls", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("calls", "_");
		arg1_type = "any";
	}
	else if (arg1.first == STRING) {
		is_arg1_valid = rel_table_.IsArg1Valid("calls", "string");
		arg1_type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("calls", "constant");
		arg1_type = "constant";
	}
	*/
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("calls", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("calls", "_");
		arg2_type = "any";
	}
	else if (arg2.first == STRING) {
		is_arg2_valid = rel_table_.IsArg2Valid("calls", "string");
		arg2_type = "string";
	}
	/* call rel does not accept int as parameter
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("calls", "constant");
		arg2_type = "constant";
	}
	*/
	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> calls_arg({ arg1.second,arg2.second });
		vector<string> calls_arg_type({ arg1_type,arg2_type });
		Clause calls_clause("calls", calls_arg, calls_arg_type, GetClausePriority("call"));
		query_table_.AddSuchThatClause(calls_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for calls"));
	}
}

void QueryValidator::MatchCallsStar() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchEntRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchEntRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("calls*", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("calls*", "_");
		arg1_type = "any";
	}
	else if (arg1.first == STRING) {
		is_arg1_valid = rel_table_.IsArg1Valid("calls*", "string");
		arg1_type = "string";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("calls*", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("calls*", "_");
		arg2_type = "any";
	}
	else if (arg2.first == STRING) {
		is_arg2_valid = rel_table_.IsArg2Valid("calls*", "string");
		arg2_type = "string";
	}
	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> callsStarArg({ arg1.second,arg2.second });
		vector<string> callsStarArgType({ arg1_type,arg2_type });
		Clause callsStarRel("calls*", callsStarArg, callsStarArgType,GetClausePriority("call*"));
		query_table_.AddSuchThatClause(callsStarRel);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for calls*"));
	}
}

void QueryValidator::MatchNext() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("next", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("next", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("next", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("next", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("next", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("next", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> next_arg({ arg1.second,arg2.second });
		vector<string> next_arg_type({ arg1_type,arg2_type });
		Clause next_clause("next", next_arg, next_arg_type,GetClausePriority("next"));
		query_table_.AddSuchThatClause(next_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for next"));
	}
}

void QueryValidator::MatchNextStar() {
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("next*", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("next*", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("next*", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("next*", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("next*", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("next*", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> next_star_arg({ arg1.second,arg2.second });
		vector<string> next_star_arg_type({ arg1_type,arg2_type });
		Clause next_star_clause("next*", next_star_arg, next_star_arg_type,GetClausePriority("next*"));
		query_table_.AddSuchThatClause(next_star_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for nextStar"));
	}
}
void QueryValidator::MatchAffects()
{
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("affects", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("affects", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("affects", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("affects", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("affects", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("affects", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> affects_arg({ arg1.second,arg2.second });
		vector<string> affects_arg_type({ arg1_type,arg2_type });
		Clause affects_clause("affects", affects_arg, affects_arg_type, GetClausePriority("affects"));
		query_table_.AddSuchThatClause(affects_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for Affects"));
	}
}
void QueryValidator::MatchAffectsStar()
{
	// Check if arguments, num of arguments are valid
	Match(OPEN_BRACKET);
	pair<int, string> arg1 = MatchStmtRef();
	Match(COMMA);
	pair<int, string> arg2 = MatchStmtRef();
	Match(CLOSE_BRACKET);

	bool is_arg1_valid = false;
	bool is_arg2_valid = false;
	string arg1_type, arg2_type;

	if (arg1.first == IDENT) {
		if (syn_to_entity_map_[arg1.second] != "") {
			is_arg1_valid = rel_table_.IsArg1Valid("affects*", syn_to_entity_map_[arg1.second]);
			arg1_type = syn_to_entity_map_[arg1.second];
		}
	}
	else if (arg1.first == UNDERSCORE) {
		is_arg1_valid = rel_table_.IsArg1Valid("affects*", "_");
		arg1_type = "any";
	}
	else if (arg1.first == INTEGER) {
		is_arg1_valid = rel_table_.IsArg1Valid("affects*", "constant");
		arg1_type = "constant";
	}
	if (arg2.first == IDENT) {
		if (syn_to_entity_map_[arg2.second] != "") {
			is_arg2_valid = rel_table_.IsArg2Valid("affects*", syn_to_entity_map_[arg2.second]);
			arg2_type = syn_to_entity_map_[arg2.second];
		}
	}
	else if (arg2.first == UNDERSCORE) {
		is_arg2_valid = rel_table_.IsArg2Valid("affects*", "_");
		arg2_type = "any";
	}
	else if (arg2.first == INTEGER) {
		is_arg2_valid = rel_table_.IsArg2Valid("affects*", "constant");
		arg2_type = "constant";
	}

	if (is_arg1_valid == true && is_arg2_valid == true) {
		vector<string> affects_star_arg({ arg1.second,arg2.second });
		vector<string> affects_star_arg_type({ arg1_type,arg2_type });
		Clause affects_star_clause("affects*", affects_star_arg, affects_star_arg_type, GetClausePriority("affects*"));
		query_table_.AddSuchThatClause(affects_star_clause);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected arguments for Affects*"));
	}
}
void QueryValidator::MatchPatternAssign()
{
	string assign_syn = next_token_.GetTokenName();
	string assign_var_ref_syn, assign_var_ref_type, expression_type;
	pair<int, string> assign_var_ref;
	bool is_sub_expr = false;
	bool is_any = false;
	bool is_assign_var_ref_valid = false;

	if (syn_to_entity_map_[assign_syn] == "assign") {
		Match(assign_syn);
		Match(OPEN_BRACKET);
		assign_var_ref = MatchVarRef();
		Match(COMMA);
		if (next_token_.GetTokenType() == UNDERSCORE) {
			Match(UNDERSCORE);
			is_any = true;
			expression_string_ = "_";
			if (next_token_.GetTokenType() == DOUBLE_QUOTE) {
				is_sub_expr = true;
				is_any = false;
				Match(DOUBLE_QUOTE);
				expression_string_ = "";
				MatchExpr();
				Match(DOUBLE_QUOTE);
				Match(UNDERSCORE);
			}
		}
		else if (next_token_.GetTokenType() == DOUBLE_QUOTE) {
			Match(DOUBLE_QUOTE);
			expression_string_ = "";
			MatchExpr();
			Match(DOUBLE_QUOTE);
		}
		Match(CLOSE_BRACKET);

		if (assign_var_ref.first == IDENT) {
			if (syn_to_entity_map_[assign_var_ref.second] != "") {
				assign_var_ref_type = syn_to_entity_map_[assign_var_ref.second];
				is_assign_var_ref_valid = rel_table_.IsArg1Valid("patternAssign", assign_var_ref_type);
			}
		}
		else if (assign_var_ref.first == UNDERSCORE) {
			is_assign_var_ref_valid = true;
			assign_var_ref_type = "any";
		}
		else if (assign_var_ref.first == STRING) {
			is_assign_var_ref_valid = true;
			assign_var_ref_type = "string";
		}

		if (is_assign_var_ref_valid == true) {
			if (is_sub_expr == true) {
				expression_type = "sub_expr";
			}
			else if (is_any == true) {
				expression_type = "any";
			}
			else {
				expression_type = "expr";
			}
			vector<string> assign_arg = { assign_syn, assign_var_ref.second , expression_string_ };
			vector<string> assign_arg_type = { "assign", assign_var_ref_type, expression_type };
			Clause pattern_assign_clause("pattern", assign_arg, assign_arg_type, GetClausePriority("pattern"));
			query_table_.AddPatternClause(pattern_assign_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected varRef for pattern-assign '" + assign_var_ref_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected valid assign synonym"));
	}
}

void QueryValidator::MatchPatternWhile() {
	// while : synonym (varRef ,"_")
	// varRef : synonym | "_" | "IDENT"
	string while_syn = next_token_.GetTokenName();
	string while_control_var_type;
	bool is_control_variable_valid = false;
	pair<int, string> while_control_variable;
	if (syn_to_entity_map_[while_syn] == "while") {
		Match(while_syn);
		Match(OPEN_BRACKET);
		while_control_variable = MatchVarRef();
		Match(COMMA);
		Match(UNDERSCORE);
		Match(CLOSE_BRACKET);

		if (while_control_variable.first == IDENT) {
			if (syn_to_entity_map_[while_control_variable.second] != "") {
				while_control_var_type = syn_to_entity_map_[while_control_variable.second];
				is_control_variable_valid = rel_table_.IsArg1Valid("patternWhile", while_control_var_type);
			}
		}
		else if (while_control_variable.first == UNDERSCORE) {
			is_control_variable_valid = true;
			while_control_var_type = "any";
		}
		else if (while_control_variable.first == STRING) {
			is_control_variable_valid = true;
			while_control_var_type = "string";
		}

		if (is_control_variable_valid == true) {
			vector<string> while_arg = { while_syn, while_control_variable.second };
			vector<string> while_arg_type = { "while", while_control_var_type };
			Clause pattern_while_clause("pattern", while_arg, while_arg_type, GetClausePriority("pattern"));
			query_table_.AddPatternClause(pattern_while_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected control variable for pattern-while '" + while_control_var_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected valid while synonym"));
	}
}

void QueryValidator::MatchPatternIf() {
	// if : synonym (varRef, "_", "_")
	// varRef : synonym | "_" | "IDENT"
	string if_syn = next_token_.GetTokenName();
	string if_control_var_type;
	bool is_control_variable_valid = false;
	pair<int, string> if_control_variable;
	if (syn_to_entity_map_[if_syn] == "if") {
		Match(if_syn);
		Match(OPEN_BRACKET);
		if_control_variable = MatchVarRef();
		Match(COMMA);
		Match(UNDERSCORE);
		Match(COMMA);
		Match(UNDERSCORE);
		Match(CLOSE_BRACKET);

		if (if_control_variable.first == IDENT) {
			if (syn_to_entity_map_[if_control_variable.second] != "") {
				if_control_var_type = syn_to_entity_map_[if_control_variable.second];
				is_control_variable_valid = rel_table_.IsArg1Valid("patternIf", if_control_var_type);
			}
		}
		else if (if_control_variable.first == UNDERSCORE) {
			is_control_variable_valid = true;
			if_control_var_type = "any";
		}
		else if (if_control_variable.first == STRING) {
			is_control_variable_valid = true;
			if_control_var_type = "string";
		}

		if (is_control_variable_valid == true) {
			vector<string> if_arg = { if_syn, if_control_variable.second };
			vector<string> if_arg_type = { "if", if_control_var_type };
			Clause pattern_if_clause("pattern", if_arg, if_arg_type, GetClausePriority("pattern"));
			query_table_.AddPatternClause(pattern_if_clause);
		}
		else {
			throw(QueryException("Invalid Query : Unexpected control variable for pattern-if '" + if_control_var_type + "'"));
		}
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected valid if synonym"));
	}
}

void QueryValidator::MatchAttrCompare()
{
	vector<string> with_arg, with_arg_type;
	Ref left_ref = MatchRef();
	Match(EQUAL);
	Ref right_ref = MatchRef();

	if (IsRefCompatible(left_ref, right_ref) == true) {
		with_arg.push_back(left_ref.value);
		with_arg.push_back(right_ref.value);
		with_arg_type.push_back(left_ref.arg_type);
		with_arg_type.push_back(right_ref.arg_type);
		Clause with_clause("with", with_arg, with_arg_type, GetClausePriority("with"));
		query_table_.AddWithClause(with_clause);
	}
	else {
		throw(QueryException("Invalid Query : MisMatched Ref '" + left_ref.value + "' and '" + right_ref.value + "' in Pattern clause"));
	}
}
QueryValidator::Ref QueryValidator::MatchRef()
{
	// ref : attrRef | synonyn | IDENT | INTEGER
	// attrRef : synonym.attrName
	string with_syn, with_syn_type, argument;
	int token_type;

	if (next_token_.GetTokenType() == IDENT) {
		with_syn = next_token_.GetTokenName();
		with_syn_type = syn_to_entity_map_[with_syn];
		if (syn_to_entity_map_.count(with_syn) == 0) {
			throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
		}
		Match(with_syn);
		token_type = next_token_.GetTokenType();
		if (next_token_.GetTokenType() != DOT) {
			if (with_syn_type != "prog_line") {
				throw(QueryException("Invalid Query : Unexpected synonym '" + with_syn + "' in with clause"));
			}
			else {
				Ref with_ref;
				with_ref.value = with_syn;
				with_ref.arg_type = "prog_line";
				with_ref.with_type = "number";
				return with_ref;
			}
		}
		Match(DOT);
		if (next_token_.GetTokenType() == IDENT || next_token_.GetTokenType() == HASH_IDENT) {
			string attrName = next_token_.GetTokenName();
			int attrType = next_token_.GetTokenType();
			if (IsAttributeMatchSynType(with_syn_type, attrName) == false) {
				throw(QueryException("Invalid Query : Unexpected attribute for '" + with_syn + "' in with clause"));
			}
			Match(attrName);
			Ref with_ref;
			with_ref.value = with_syn;
			if (with_syn_type == "call") {
				if (attrName == "procName") {
					with_ref.arg_type = "call_name";
					with_ref.with_type = "name";
					return with_ref;
				}
				else if (attrName == "stmt#") {
					with_ref.arg_type = "call_number";
					with_ref.with_type = "number";
					return with_ref;
				}
			}

			if (IsAttributeMatchSynType(with_syn_type, attrName) == true) {
				with_ref.arg_type = syn_to_entity_map_[with_syn];
				if (with_syn_type == "constant") {
					with_ref.arg_type = "value";
					with_ref.with_type = "number";
				}
				else {
					with_ref.with_type = GetWithTypeByAttrName(attrName);
				}
				return with_ref;
			}
			else {
				throw(QueryException("Invalid Query : Unexpected attibute '" + attrName + "' for synonym :'" + with_syn + "'"));
			}
		}
		else {
			throw(QueryException("Invalid Query : Unexpected attibute '" + next_token_.GetTokenName() + "' in with clause"));
		}
	}
	else if (next_token_.GetTokenType() == DOUBLE_QUOTE) {
		// '"' IDENT '"'
		Match(DOUBLE_QUOTE);
		argument = next_token_.GetTokenName();
		token_type = STRING;
		Match(argument);
		Match(DOUBLE_QUOTE);
		Ref with_ref;
		with_ref.value = argument;
		with_ref.arg_type = "string";
		with_ref.with_type = "name";
		return with_ref;
	}
	else if (next_token_.GetTokenType() == INTEGER) {
		argument = next_token_.GetTokenName();
		token_type = next_token_.GetTokenType();
		Match(argument);
		Ref with_ref;
		with_ref.value = argument;
		with_ref.arg_type = "number";
		with_ref.with_type = "number";
		return with_ref;
		return with_ref;
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected Ref token"));
	}
}

pair<int, string> QueryValidator::MatchStmtRef() {
	// stmtRef : synonym | �_� | INTEGER
	int tokenType;
	string argument;
	if (next_token_.GetTokenType() == IDENT) {
		argument = next_token_.GetTokenName();
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else if (next_token_.GetTokenName() == "_") {
		argument = "_";
		tokenType = next_token_.GetTokenType();
		Match(UNDERSCORE);
	}
	else if (next_token_.GetTokenType() == INTEGER) {
		argument = next_token_.GetTokenName();
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else {
		// Invalid Query
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected stmtRef token"));
	}
	return pair<int, string>(tokenType, argument);
}

pair<int, string> QueryValidator::MatchEntRef() {
	// entRef : synonym | �_� | �"� IDENT �"� | INTEGER
	string argument;
	int tokenType;
	if (next_token_.GetTokenType() == IDENT) {
		// synonym
		argument = next_token_.GetTokenName();
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else if (next_token_.GetTokenName() == "_") {
		// '_'
		argument = "_";
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else if (next_token_.GetTokenName() == "\"") {
		// '"' IDENT '"'
		Match(DOUBLE_QUOTE);
		argument = next_token_.GetTokenName();
		tokenType = STRING;
		Match(argument);
		Match(DOUBLE_QUOTE);
	}
	else if (next_token_.GetTokenType() == INTEGER) {
		argument = next_token_.GetTokenName();
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected entRef token"));
	}

	return pair<int, string>(tokenType, argument);
}

pair<int, string> QueryValidator::MatchVarRef()
{
	// entRef : synonym | �_� | �"� IDENT �"�
	string argument;
	int tokenType;
	if (next_token_.GetTokenType() == IDENT) {
		// synonym
		argument = next_token_.GetTokenName();
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else if (next_token_.GetTokenName() == "_") {
		// '_'
		argument = "_";
		tokenType = next_token_.GetTokenType();
		Match(argument);
	}
	else if (next_token_.GetTokenName() == "\"") {
		// '"' IDENT '"'
		Match(DOUBLE_QUOTE);
		argument = next_token_.GetTokenName();
		tokenType = STRING;
		Match(argument);
		Match(DOUBLE_QUOTE);
	}
	else {
		throw(QueryException("Invalid Query : Unexpected token '" + next_token_.GetTokenName() + "'; Expected entRef token"));
	}

	return pair<int, string>(tokenType, argument);
}

void QueryValidator::MatchExpr() {
	if (next_token_.GetTokenType() == PLUS || next_token_.GetTokenType() == MINUS) {
		expression_string_ += next_token_.GetTokenName();
		Match(next_token_.GetTokenName());
	}
	MatchTerm();
	while (next_token_.GetTokenType() == PLUS || next_token_.GetTokenType() == MINUS) {
		expression_string_ += next_token_.GetTokenName();
		Match(next_token_.GetTokenName());
		MatchTerm();
	}
}

void QueryValidator::MatchTerm() {
	MatchFactor();
	while (next_token_.GetTokenType() == STAR) {
		expression_string_ += next_token_.GetTokenName();
		Match(next_token_.GetTokenName());
		MatchFactor();
	}
}

void QueryValidator::MatchFactor() {
	if (next_token_.GetTokenType() == IDENT) {
		expression_string_ += next_token_.GetTokenName();
		Match(next_token_.GetTokenName());
	}
	else if (next_token_.GetTokenType() == INTEGER) {
		expression_string_ += next_token_.GetTokenName();
		Match(next_token_.GetTokenName());
	}
	else if (next_token_.GetTokenType() == OPEN_BRACKET) {
		expression_string_ += next_token_.GetTokenName();
		Match(OPEN_BRACKET);
		MatchExpr();
		expression_string_ += next_token_.GetTokenName();
		Match(CLOSE_BRACKET);
	}
	else {
		throw(QueryException("Invalid expression!"));
	}
}
bool QueryValidator::IsAttributeMatchSynType(string synType, string attrName) {
	if (((synType == "procedure") || (synType == "call")) && (attrName == "procName")) {
		return true;
	}
	else if ((synType == "variable") && (attrName == "varName")) {
		return true;
	}
	else if ((synType == "constant") && (attrName == "value")) {
		return true;
	}
	else if ((synType == "stmt") || (synType == "assign") || (synType == "while") || (synType == "if") || (synType == "call")){
		if (attrName == "stmt#") {
			return true;
		}
	}
	return false;
}

bool QueryValidator::IsRefCompatible(Ref left_ref, Ref right_ref)
{
	if (left_ref.with_type == "number" && right_ref.with_type == "number") {
		return true;
	}
	else if (left_ref.with_type == "name" && right_ref.with_type == "name") {
		return true;
	}
	return false;
}

string QueryValidator::GetWithTypeByAttrName(string attrName)
{
	if (attrName == "varName" || attrName == "procName") {
		return "name";
	}
	else if (attrName == "value" || attrName == "stmt#") {
		return "number";
	}
	else {
		return "";
	}
}

int QueryValidator::GetClausePriority(string relation)
{
	if (relation == "follows") {
		return 1;
	}
	else if (relation == "follows*") {
		return 1;
	}
	else if (relation == "parent") {
		return 1;
	}
	else if (relation == "parent*") {
		return 2;
	}
	else if (relation == "next") {
		return 1;
	}
	else if (relation == "next*") {
		return 2;
	}
	else if (relation == "call") {
		return 1;
	}
	else if (relation == "call*") {
		return 2;
	}
	else if (relation == "affects") {
		return 20;
	}
	else if (relation == "affects*") {
		return 40;
	}
	else if (relation == "modifies") {
		return 2;
	}
	else if (relation == "uses") {
		return 2;
	}
	else if (relation == "pattern") {
		return 2;
	}
	else if (relation == "with") {
		return 0;
	}
	else {
		return -1;
	}
}
