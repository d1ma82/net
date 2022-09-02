#pragma once

#include "repo/mnist.h"
#include "repo/picture.h"

namespace repo{
	
const char* MNIST 	= "mnist";
const char* IDX3 	= "idx3";
const char* PICS	= "picture";
const char* TYPE	= "type";
const char* IMAGES 	= "images";
const char* LABELS 	= "labels";

inline bool valid(const string command) {

	return command==TYPE or command==IMAGES or command==LABELS;
}

class Repo {
	
private:
	Database* database {nullptr};
public:
	Repo (ostream& ostr, const config& conf) {
		if (conf.empty()) {
			error(ERROR, "No repo configuration.\n");
		}
		else if (conf.at(TYPE)==MNIST) {
			database= new Mnist(ostr, conf.at(IMAGES).c_str(), conf.at(LABELS).c_str());
		}
		else if (conf.at(TYPE)==IDX3) {
			database = new Mnist(ostr, conf.at(IMAGES).c_str());
		}
		else if (conf.at(TYPE)==PICS) {
			database = new Pictures(ostr, conf.at(IMAGES).c_str());
		} 		
		else {
			error(ERROR, "Unknown database type.\n");
		}
	}
		
	~Repo(){
		if (database) delete database;
	}
	
	inline Database* operator->() { 
		if (!database) error(ER_NULLPTR, "Database* operator->, database==nullptr\n");
		return database;
	}
};
}