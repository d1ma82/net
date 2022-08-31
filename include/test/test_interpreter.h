#pragma once

#include "manager/interpreter.h"
using namespace std;

typedef Interpreter<FullConnectedTrainer<Matrix<double>>> interpreter;

static interpreter interp{cin, cout};

bool test_create() {
	
	istringstream input("create 784 100 10 0.1");
	interpreter interp(input, cout);
	try{interp.run();} catch(runtime_error& ex) {return false;}
	return true;
}

bool test_create_no_param() {
	
	istringstream input("create 784 10 0.1");
	interpreter interp(input, cout);
	try{interp.run();} catch(runtime_error& ex) {return true;}
	return false;
}

bool test_create_space_last() {
	
	istringstream input("create 784 100 10 0.1 ");
	interpreter interp(input, cout);
	try{interp.run();} catch(runtime_error& ex) {return false;}
	return true;	
}

bool test_create_space_last_no_param() {
	
	istringstream input("create 784 100 10 ");
	interpreter interp(input, cout);
	try{interp.run();} catch(runtime_error& ex) {return true;}
	return false;	
}