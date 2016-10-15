/*
ParentTable main table is keyed by stmtLine and its corresponding parent
NO_PARENT_FLAG = -1 is used to indicate that the stmt is not in any container
Parent* is computed by following the chain of parents until the NO_PARENT_FLAG is reached
*/
#include "ParentTable.h"

ParentTable::ParentTable()
{
	TableChildWise = unordered_map<int, int>();
	TableParentWise = unordered_map<int, list<int> >();
	TableParentWiseStar = unordered_map<int, list<int> >();
	TableChildWiseStar = unordered_map<int, list<int> >();
}

ParentTable::~ParentTable()
{
}

void ParentTable::addParent(int parent, int child)
{	
	if (parent == NO_PARENT_FLAG)
	{
		return;
	}
	TableChildWise[child] = parent;
	TableParentWise[parent].push_back(child);
	TableChildWiseStar[child].push_back(parent);

	// Because we have to keep track of parent-star relationships
	unordered_map<int, int>::iterator it = TableChildWise.find(parent);

	while (it != TableChildWise.end())
	{ // if the parent is itself a child
		int parentOfParent = it->second;
		TableChildWiseStar[child].push_back(parentOfParent); // parent-star has been found; track it
		it = TableChildWise.find(parentOfParent);
	}
}

// The parser will populate for each child stmt all the parents in the stack (i.e. Parser will call appropriately)
void ParentTable::addParentStar(int parentStar, int child)
{
	if (parentStar == NO_PARENT_FLAG)
	{
		return;
	}
	TableParentWiseStar[parentStar].push_back(child);
}

int ParentTable::getParentOf(int stmt)
{
	int res = -1;
	unordered_map<int, int >::iterator it = TableChildWise.find(stmt);
	if (it == TableChildWise.end())
	{
		return -1;
	}
	else
	{
		return TableChildWise.at(stmt);
	}
}

list<int> ParentTable::getChildrenOf(int stmt)
{
	list<int> res;
	list<int> emptyList;

	list<int> test = TableParentWise[stmt];
	if (test.empty() == true)
	{
		return res;
	}
	else
	{
		return TableParentWise.at(stmt);
	}
}

bool ParentTable::isParentEmpty() {
	return TableChildWise.empty();
}

bool ParentTable::isParentOf(int parent, int child)
{
	bool r = false;
	int flag = -1;
	int res = flag;

	unordered_map<int, int >::iterator it = TableChildWise.find(child);
	if (it == TableChildWise.end())
	{
		return false;
	}
	else
	{
		if (TableChildWise.at(child) == parent)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}

list<int> ParentTable::getParentStar(int stmt)
{
	unordered_map<int, list<int> >::iterator it = TableChildWiseStar.find(stmt);
	list<int> res;

	if (it == TableChildWiseStar.end())
	{
		res = list<int>();
	}
	else
	{
		return it->second;
	}
}

bool ParentTable::isParentStar(int parent, int child)
{
	unordered_map<int, list<int> >::iterator it = TableChildWiseStar.find(child);
	bool res = false;

	if (it != TableChildWiseStar.end())
	{
		list<int>::iterator lIt = find(it->second.begin(), it->second.end(), parent); // does the argumnent 'parent' exist as a parent-star?
		if (lIt != it->second.end())
		{
			res = true;
		}
	}
	return res;
}

list<int> ParentTable::getChildStarOf(int stmt)
{
	unordered_map<int, list<int> >::iterator it = TableParentWiseStar.find(stmt);
	list<int> results = list<int>();

	if (it != TableParentWiseStar.end())
	{
		results = it->second;
		return results;
	}
	else
	{
		return results;
	}
}