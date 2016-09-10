#include <stdio.h>
#include <string>
#include <deque>

using namespace std;

class Tokenizer{
public:
	Tokenizer();
	Tokenizer(string fileName);
	string getNextToken();

private:
	deque<string> tokens;
};

