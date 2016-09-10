#include <list>
#include <vector>
#include <string>
#include <unordered_map>

using namespace std;

class ConstantTable {
public:
	void addConstant(int, int);
	list<int> getConstantList(void);
	list<int> getStmtlineByConstant(int); //input: constant, output: stmtline

private:
	unordered_map<int, list<int>> constantTable;
	list<int> constantList;
};