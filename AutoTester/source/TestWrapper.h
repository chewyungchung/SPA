#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "AbstractWrapper.h"
#include "Parser.h"
#include "QueryResultProjector.h"
#include <chrono>
#include <thread>

class TestWrapper : public AbstractWrapper {
private:
	Parser _parser;
	QueryValidator _qv;
	QueryEvaluator _qe;
	QueryResultProjector _qrp;
	PKB _pkb;

public:
  // default constructor
  TestWrapper();
  
  // destructor
  ~TestWrapper();
  
  // method for parsing the SIMPLE source
  virtual void parse(std::string filename);
  
  // method for evaluating a query
  virtual void evaluate(std::string query, std::list<std::string>& results);
};

#endif
