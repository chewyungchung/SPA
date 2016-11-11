#pragma once

#include "DesignExtractor.h"

DesignExtractor::DesignExtractor()
{
}

DesignExtractor::DesignExtractor(PKB& pkb, int num_procs)
{
	pkb_ = &pkb;
	vertex_count_ = num_procs;
}

DesignExtractor::~DesignExtractor()
{
}

void DesignExtractor::BuildCallsGraph(int vertex_count)
{
	pkb_->InitializeCallsGraph(vertex_count); // set nxn matrix all 0s
	for (int i = 0; i < vertex_count; i++)
	{
		list<string> callees = pkb_->GetCallee(pkb_->GetProcName(i));
		list<string>::iterator it = callees.begin();
		for (; it != callees.end(); ++it)
		{
			pkb_->AddCallsGraphEdge(i, pkb_->GetProcIndex(*it));
		}
	}
}

// Pre-cond: Graph must be acyclic, otherwise the update traversal will be infinite
// Essentially a reverse DFS from all 'leaf' nodes
// Observe that anytime we move up the chain, the previous node data must be carried up
void DesignExtractor::UpdateAllProcModUses()
{
	for (int i = 0; i < vertex_count_; i++)
	{
		bool is_leaf = true;
		for (int j = 0; j < vertex_count_; j++)
		{
			if (pkb_->HasArcCallsGraph(i, j))
			{
				is_leaf = false;
			}
		}
		if (is_leaf)
		{
			//for (int j = 0; j < vertexCount; j++)
			//{
			//	revDFSVisited[j] = false;
			//}
			RevDFSAndUpdate(i);
		}
	}
}

void DesignExtractor::RevDFSAndUpdate(int v)
{
	//revDFSVisited[v] = true;
	for (int i = 0; i < vertex_count_; i++)
	{
		if (pkb_->HasArcCallsGraph(i, v))
		{
			UpdateParentProc(i, v);
			RevDFSAndUpdate(i);
		}
	}
}

void DesignExtractor::UpdateParentProc(int parent, int child)
{
	AddAllModVar(parent, child);
	AddAllUsedVar(parent, child);
}

void DesignExtractor::AddAllModVar(int parent, int child)
{
	string s_parent = pkb_->GetProcName(parent);
	string s_child = pkb_->GetProcName(child);

	list<string> mod_var_child_list = pkb_->GetModifiedByProc(s_child);
	list<string>::iterator it;
	for (it = mod_var_child_list.begin(); it != mod_var_child_list.end(); ++it)
	{
		pkb_->AddProcMod(s_parent, *it);
	}
}

void DesignExtractor::AddAllUsedVar(int parent, int child)
{
	string s_parent = pkb_->GetProcName(parent);
	string s_child = pkb_->GetProcName(child);

	list<string> uses_var_child_list = pkb_->GetUsedByProc(s_child);
	list<string>::iterator it;
	for (it = uses_var_child_list.begin(); it != uses_var_child_list.end(); ++it)
	{
		pkb_->AddProcUses(s_parent, *it);
	}
}

void DesignExtractor::UpdateAllCallStmtModUses()
{
	list<int> call_stmt_list = pkb_->GetCallList();

	list<int>::iterator it;
	for (it = call_stmt_list.begin(); it != call_stmt_list.end(); ++it)
	{
		// Update this callStmt with its associated called procedure's Mod/Uses
		int curr_call_stmt = *it;
		// Update this parentStars of this callStmt with associated proc's Mod/Uses as well
		list<int> curr_call_stmt_parent_star = pkb_->GetParentStar(curr_call_stmt);

		string associated_proc = pkb_->GetProcNameByCallStmt(curr_call_stmt);
		list<string> mod_var_list = pkb_->GetModifiedByProc(associated_proc);
		list<string> use_var_list = pkb_->GetUsedByProc(associated_proc);

		list<int>::iterator parent_star_it;

		list<string>::iterator mod_list_it;		
		for (mod_list_it = mod_var_list.begin(); mod_list_it != mod_var_list.end(); ++mod_list_it)
		{
			pkb_->AddModifies(curr_call_stmt, *mod_list_it);
			pkb_->AddModifies(*mod_list_it, curr_call_stmt);
			for (parent_star_it = curr_call_stmt_parent_star.begin(); parent_star_it != curr_call_stmt_parent_star.end(); ++parent_star_it)
			{
				pkb_->AddModifies(*parent_star_it, *mod_list_it);
				pkb_->AddModifies(*mod_list_it, *parent_star_it);
			}
		}

		list<string>::iterator use_list_it;
		for (use_list_it = use_var_list.begin(); use_list_it != use_var_list.end(); ++use_list_it)
		{
			pkb_->AddUses(curr_call_stmt, *use_list_it);
			pkb_->AddUses(*use_list_it, curr_call_stmt);
			for (parent_star_it = curr_call_stmt_parent_star.begin(); parent_star_it != curr_call_stmt_parent_star.end(); ++parent_star_it)
			{
				pkb_->AddUses(*parent_star_it, *use_list_it);
				pkb_->AddUses(*use_list_it, *parent_star_it);
			}
		}
	}
}