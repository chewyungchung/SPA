#include "QueryEvaluator.h"

QueryEvaluator::QueryEvaluator() {
}

QueryEvaluator::QueryEvaluator(QueryTable input_query, PKB pkb) {
	input_query_ = input_query;
	pkb_ = pkb;
}

list<string> QueryEvaluator::Evaluate() {
	list<string> final_results;

	if (input_query_.IsNullQuery() == true) {
		return final_results;
	}

	if (ProcessNoSynGroup() == false) {
		if (IsBooleanSelected() == true) {
			final_results.push_back("false");
		}
		return final_results;
	} 

	if (ProcessNonConnectedGroups() == false) {
		if (IsBooleanSelected() == true) {
			final_results.push_back("false");
		}
		return final_results;
	}

	if (ProcessConnectedGroups() == false) {
		if (IsBooleanSelected() == true) {
			final_results.push_back("false");
		}
		return final_results;
	}

	QueryResultProjector output_manager(connected_group_intermediate_result_, non_connected_group_intermediate_result_, input_query_.GetSelectClause());
	final_results = output_manager.GetResults();
	return final_results;
}

ResultTable QueryEvaluator::ProcessSubstitute(Clause substitute_clause)
{
	string substitute_arg = substitute_clause.GetArg().at(0);
	string substitute_arg_type = substitute_clause.GetArgType().at(0);
	ResultTable temp_result = ResultTable(substitute_arg);
	list<int> data_list;
	vector<string> temp_row_data;

	if (substitute_arg_type == ARGTYPE_PROCEDURE) {
		list<string> procedure_list = pkb_.getProcedureList();
		if (procedure_list.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &procedure : procedure_list) {
				temp_row_data.push_back(procedure);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else if (substitute_arg_type == ARGTYPE_VARIABLE) {
		list<string> variable_list = pkb_.getVarList();
		if (variable_list.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &variable : variable_list) {
				temp_row_data.push_back(variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else {
		data_list = GetList(substitute_arg_type);
		if (data_list.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &data : data_list) {
				temp_row_data.push_back(to_string(data));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
}

ResultTable QueryEvaluator::ProcessClause(Clause input_clause)
{
	string relation = input_clause.GetRelation();
	if (relation == REL_SUBSTITUTE) {
		return ProcessSubstitute(input_clause);
	}
	else if (relation == REL_PATTERN) {
		return ProcessPattern(input_clause);
	}
	else if (relation == REL_WITH) {
		return ProcessWith(input_clause);
	}
	else {
		return ProcessSuchThat(input_clause);
	}
}

ResultTable QueryEvaluator::ProcessSuchThat(Clause such_that_clause) {
	string relation = such_that_clause.GetRelation();
	ResultTable temp_result;

	if (relation == REL_FOLLOWS) {
		temp_result = ProcessFollows(such_that_clause);
	}
	else if (relation == REL_FOLLOWS_STAR) {
		temp_result = ProcessFollowsT(such_that_clause);
	}
	else if (relation == REL_PARENT) {
		temp_result = ProcessParent(such_that_clause);
	}
	else if (relation == REL_PARENT_STAR) {
		temp_result = ProcessParentT(such_that_clause);
	}
	else if (relation == REL_MODIFIES) {
		temp_result = ProcessModifies(such_that_clause);
	}
	else if (relation == REL_USES) {
		temp_result = ProcessUses(such_that_clause);
	}
	else if (relation == REL_NEXT) {
		temp_result = ProcessNext(such_that_clause);
	}
	else if (relation == REL_NEXT_STAR) {
		temp_result = ProcessNextT(such_that_clause);
	}
	else if (relation == REL_CALLS) {
		temp_result = ProcessCalls(such_that_clause);
	}
	else if (relation == REL_CALLS_STAR) {
		temp_result = ProcessCallsStar(such_that_clause);
	}
	else if (relation == REL_PATTERN) {
		temp_result = ProcessPattern(such_that_clause);
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessPattern(Clause pattern_clause) {
	ResultTable temp_result;
	string pattern_syn_type = pattern_clause.GetArgType().at(0);
	if (pattern_syn_type == ARGTYPE_ASSIGN) {
		temp_result = ProcessPatternAssign(pattern_clause);
	}
	else if (pattern_syn_type == ARGTYPE_WHILE) {
		temp_result = ProcessPatternWhile(pattern_clause);
	}
	else if (pattern_syn_type == ARGTYPE_IF) {
		temp_result = ProcessPatternIf(pattern_clause);
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessFollows(Clause follows_clause) {
	string arg1 = follows_clause.GetArg().at(0);
	string arg2 = follows_clause.GetArg().at(1);
	string arg1_type = follows_clause.GetArgType().at(0);
	string arg2_type = follows_clause.GetArgType().at(1);
	
	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		if (pkb_.getFollower(arg1_stmt_num) == -1) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);

			if (arg2_stmt_num <= arg1_stmt_num) {
				return temp_result;
			}

			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (pkb_.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}
			
			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// At this point, arg1 determined to have follower from the first getFollower() check for. Hence, return QueryResult set to 1
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (pkb_.getFollowedFrom(arg2_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isFollowEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.getFollowedFrom(arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.getFollower(arg1_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case: Follows(s,s)
			if (arg2 == arg1) {
				return temp_result;
			}
			
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (pkb_.isValidFollows(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						if (arg2_type == ARGTYPE_CALLS) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
						break;
					}
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessFollowsT(Clause follow_star_clause) {
	string arg1 = follow_star_clause.GetArg().at(0);
	string arg2 = follow_star_clause.GetArg().at(1);
	string arg1_type = follow_star_clause.GetArgType().at(0);
	string arg2_type = follow_star_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_follower_star = pkb_.getFollowerStar(arg1_stmt_num);
		if (arg1_follower_star.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg1_follower_star.begin(), arg1_follower_star.end(), arg2_stmt_num) != arg1_follower_star.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isFollowsStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> arg2_followed_from_star = pkb_.getFollowedFromStar(arg2_stmt_num);
			if (arg2_followed_from_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isFollowEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> arg2_followed_from_star = pkb_.getFollowedFromStar(arg2_stmt_num);
				if (arg2_followed_from_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if(arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isFollowsStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_follower_star = pkb_.getFollowerStar(arg1_stmt_num);
				if (arg1_follower_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case FollowStar*(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (pkb_.isFollowsStar(arg1_stmt_num, arg2_stmt_num)) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						if (arg2_type == ARGTYPE_CALLS) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessParent(Clause parent_clause) {
	string arg1 = parent_clause.GetArg().at(0);
	string arg2 = parent_clause.GetArg().at(1);
	string arg1_type = parent_clause.GetArgType().at(0);
	string arg2_type = parent_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT)
	{
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_children = pkb_.getChildrenOf(arg1_stmt_num);
		if (arg1_children.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (pkb_.isParentOf(arg1_stmt_num, arg2_stmt_num)) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// Query is true since arg1 has children
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (pkb_.getParentOf(arg2_stmt_num) != -1) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// If ParentTable is non empty (with relationships), existential query
			if (pkb_.isParentEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.getParentOf(arg2_stmt_num) != -1) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else 
	{
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
					break;
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_children = pkb_.getChildrenOf(arg1_stmt_num);
				if (arg1_children.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else
		{
			// Argument 2 is a synonym			
			// Corner case. Parent(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (pkb_.isParentOf(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						if (arg2_type == ARGTYPE_CALLS) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessParentT(Clause parent_star_clause) {
	string arg1 = parent_star_clause.GetArg().at(0);
	string arg2 = parent_star_clause.GetArg().at(1);
	string arg1_type = parent_star_clause.GetArgType().at(0);
	string arg2_type = parent_star_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		list<int> arg1_children_star = pkb_.getChildStarOf(arg1_stmt_num);
		if (arg1_children_star.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg1_children_star.begin(), arg1_children_star.end(), arg2_stmt_num) != arg1_children_star.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			temp_result.SetIsQueryTrue(true);
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> arg2_parent_star = pkb_.getParentStar(arg2_stmt_num);
			if (arg2_parent_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isParentEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> arg2_parent_star = pkb_.getParentStar(arg2_stmt_num);
				if (arg2_parent_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_children_star = pkb_.getChildStarOf(arg1_stmt_num);
				if (arg1_children_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case Parent*(s,s)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (pkb_.isParentStar(arg1_stmt_num, arg2_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						if (arg2_type == ARGTYPE_CALLS) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessModifies(Clause modifies_clause) {
	string arg1 = modifies_clause.GetArg().at(0);
	string arg2 = modifies_clause.GetArg().at(1);
	string arg1_type = modifies_clause.GetArgType().at(0);
	string arg2_type = modifies_clause.GetArgType().at(1);
	
	ResultTable temp_result;

	if (arg2_type == ARGTYPE_STRING) {
		if (pkb_.isValidVar(arg2) == false) {
			return temp_result;
		}
		list<int> arg2_modified_by = pkb_.getModifiedBy(arg2);
		if (arg2_modified_by.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}


			if (find(arg2_modified_by.begin(), arg2_modified_by.end(), arg1_stmt_num) != arg2_modified_by.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			if (pkb_.isModifiedByProc(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				if (pkb_.isModifiedByProc(arg1_procedure, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}
			
			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isModified(arg1_stmt_num, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg2_type == ARGTYPE_VARIABLE) {
		// Argument 2 is a variable synonym
		list<string> all_variable_list = pkb_.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg2);
		vector<string> temp_row_data;

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}
			
			// Check if arg1 modifies anything
			list<string> arg1_modified_variables = pkb_.getModifiedBy(arg1_stmt_num);
			if (arg1_modified_variables.empty() == true) {
				return temp_result;
			}

			// Query is 1 since arg1_modified_variables is non empty
			temp_result.SetIsQueryTrue(true);

			for (auto &arg2_variable : arg1_modified_variables) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_modifies_variable = pkb_.getModifiedByProc(arg1);
			if (arg1_procedure_modifies_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg2_variable : arg1_procedure_modifies_variable) {
					temp_row_data.push_back(arg2_variable);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1, arg2);
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_modifies_variable = pkb_.getModifiedByProc(arg1_procedure);
				if (arg1_procedure_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_procedure_modifies_variable) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_modifies_variable = pkb_.getModifiedBy(arg1_stmt_num);
				if (arg1_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_modifies_variable) {
						string arg1_argument = to_string(arg1_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	else {
		// arg2_type is any ('_'), explicitly is a variable
		list<string> all_variable_list = pkb_.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			list<string> arg1_modifies = pkb_.getModifiedBy(stoi(arg1));
			if (arg1_modifies.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = pkb_.getModifiedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_modifies_variable = pkb_.getModifiedByProc(arg1_procedure);
				if (arg1_procedure_modifies_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_modifies = pkb_.getModifiedBy(arg1_stmt_num);
				if (arg1_modifies.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	return temp_result;
}

ResultTable QueryEvaluator::ProcessNext(Clause next_clause)
{
	string arg1 = next_clause.GetArg().at(0);
	string arg2 = next_clause.GetArg().at(1);
	string arg1_type = next_clause.GetArgType().at(0);
	string arg2_type = next_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}
			if (pkb_.isNext(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			// Check if there is a statement that can be executed after arg1_stmt_num
			list<int> arg1_executed_after = pkb_.getExecutedAfter(arg1_stmt_num);
			if (arg1_executed_after.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			// Init the table with arg2 synonym
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			// For each statemet of arg2_type, check if arg1_stmt_num is executed before it
			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isNext(arg1_stmt_num, arg2_stmt_num) != false) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	} 
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}
			
			list<int> arg2_executed_before = pkb_.getExecutedBefore(arg2_stmt_num);
			if (arg2_executed_before.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isNextEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> arg2_executed_before = pkb_.getExecutedBefore(arg2_stmt_num);
				if (arg2_executed_before.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int>synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isNext(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> arg1_executed_after = pkb_.getExecutedAfter(arg1_stmt_num);
				if (arg1_executed_after.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			// Corner case: Next(n,n)
			if (arg1 == arg2) {
				return temp_result;
			}

			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
					if (pkb_.isNext(arg1_stmt_num, arg2_stmt_num) != false) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						if (arg2_type == ARGTYPE_CALLS) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_row_data.push_back(arg2_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessNextT(Clause next_star_clause)
{
	string arg1 = next_star_clause.GetArg().at(0);
	string arg2 = next_star_clause.GetArg().at(1);
	string arg1_type = next_star_clause.GetArgType().at(0);
	string arg2_type = next_star_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_CONSTANT) {
		int arg1_stmt_num = stoi(arg1);
		if (pkb_.isValidStmt(arg1_stmt_num) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			if (pkb_.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<int> executed_after_arg1_list = pkb_.getExecutedAfterStar(arg1_stmt_num);
			if (executed_after_arg1_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				if (pkb_.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			list<int> executed_before_arg2_list = pkb_.getExecutedBeforeStar(arg2_stmt_num);
			if (executed_before_arg2_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isNextEmpty() == false) {
				temp_result.SetIsQueryTrue(true);
			}
			
			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
				list<int> executed_before_star_arg2 = pkb_.getExecutedBeforeStar(arg2_stmt_num);
				if (executed_before_star_arg2.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg2_argument = to_string(arg2_stmt_num);
					if (arg2_type == ARGTYPE_CALLS) {
						arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
					}
					temp_row_data.push_back(arg2_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym
		list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
		if (synonym_stmt_list_arg1.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_CONSTANT) {
			int arg2_stmt_num = stoi(arg2);
			if (pkb_.isValidStmt(arg2_stmt_num) == false) {
				return temp_result;
			}

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				if (pkb_.isNextStar(arg1_stmt_num, arg2_stmt_num) == true) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<int> executed_after_star_arg1 = pkb_.getExecutedAfterStar(arg1_stmt_num);
				if (executed_after_star_arg1.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					string arg1_argument = to_string(arg1_stmt_num);
					if (arg1_type == ARGTYPE_CALLS) {
						arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
					}
					temp_row_data.push_back(arg1_argument);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym
			list<int> synonym_stmt_list_arg2 = GetList(arg2_type);
			if (synonym_stmt_list_arg2.empty() == true) {
				return temp_result;
			}

			if (arg1 == arg2) {
				temp_result = ResultTable(arg1);
			}
			else {
				temp_result = ResultTable(arg1, arg2);
			}
			
			// Corner case N*(n,n)
			if (arg1 == arg2) {
				for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
					if (pkb_.isNextStar(arg1_stmt_num, arg1_stmt_num) == true) {
						temp_result.SetIsQueryTrue(true);
						string arg1_argument = to_string(arg1_stmt_num);
						if (arg1_type == ARGTYPE_CALLS) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
			else {
				for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
					for (auto &arg2_stmt_num : synonym_stmt_list_arg2) {
						if (pkb_.isNextStar(arg1_stmt_num, arg2_stmt_num)) {
							temp_result.SetIsQueryTrue(true);
							string arg1_argument = to_string(arg1_stmt_num);
							string arg2_argument = to_string(arg2_stmt_num);
							if (arg1_type == ARGTYPE_CALLS) {
								arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
							}
							if (arg2_type == ARGTYPE_CALLS) {
								arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
							}
							temp_row_data.push_back(arg1_argument);
							temp_row_data.push_back(arg2_argument);
							temp_result.InsertRow(temp_row_data);
							temp_row_data.clear();
						}
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessCalls(Clause calls_clause)
{
	string arg1 = calls_clause.GetArg().at(0);
	string arg2 = calls_clause.GetArg().at(1);
	string arg1_type = calls_clause.GetArgType().at(0);
	string arg2_type = calls_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_STRING) {
		if (pkb_.isProcedureExist(arg1) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			if (pkb_.isCall(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<string> arg1_callee = pkb_.getCallee(arg1);
			if (arg1_callee.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should be non-empty
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;
			
			list<string> arg1_callee_list = pkb_.getCallee(arg1);
			if (arg1_callee_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg1_callee : arg1_callee_list) {
					temp_row_data.push_back(arg1_callee);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller = pkb_.getCaller(arg2);
			if (arg2_caller.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isCallExist() == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should not be empt
			list<string> procedure_list = pkb_.getProcedureList();
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_procedure : procedure_list) {
				list<string> arg2_caller = pkb_.getCaller(arg2_procedure);
				if (arg2_caller.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg2_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym of procedure type. Should not be empty
		list<string> procedure_list = pkb_.getProcedureList();
		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_list = pkb_.getCaller(arg2);
			if (arg2_caller_list.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &arg2_caller : arg2_caller_list) {
					temp_row_data.push_back(arg2_caller);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee = pkb_.getCallee(arg1_procedure);
				if (arg1_callee.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type. Should not be empty
			// Corner case: Call(p,p)
			if (arg1 == arg2) {
				return temp_result;
			}

			if (procedure_list.size() < 2) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee_list = pkb_.getCallee(arg1_procedure);
				if (arg1_callee_list.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_callee : arg1_callee_list) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg1_callee);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
			
			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessCallsStar(Clause calls_star_clause)
{
	string arg1 = calls_star_clause.GetArg().at(0);
	string arg2 = calls_star_clause.GetArg().at(1);
	string arg1_type = calls_star_clause.GetArgType().at(0);
	string arg2_type = calls_star_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg1_type == ARGTYPE_STRING) {
		if (pkb_.isProcedureExist(arg1) == false) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			if (pkb_.isCallStar(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			list<string> arg1_callee_star = pkb_.getCalleeStar(arg1);
			if (arg1_callee_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			list<string> arg1_callee_star_list = pkb_.getCalleeStar(arg1);
			if (arg1_callee_star_list.empty() == false) {
				for (auto &arg1_callee_star : arg1_callee_star_list) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_callee_star);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_star = pkb_.getCallerStar(arg2);
			if (arg2_caller_star.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			if (pkb_.isCallExist() == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			list<string> procedure_list = pkb_.getProcedureList();
			temp_result = ResultTable(arg2);
			vector<string> temp_row_data;

			for (auto &arg2_procedure : procedure_list) {
				list<string> arg2_caller_star = pkb_.getCallerStar(arg2_procedure);
				if (arg2_caller_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg2_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a synonym of procedure type. Should not be empty
		list<string> procedure_list = pkb_.getProcedureList();
		temp_result = ResultTable(arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_STRING) {
			if (pkb_.isProcedureExist(arg2) == false) {
				return temp_result;
			}

			list<string> arg2_caller_star_list = pkb_.getCallerStar(arg2);
			if (arg2_caller_star_list.empty() == false) {
				for (auto &arg2_caller_star : arg2_caller_star_list) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg2_caller_star);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_ANY) {
			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee_star = pkb_.getCalleeStar(arg1_procedure);
				if (arg1_callee_star.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 2 is a synonym of procedure type
			// Corner Case: Call*(p,p)
			if (arg1 == arg2) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_callee_star_list = pkb_.getCalleeStar(arg1_procedure);
				if (arg1_callee_star_list.empty() == false) {
					for (auto &arg1_callee_star : arg1_callee_star_list) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg1_callee_star);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessPatternAssign(Clause pattern_assign_clause)
{
	string pattern_assign_syn = pattern_assign_clause.GetArg().at(0);
	string arg1 = pattern_assign_clause.GetArg().at(1);
	string arg2 = pattern_assign_clause.GetArg().at(2);
	string arg1_type = pattern_assign_clause.GetArgType().at(1);
	string arg2_type = pattern_assign_clause.GetArgType().at(2);

	list<int> all_assign_statements = GetList(ARGTYPE_ASSIGN);

	ResultTable temp_result;

	if (all_assign_statements.empty() == true) {
		return temp_result;
	}

	temp_result = ResultTable(pattern_assign_syn);
	vector<string> temp_row_data;

	if (arg1_type == ARGTYPE_STRING) {
		if (pkb_.isValidVar(arg1) == false) {
			return temp_result;
		}
		list<int> arg1_modified_by = pkb_.getModifiedBy(arg1);
		if (arg1_modified_by.empty() == true) {
			return temp_result;
		}

		if (arg2_type == ARGTYPE_ANY) {
			if (arg1_modified_by.empty() == true) {
				return temp_result;
			}

			for (auto &assign_stmt : all_assign_statements) {
				for (auto &arg1_modified_by_stmt_num : arg1_modified_by) {
					if (assign_stmt == arg1_modified_by_stmt_num) {
						temp_result.SetIsQueryTrue(true);
						temp_row_data.push_back(to_string(arg1_modified_by_stmt_num));
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = pkb_.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == false) {
				for (auto &arg1_modified_by_stmt_num : arg1_modified_by) {
					for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
						if (arg1_modified_by_stmt_num == pattern_assign_syn_stmt_num) {
							temp_result.SetIsQueryTrue(true);
							temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
							temp_result.InsertRow(temp_row_data);
							temp_row_data.clear();
						}
					}
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = pkb_.getAssignWithSubExpression(arg2);
			if (assign_with_sub_expression.empty() == false) {
				for (auto &arg1_modified_by_stmt_num : arg1_modified_by) {
					for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
						if (arg1_modified_by_stmt_num == pattern_assign_syn_stmt_num) {
							temp_result.SetIsQueryTrue(true);
							temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
							temp_result.InsertRow(temp_row_data);
							temp_row_data.clear();
						}
					}
				}
			}

			return temp_result;
		}
		else {
			// Should never be here. Consider removing this else.
			return temp_result;
		}
	}
	else if (arg1_type == ARGTYPE_ANY) {
		if (arg2_type == ARGTYPE_ANY) {
			list<int> all_assign_list = pkb_.getAssignList();
			if (all_assign_list.empty() == true) {
				return temp_result;
			}

			temp_result.SetIsQueryTrue(true);

			for (auto &pattern_assign_syn_stmt_num : all_assign_list) {
				temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = pkb_.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = pkb_.getAssignWithSubExpression(arg2);
			if (assign_with_sub_expression.empty() == false) {
				temp_result.SetIsQueryTrue(true);
				for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
					temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 1 is a variable synonym
		list<string> all_variable_list = pkb_.getVarList();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(pattern_assign_syn, arg1);
		vector<string> temp_row_data;

		if (arg2_type == ARGTYPE_ANY) {
			for (auto &pattern_assign_syn_stmt_num : all_assign_statements) {
				list<string> pattern_assign_syn_modifies_variables = pkb_.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
			return temp_result;
		}
		else if (arg2_type == ARGTYPE_EXPR) {
			list<int> assign_with_expression = pkb_.getAssignWithExpression(arg2);
			if (assign_with_expression.empty() == true) {
				return temp_result;
			}

			for (auto &pattern_assign_syn_stmt_num : assign_with_expression) {
				list<string> pattern_assign_syn_modifies_variables = pkb_.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else if (arg2_type == ARGTYPE_SUB_EXPR) {
			list<int> assign_with_sub_expression = pkb_.getAssignWithSubExpression(arg2);
			if (assign_with_sub_expression.empty() == true) {
				return temp_result;
			}

			for (auto &pattern_assign_syn_stmt_num : assign_with_sub_expression) {
				list<string> pattern_assign_syn_modifies_variables = pkb_.getModifiedBy(pattern_assign_syn_stmt_num);
				if (pattern_assign_syn_modifies_variables.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg1_variable : pattern_assign_syn_modifies_variables) {
						temp_row_data.push_back(to_string(pattern_assign_syn_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessPatternWhile(Clause pattern_while_clause)
{
	string pattern_while_syn = pattern_while_clause.GetArg().at(0);
	string arg1 = pattern_while_clause.GetArg().at(1);
	string arg1_type = pattern_while_clause.GetArgType().at(1);

	ResultTable temp_result = ResultTable(pattern_while_syn);
	vector<string> temp_row_data;

	if (arg1_type == ARGTYPE_STRING) {
		if (pkb_.isValidVar(arg1) == false) {
			return temp_result;
		}

		list<int> while_stmt_with_arg1_control_variable = pkb_.getWhileListWithControlVariable(arg1);
		if (while_stmt_with_arg1_control_variable.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &while_stmt_num : while_stmt_with_arg1_control_variable) {
				temp_row_data.push_back(to_string(while_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else if (arg1_type == ARGTYPE_ANY) {
		list<int> all_while_list = pkb_.getWhileList();
		if (all_while_list.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &while_stmt_num : all_while_list) {
				temp_row_data.push_back(to_string(while_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else {
		// Argument 1 is a synonym
		list<string> all_variable_list = pkb_.getVarList();
		if (all_variable_list.empty() == false) {
			temp_result = ResultTable(pattern_while_syn, arg1);
			for (auto &arg1_variable : all_variable_list) {
				list<int> while_with_arg1_control_variable = pkb_.getWhileListWithControlVariable(arg1_variable);
				if (while_with_arg1_control_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &while_stmt_num : while_with_arg1_control_variable) {
						temp_row_data.push_back(to_string(while_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
		}

		return temp_result;
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessPatternIf(Clause pattern_if_clause)
{
	string pattern_if_syn = pattern_if_clause.GetArg().at(0);
	string arg1 = pattern_if_clause.GetArg().at(1);
	string arg1_type = pattern_if_clause.GetArgType().at(1);

	ResultTable temp_result = ResultTable(pattern_if_syn);
	vector<string> temp_row_data;

	if (arg1_type == ARGTYPE_STRING) {
		if (pkb_.isValidVar(arg1) == false) {
			return temp_result;
		}

		list<int> if_with_arg1_control_variable = pkb_.getIfListWithControlVariable(arg1);
		if (if_with_arg1_control_variable.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &if_stmt_num : if_with_arg1_control_variable) {
				temp_row_data.push_back(to_string(if_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else if (arg1_type == ARGTYPE_ANY) {
		list<int> all_if_list = pkb_.getIfList();
		if (all_if_list.empty() == false) {
			temp_result.SetIsQueryTrue(true);
			for (auto &if_stmt_num : all_if_list) {
				temp_row_data.push_back(to_string(if_stmt_num));
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}
		}

		return temp_result;
	}
	else {
		// Argument 1 is a synonym
		list<string> all_variable_list = pkb_.getVarList();
		if (all_variable_list.empty() == false) {
			temp_result = ResultTable(pattern_if_syn, arg1);
			for (auto &arg1_variable : all_variable_list) {
				list<int> if_with_arg1_control_variable = pkb_.getWhileListWithControlVariable(arg1_variable);
				if (if_with_arg1_control_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &if_stmt_num : if_with_arg1_control_variable) {
						temp_row_data.push_back(to_string(if_stmt_num));
						temp_row_data.push_back(arg1_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
		}

		return temp_result;
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessWith(Clause with_clause)
{
	string arg1_type = with_clause.GetArgType().at(0);
	string arg2_type = with_clause.GetArgType().at(1);

	ResultTable temp_result;

	if ((arg1_type != ARGTYPE_PROCEDURE && arg1_type != ARGTYPE_CALLS_NAME && arg1_type != ARGTYPE_VARIABLE && arg1_type != ARGTYPE_STRING)
		&& (arg2_type != ARGTYPE_PROCEDURE && arg2_type != ARGTYPE_CALLS_NAME && arg2_type != ARGTYPE_VARIABLE && arg2_type != ARGTYPE_STRING)) {
		temp_result = ProcessWithNumber(with_clause);
	}
	else {
		temp_result = ProcessWithName(with_clause);
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessWithName(Clause with_name_clause)
{
	string arg1 = with_name_clause.GetArg().at(0);
	string arg2 = with_name_clause.GetArg().at(1);
	string arg1_type = with_name_clause.GetArgType().at(0);
	string arg2_type = with_name_clause.GetArgType().at(1);

	ResultTable temp_result;// = ResultTable(arg1, arg2);
	vector<string> temp_row_data;

	bool is_arg1_syn = false;
	bool is_arg2_syn = false;
	bool is_arg1_call = false;
	bool is_arg2_call = false;
	list<string> arg1_data_list;
	list<string> arg2_data_list;

	if (arg1_type == ARGTYPE_PROCEDURE) {
		is_arg1_syn = true;
		temp_result.InsertNewColumn(arg1);
		arg1_data_list = pkb_.getProcedureList();
	}
	else if (arg1_type == ARGTYPE_VARIABLE) {
		is_arg1_syn = true;
		temp_result.InsertNewColumn(arg1);
		arg1_data_list = pkb_.getVarList();
	}
	else if (arg1_type == ARGTYPE_CALLS_NAME) {
		is_arg1_syn = true;
		is_arg1_call = true;
		temp_result.InsertNewColumn(arg1);
		arg1_data_list = pkb_.getCalledProcNamesList();
	}
	else {
		arg1_data_list.push_back(arg1); // Arg1 is a string
	}

	if (arg2_type == ARGTYPE_PROCEDURE) {
		is_arg2_syn = true;
		temp_result.InsertNewColumn(arg2);
		arg2_data_list = pkb_.getProcedureList();
	}
	else if (arg2_type == ARGTYPE_VARIABLE) {
		is_arg2_syn = true;
		temp_result.InsertNewColumn(arg2);
		arg2_data_list = pkb_.getVarList();
	}
	else if (arg2_type == ARGTYPE_CALLS_NAME) {
		is_arg2_syn = true;
		is_arg2_call = true;
		temp_result.InsertNewColumn(arg2);
		arg2_data_list = pkb_.getCalledProcNamesList();
	}
	else {
		arg2_data_list.push_back(arg2); // Arg2 is a string
	}

	if ((arg1_data_list.empty() == false) && (arg2_data_list.empty() == false)) {
		if (is_arg1_call == true && is_arg2_call == true) {
			for (auto &arg1_data : arg1_data_list) {
				list<int> arg1_call_list = pkb_.getCallByProcName(arg1_data);
				for (auto &arg2_data : arg2_data_list) {
					list<int> arg2_call_list = pkb_.getCallByProcName(arg2_data);
					if (arg1_data == arg2_data) {
						temp_result.SetIsQueryTrue(true);
						for (auto &arg1_call : arg1_call_list) {
							for (auto &arg2_call : arg2_call_list) {
								string arg1_argument = to_string(arg1_call) + " " + arg1_data;
								string arg2_argument = to_string(arg2_call) + " " + arg2_data;
								temp_row_data.push_back(arg1_data);
								temp_row_data.push_back(arg2_data);
								temp_result.InsertRow(temp_row_data);
								temp_row_data.clear();
							}
						}
					}
				}
			}
		}
		else if (is_arg1_call == true) {
			for (auto &arg1_data : arg1_data_list) {
				list<int> arg1_call_list = pkb_.getCallByProcName(arg1_data);
				for (auto &arg2_data : arg2_data_list) {
					if (arg1_data == arg2_data) {
						temp_result.SetIsQueryTrue(true);
						for (auto &arg1_call : arg1_call_list) {
							string arg1_argument = to_string(arg1_call) + " " + arg1_data;
							temp_row_data.push_back(arg1_argument);
							if (is_arg2_syn == true) {
								temp_row_data.push_back(arg2_data);
							}
							temp_result.InsertRow(temp_row_data);
							temp_row_data.clear();
						}
					}
				}
			}
		}
		else if (is_arg2_call == true) {
			for (auto &arg1_data : arg1_data_list) {
				for (auto &arg2_data : arg2_data_list) {
					list<int> arg2_call_list = pkb_.getCallByProcName(arg2_data);
					if (arg1_data == arg2_data) {
						temp_result.SetIsQueryTrue(true);
						for (auto &arg2_call : arg2_call_list) {
							if (is_arg1_syn == true) {
								temp_row_data.push_back(arg1_data);
							}
							string arg2_argument = to_string(arg2_call) + " " + arg2_data;
							temp_row_data.push_back(arg2_argument);
							temp_result.InsertRow(temp_row_data);
							temp_row_data.clear();
						}
					}
				}
			}
		}
		else {
			for (auto &arg1_data : arg1_data_list) {
				for (auto &arg2_data : arg2_data_list) {
					if (arg1_data == arg2_data) {
						temp_result.SetIsQueryTrue(true);
						if (is_arg1_syn == true) {
							temp_row_data.push_back(arg1_data);
						}
						if (is_arg2_syn == true) {
							temp_row_data.push_back(arg2_data);
						}
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}
		}
	}

	return temp_result;
}

ResultTable QueryEvaluator::ProcessWithNumber(Clause with_number_clause)
{
	string arg1 = with_number_clause.GetArg().at(0);
	string arg2 = with_number_clause.GetArg().at(1);
	string arg1_type = with_number_clause.GetArgType().at(0);
	string arg2_type = with_number_clause.GetArgType().at(1);

	ResultTable temp_result;
	vector<string> temp_row_data;

	bool is_arg1_syn = false;
	bool is_arg2_syn = false;

	list<int> arg1_data_list;
	list<int> arg2_data_list;

	if (arg1_type == ARGTYPE_NUMBER) {
		arg1_data_list.push_back(stoi(arg1));
	}
	else {
		is_arg1_syn = true;
		temp_result.InsertNewColumn(arg1);
		arg1_data_list = GetList(arg1_type);
	}

	if (arg2_type == ARGTYPE_NUMBER) {
		arg2_data_list.push_back(stoi(arg2));
	}
	else {
		is_arg2_syn = true;
		temp_result.InsertNewColumn(arg2);
		arg2_data_list = GetList(arg2_type);
	}

	if ((arg1_data_list.empty() == false) && (arg2_data_list.empty() == false)) {
		for (auto &arg1_stmt_num : arg1_data_list) {
			for (auto &arg2_stmt_num : arg2_data_list) {
				if (arg1_stmt_num == arg2_stmt_num) {
					temp_result.SetIsQueryTrue(true);
					if (is_arg1_syn == true) {
						string arg1_argument = to_string(arg1_stmt_num);
						if (arg1_type == ARGTYPE_CALLS_NUMBER) {
							arg1_argument += " " + pkb_.getProcNameByCallStmt(arg1_stmt_num);
						}
						temp_row_data.push_back(arg1_argument);
					}
					if (is_arg2_syn == true) {
						string arg2_argument = to_string(arg2_stmt_num);
						if (arg2_type == ARGTYPE_CALLS_NUMBER) {
							arg2_argument += " " + pkb_.getProcNameByCallStmt(arg2_stmt_num);
						}
						temp_row_data.push_back(arg2_argument);
					}
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}
		}
	}

	return temp_result;
}

bool QueryEvaluator::ProcessNoSynGroup()
{
	vector<Clause> non_related_group = input_query_.GetNoSynGroup();
	ResultTable temp_result;

	for (auto &no_syn_clause : non_related_group) {
		temp_result = ProcessClause(no_syn_clause);
		if (temp_result.IsQueryTrue() == false) {
			return false;
		}
	}
	return true;
}

bool QueryEvaluator::ProcessConnectedGroups()
{
	vector<vector<Clause>> connected_groups = input_query_.GetConnectedGroups();
	vector<ResultTable> group_intermediate_result;
	ResultTable temp_result;

	for (auto &group : connected_groups) {
		for (auto &connected_clause : group) {
			temp_result = ProcessClause(connected_clause);
			if (temp_result.IsQueryTrue() == false) {
				return false;
			}
			group_intermediate_result.push_back(temp_result);
		}
		connected_group_intermediate_result_.push_back(group_intermediate_result);
		group_intermediate_result.clear();
	}

	return true;
}

bool QueryEvaluator::ProcessNonConnectedGroups()
{
	vector<vector<Clause>> non_connected_groups = input_query_.GetNonConnectedGroups();
	vector<ResultTable> group_intermediate_result;
	ResultTable temp_result;

	for (auto &group : non_connected_groups) {
		for (auto &non_connected_clause : group) {
			temp_result = ProcessClause(non_connected_clause);
			if (temp_result.IsQueryTrue() == false) {
				return false;
			}
			group_intermediate_result.push_back(temp_result);
		}
		non_connected_group_intermediate_result_.push_back(group_intermediate_result);
		group_intermediate_result.clear();
	}

	return true;
}

bool QueryEvaluator::IsBooleanSelected()
{
	if (input_query_.GetSelectClause().GetArgType().at(0) == "BOOLEAN") {
		return true;
	}
	return false;
}

ResultTable QueryEvaluator::ProcessUses(Clause uses_clause) {
	string arg1 = uses_clause.GetArg().at(0);
	string arg2 = uses_clause.GetArg().at(1);
	string arg1_type = uses_clause.GetArgType().at(0);
	string arg2_type = uses_clause.GetArgType().at(1);

	ResultTable temp_result;

	if (arg2_type == ARGTYPE_STRING) {
		if (pkb_.isValidVar(arg2) == false) {
			return temp_result;
		}

		list<int> arg2_used_by = pkb_.getUsedBy(arg2);
		if (arg2_used_by.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			if (find(arg2_used_by.begin(), arg2_used_by.end(), arg1_stmt_num) != arg2_used_by.end()) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			if (pkb_.isUsedByProc(arg1, arg2) == true) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure. Guaranteed to have at least 1 procedure
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				if (pkb_.isUsedByProc(arg1_procedure, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synomym_stmt_list_arg1 = GetList(arg1_type);
			if (synomym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synomym_stmt_list_arg1) {
				if (pkb_.isUsed(arg1_stmt_num, arg2) == true) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}
	else if (arg2_type == ARGTYPE_VARIABLE) {
		// Argument 2 is a variable synonym
		list<string> all_variable_list = pkb_.getVarList();
		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		temp_result = ResultTable(arg2);
		vector<string> temp_row_data;

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			list<string> arg1_uses_variable = pkb_.getUsedBy(arg1_stmt_num);
			if (arg1_uses_variable.empty() == true) {
				return temp_result;
			}

			// If arg1_uses_variable non empty, add everything in arg1UsesVar to results
			temp_result.SetIsQueryTrue(true);
			for (auto &arg2_variable : arg1_uses_variable) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = pkb_.getUsedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == true) {
				return temp_result;
			}

			temp_result.SetIsQueryTrue(true);
			for (auto &arg2_variable : arg1_procedure_uses_variable) {
				temp_row_data.push_back(arg2_variable);
				temp_result.InsertRow(temp_row_data);
				temp_row_data.clear();
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type. Guaranteed to have at least 1 procedure
			temp_result = ResultTable(arg1, arg2);
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_uses_variable = pkb_.getUsedByProc(arg1_procedure);
				if (arg1_procedure_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_procedure_uses_variable) {
						temp_row_data.push_back(arg1_procedure);
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1, arg2);

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_uses_variable = pkb_.getUsedBy(arg1_stmt_num);
				if (arg1_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					for (auto &arg2_variable : arg1_uses_variable) {
						temp_row_data.push_back(to_string(arg1_stmt_num));
						temp_row_data.push_back(arg2_variable);
						temp_result.InsertRow(temp_row_data);
						temp_row_data.clear();
					}
				}
			}

			return temp_result;
		}
	}
	else {
		// Argument 2 is any ('_')
		list<string> all_variable_list = pkb_.getVarList();
;		all_variable_list.unique();
		if (all_variable_list.empty() == true) {
			return temp_result;
		}

		if (arg1_type == ARGTYPE_CONSTANT) {
			int arg1_stmt_num = stoi(arg1);
			if (pkb_.isValidStmt(arg1_stmt_num) == false) {
				return temp_result;
			}

			// Check if arg1 use anything
			list<string> arg1_uses_var = pkb_.getUsedBy(arg1_stmt_num);
			if (arg1_uses_var.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_STRING) {
			// Argument 1 is a procedure name
			if (pkb_.isProcedureExist(arg1) == false) {
				return temp_result;
			}

			list<string> arg1_procedure_uses_variable = pkb_.getUsedByProc(arg1);
			if (arg1_procedure_uses_variable.empty() == false) {
				temp_result.SetIsQueryTrue(true);
			}

			return temp_result;
		}
		else if (arg1_type == ARGTYPE_PROCEDURE) {
			// Argument 1 is a synonym of procedure type
			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;
			list<string> procedure_list = pkb_.getProcedureList();

			for (auto &arg1_procedure : procedure_list) {
				list<string> arg1_procedure_uses_variable = pkb_.getUsedByProc(arg1_procedure);
				if (arg1_procedure_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(arg1_procedure);
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
		else {
			// Argument 1 is a synonym of statement type
			list<int> synonym_stmt_list_arg1 = GetList(arg1_type);
			if (synonym_stmt_list_arg1.empty() == true) {
				return temp_result;
			}

			temp_result = ResultTable(arg1);
			vector<string> temp_row_data;

			for (auto &arg1_stmt_num : synonym_stmt_list_arg1) {
				list<string> arg1_uses_variable = pkb_.getUsedBy(arg1_stmt_num);
				if (arg1_uses_variable.empty() == false) {
					temp_result.SetIsQueryTrue(true);
					temp_row_data.push_back(to_string(arg1_stmt_num));
					temp_result.InsertRow(temp_row_data);
					temp_row_data.clear();
				}
			}

			return temp_result;
		}
	}

	return temp_result;
}

list<int> QueryEvaluator::GetList(string arg_type) {
	list<int> wanted_list;

	if (arg_type == ARGTYPE_ASSIGN) {
		wanted_list = pkb_.getAssignList();
	}
	else if (arg_type == ARGTYPE_WHILE) {
		wanted_list = pkb_.getWhileList();
	}
	else if (arg_type == ARGTYPE_IF) {
		wanted_list = pkb_.getIfList();
	} 
	else if (arg_type == ARGTYPE_CALLS || arg_type == ARGTYPE_CALLS_NUMBER) {
		wanted_list = pkb_.getCallList();
	}
	else if (arg_type == ARGTYPE_STMT || arg_type == ARGTYPE_PROG_LINE) {
		wanted_list = pkb_.getStmtList();
	}
	else if (arg_type == ARGTYPE_CONSTANT_VALUE) {
		wanted_list = pkb_.getConstantList();
	}

	return wanted_list;
}