#pragma once
#include <exception>
#include <string>

using namespace std;

class QueryException : public runtime_error {
public: 
	QueryException();
	~QueryException() throw();
	QueryException(string errorMessage) : runtime_error(errorMessage) {}
	const char* what() const throw() {
		return runtime_error::what();
	}
};