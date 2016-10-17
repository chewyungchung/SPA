#ifndef TESTWRAPPER_H
#define TESTWRAPPER_H

#include <string>
#include <iostream>
#include <list>

// include your other headers here
#include "AbstractWrapper.h"
#include "Parser.h"
#include "Clause.h"
#include "QueryToken.h"
#include "QueryTokenizer.h"
#include "QueryTable.h"
#include "QueryEvaluator.h"
#include "QueryValidator.h"
#include "ResultTable.h"
#include "QueryResultProjector.h"

class TestWrapper : public AbstractWrapper {
private:
	Parser parser_;
	PKB pkb_;
	QueryValidator qv_;
	QueryEvaluator qe_;
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
