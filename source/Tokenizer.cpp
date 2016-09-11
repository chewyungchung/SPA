#include "Tokenizer.h"
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>

using namespace std;

static const string arr[] = { "{", "}", ";", "=" };

// Empty default constructor
Tokenizer::Tokenizer() {

}

// Overloaded constructor
Tokenizer::Tokenizer(string filename) {
	vector<string> specials(arr, arr + sizeof(arr) / sizeof(arr[0]));
	char whitespace = ' ';

	ifstream infile(filename);
	string line;
	string token;
	while (getline(infile, line)) {
		int length = line.length();
		for (int i = 0; i < length; i++) {
			if (find(specials.begin(), specials.end(), line[i]) != specials.end()) {
				if (token.length() != 0) {
					tokens.push_back(token);
				}

				stringstream ss;
				ss << line[i];
				ss >> token;
				tokens.push_back(token);
			}
			else if (line[i] == whitespace) {
				if (token.length() != 0) {
					tokens.push_back(token);
				}
			}
			else {
				token += line[i];
			}
		}
	}
}

string Tokenizer::getNextToken() {
	if (tokens.empty()) {
		return NULL;
	}
	else {
		string output = tokens.front();
		tokens.pop_front();
		return output;
	}
}