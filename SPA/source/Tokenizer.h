/*
Tokenizer splits the SIMPLE source into 'tokens': SIMPLE language flags, variables, constants etc.
*/
#pragma once

#include <stdio.h>
#include <string>
#include <deque>

using namespace std;

class Tokenizer
{
public:
	Tokenizer();
	Tokenizer(string file_name);
	string GetNextToken();

private:
	deque<string> tokens_;
};

