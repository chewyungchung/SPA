#include "ParentTable.h"

ParentTable::ParentTable() {
	TableChildWise = unordered_map<int, int>();
	TableParentWise = unordered_map<int, list<int> >();
	TableChildWiseStar = unordered_map<int, list<int> >();
}

ParentTable::~ParentTable()
{
	// What's there to de-allocate ah?
}

void ParentTable::addParent(int parent, int child) {
	
	TableChildWise[child] = parent;
	TableParentWise[parent].push_back(child);
	TableChildWiseStar[child].push_back(parent);

	// Because we have to keep track of parent-star relationships
	unordered_map<int, int>::iterator it = TableChildWise.find(parent);

	while (it != TableChildWise.end()) { // if the parent is itself a child
		int parentOfParent = it->second;
		TableChildWiseStar[child].push_back(parentOfParent); // parent-star has been found; track it
		it = TableChildWise.find(parentOfParent);
	}	
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

list<int> ParentTable::getParentStar(int stmt)
{
	unordered_map<int, list<int> >::iterator it = TableChildWiseStar.find(stmt);
	list<int> res;

	if (it == TableChildWiseStar.end()) {
		res = list<int>();
	}
	else {
		res = it->second;
	}

	return res;
}

bool ParentTable::isParentStar(int parent, int child)
{
	unordered_map<int, list<int> >::iterator it = TableChildWiseStar.find(child);
	bool res = false;

	if (it != TableChildWiseStar.end()) {
		list<int>::iterator lIt = find(it->second.begin(), it->second.end(), parent); // does the argumnent 'parent' exist as a parent-star?
		if (lIt != it->second.end()) {
			res = true;
		}
	}

	return res;
}
