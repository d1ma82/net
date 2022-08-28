#include <iostream>
#include "test_interpreter.h"
#include "utils/log.h"

using namespace std;


int main(int argc, char* argv[]) {
	
	LOG(cout, "test_create "); if (test_create()) {LOG(cout, "OK\n")} else LOG(cout, "No\n");
	LOG(cout, "test_create_no_param "); if (test_create_no_param()) {LOG(cout, "OK\n")} else LOG(cout, "No\n");
	LOG(cout, "test_create_space_last "); if (test_create_space_last()) {LOG(cout, "OK\n")} else LOG(cout, "No\n");
	LOG(cout, "test_create_space_last_no_param "); if (test_create_space_last_no_param()) {LOG(cout, "OK\n")} else LOG(cout, "No\n");
	
}