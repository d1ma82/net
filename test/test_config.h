#include "utils/config.h"

void test_config() {
	//config conf{"conf.txt"};
	config conf;
	conf.open("conf.txt");
	if (!conf.is_open()) return;
	string type;
	conf["type"]>>type;
	int count{0}, count2{0};
	conf["count"]>>count;
	conf["count"]>>count2;
	if (conf["count"].fail()) {
		LOG("Fail count2\n")
	}
	LOG(type<<';'<<count<<';'<<count2<<'\n'<<conf["thread1"].str())
}