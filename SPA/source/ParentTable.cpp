/*
ParentTable main table is keyed by stmtLine and its corresponding parent
NO_PARENT_FLAG = -1 is used to indicate that the stmt is not in any container
Parent* is computed by following the chain of parents until the NO_PARENT_FLAG is reached
*/
#include "ParentTable.h"

ParentTable::ParentTable()
{
	table_child_wise_ = map<int, int>();
	table_parent_wise_ = map<int, list<int> >();
	table_parent_wise_star_ = map<int, list<int> >();
	table_child_wise_star_ = map<int, list<int> >();
}

ParentTable::~ParentTable()
{
}

void ParentTable::AddParent(int parent, int child)
{	
	if (parent == NO_PARENT_FLAG)
	{
		return;
	}
	table_child_wise_[child] = parent;
	table_parent_wise_[parent].push_back(child);
	table_child_wise_star_[child].push_back(parent);

	// Because we have to keep track of parent-star relationships
	map<int, int>::iterator it = table_child_wise_.find(parent);

	while (it != table_child_wise_.end())
	{ // if the parent is itself a child
		int parent_of_parent = it->second;
		table_child_wise_star_[child].push_back(parent_of_parent); // parent-star has been found; track it
		it = table_child_wise_.find(parent_of_parent);
	}
}

// The parser will populate for each child stmt all the parents in the stack (i.e. Parser will call appropriately)
void ParentTable::AddParentStar(int parentStar, int child)
{
	if (parentStar == NO_PARENT_FLAG)
	{
		return;
	}
	table_parent_wise_star_[parentStar].push_back(child);
}

int ParentTable::GetParentOf(int stmt)
{
	int res = -1;
	map<int, int >::iterator it = table_child_wise_.find(stmt);
	if (it == table_child_wise_.end())
	{
		return -1;
	}
	else
	{
		return table_child_wise_.at(stmt);
	}
}

list<int> ParentTable::GetChildrenOf(int stmt)
{
	list<int> res;
	list<int> empty_list;

	list<int> test = table_parent_wise_[stmt];
	if (test.empty() == true)
	{
		return res;
	}
	else
	{
		return table_parent_wise_.at(stmt);
	}
}

bool ParentTable::IsParentEmpty() {
	return table_child_wise_.empty();
}

bool ParentTable::IsParentOf(int parent, int child)
{
	bool r = false;
	int flag = -1;
	int res = flag;

	map<int, int >::iterator it = table_child_wise_.find(child);
	if (it == table_child_wise_.end())
	{
		return false;
	}
	else
	{
		if (table_child_wise_.at(child) == parent)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

list<int> ParentTable::GetParentStar(int stmt)
{
	map<int, list<int> >::iterator it = table_child_wise_star_.find(stmt);
	list<int> res;

	if (it == table_child_wise_star_.end())
	{
		return res;
	}
	else
	{
		return it->second;
	}
}

bool ParentTable::IsParentStar(int parent, int child)
{
	map<int, list<int> >::iterator it = table_child_wise_star_.find(child);
	bool res = false;

	if (it != table_child_wise_star_.end())
	{
		list<int>::iterator lIt = find(it->second.begin(), it->second.end(), parent); // does the argumnent 'parent' exist as a parent-star?
		if (lIt != it->second.end())
		{
			res = true;
		}
	}
	return res;
}

list<int> ParentTable::GetChildStarOf(int stmt)
{
	map<int, list<int> >::iterator it = table_parent_wise_star_.find(stmt);
	list<int> results = list<int>();

	if (it != table_parent_wise_star_.end())
	{
		results = it->second;
		return results;
	}
	else
	{
		return results;
	}
}