#pragma once

#include "repo/mnist.h"
#include "repo/picture.h"
#include "repo/photo.h"

namespace repo{
	
const char* MNIST 	= "mnist";
const char* PICS	= "picture";
const char* PHOTO 	= "photo";
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

		LOG(ostr, "Scale factor: "<<global_scale_factor<<'\n')
		if (conf.empty()) {
			error(ERROR, "No repo configuration.\n");
		}
		else if (conf.at(TYPE)==MNIST) {
			database= new Mnist(ostr, conf.at(IMAGES).c_str(), conf.at(LABELS).c_str());
		}
		else if (conf.at(TYPE)==PICS) {
			database = new Pictures(ostr, conf.at(IMAGES).c_str(), global_scale_factor);
		}
		else if (conf.at(TYPE)==PHOTO) {
			database = new Photo(ostr, conf.at(IMAGES).c_str(), global_scale_factor);
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