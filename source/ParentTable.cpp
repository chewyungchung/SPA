#include "ParentTable.h"

ParentTable::ParentTable() {
	TableChildWise = unordered_map<int, int>();
	TableParentWise = unordered_map<int, list<int> >();
}

ParentTable::~ParentTable()
{
	// What's there to de-allocate ah?
}

void ParentTable::addParent(int parent, int child) {
	
	TableChildWise[child] = parent;
	TableParentWise[parent].push_back(child);
}

int ParentTable::getParentOf(int stmt)
{
	int res = -1;
	try {
		res = TableChildWise.at(stmt);
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return -1;
	}
	return res;
}

list<int> ParentTable::getChildrenOf(int stmt) {
	list<int> res = list<int>();
	try {
		res = TableParentWise.at(stmt);
	}
	catch(out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return list<int>();
	}
	if (res.size() == 0) {
		res = list<int>();
	}
	return res;
}

bool ParentTable::isParentEmpty() {
	return TableChildWise.empty();
}

bool ParentTable::isParentOf(int parent, int child)
{
	bool r = false;
	int flag = -1;
	int res = flag;
	try {
		res = TableChildWise.at(child);
	}
	catch (out_of_range& oor) {
		cerr << "Out of range error: " << oor.what() << "\n";
		return false;
	}
	if (res != flag) {
		r = true;
	}
	return r;
}
