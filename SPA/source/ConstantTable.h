#include <list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class ConstantTable
{
public:
	ConstantTable();
	void addConstant(int constant, int stmtline);
	list<int> getConstantList();
	list<int> getStmtlineByConstant(int constant); //input: constant, output: stmtline

private:
	unordered_map<int, list<int>> constantTable;
	list<int> constantList;
};