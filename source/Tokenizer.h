#include <stdio.h>
#include <string>

class Tokenizer{
public:
	Tokenizer();
	~Tokenizer();
	std::string getNextToken();
};

