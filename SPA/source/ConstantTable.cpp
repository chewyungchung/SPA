#include "ConstantTable.h"
using namespace std;

ConstantTable::ConstantTable()
{
	constantList = list<int>();
	constantTable = unordered_map<int, list<int>>();
}

void ConstantTable::addConstant(int constant, int stmtline)
{
	std::unordered_map<int, list<int>>::iterator index = constantTable.find(constant);
	if (index != constantTable.end())
	{
		index->second.push_back(stmtline);
	}
	else
	{
		constantList.push_back(constant);
		list<int> x = list<int>();
		x.push_back(stmtline);
		constantTable.insert(pair<int, list<int>>(constant, x));
	}
}

list<int> ConstantTable::getConstantList(void)
{
	return constantList;
}

list<int> ConstantTable::getStmtlineByConstant(int constant)
{
	std::unordered_map<int, list<int>>::iterator index = constantTable.find(constant);
	if (index != constantTable.end())
	{
		return index->second;
	}
	else
	{
		return list<int>();
	}
}