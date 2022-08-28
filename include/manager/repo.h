#pragma once

#include <map>
#include "repo/mnist.h"
#include "repo/picture.h"

namespace repo{
	
const char* MNIST 	= "mnist";
const char* IDX3 	= "idx3";
const char* TYPE	= "type";
const char* IMAGES 	= "images";
const char* LABELS 	= "labels";

typedef std::map<string, string> config;

class Repo {
	
private:
	Database* database {nullptr};
public:
	Repo (ostream& ostr, const config& conf) {
		if (conf.empty()) {
			error(ERROR, "No repo configuration.");
		}
		else if (strcmp(conf.at(TYPE).c_str(), MNIST)==0) {
			database= new Mnist(ostr, conf.at(IMAGES).c_str(), conf.at(LABELS).c_str());
		}
		else if (strcmp(conf.at(TYPE).c_str(), IDX3)==0) {
			database = new Mnist(ostr, conf.at(IMAGES).c_str());
		} 		
		else {
			error(ERROR, "Unknown database type.");
		}
	}
		
	~Repo(){
		if (database) delete database;
	}
	
	inline Database* operator->() { 
		if (!database) error(ER_NULLPTR, "Database* operator->, database==nullptr");
		return database;
	}
};
}