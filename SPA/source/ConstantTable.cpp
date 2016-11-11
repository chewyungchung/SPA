#include "ConstantTable.h"
using namespace std;

ConstantTable::ConstantTable()
{
	constant_list_ = list<int>();
	constant_table_ = map<int, list<int>>();
}

void ConstantTable::AddConstant(int constant, int stmtline)
{
	map<int, list<int>>::iterator index = constant_table_.find(constant);
	if (index != constant_table_.end())
	{
		index->second.push_back(stmtline);
	}
	else
	{
		constant_list_.push_back(constant);
		list<int> x = list<int>();
		x.push_back(stmtline);
		constant_table_.insert(pair<int, list<int>>(constant, x));
	}
}

list<int> ConstantTable::GetConstantList(void)
{
	return constant_list_;
}

list<int> ConstantTable::GetStmtlineByConstant(int constant)
{
	map<int, list<int>>::iterator index = constant_table_.find(constant);
	if (index != constant_table_.end())
	{
		return index->second;
	}
	else
	{
		return list<int>();
	}
}