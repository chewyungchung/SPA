#include "TestWrapper.h"

// implementation code of WrapperFactory - do NOT modify the next 5 lines
AbstractWrapper* WrapperFactory::wrapper = 0;
AbstractWrapper* WrapperFactory::createWrapper() {
  if (wrapper == 0) wrapper = new TestWrapper;
  return wrapper;
}
// Do not modify the following line
volatile bool TestWrapper::GlobalStop = false;

// a default constructor
TestWrapper::TestWrapper() {
  // create any objects here as instance variables of this class
  // as well as any initialization required for your spa program
	qv_ = QueryValidator();
	//qe_ = QueryEvaluator();
}

// method for parsing the SIMPLE source
void TestWrapper::parse(std::string filename) {
	// call your parser to do the parsing
  // ...rest of your code...
	cout << "Start parsing!" << endl;
	parser_ = Parser(filename);
	cout << "File initialized" << endl;
	pkb_ = parser_.process();
	cout << "SRC code parsing complete" << endl;
}

// method to evaluating a query
void TestWrapper::evaluate(std::string query, std::list<std::string>& results){
// call your evaluator to evaluate the query here
  // ...code to evaluate query...
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));
	cout << "Starting query stuff" << endl;
	qv_ = QueryValidator(query);
	QueryTable qt_ = qv_.Parse();
	qt_.Optimize();
	QueryEvaluator qe_(qt_, pkb_);
	list<string> final_results = qe_.Evaluate();
	final_results.unique();
	results = final_results;

	//// store the answers to the query in the results list (it is initially empty)
	//// each result must be a string.
}