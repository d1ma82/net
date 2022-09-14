#pragma once

#include <opencv2/core.hpp>
#include <vector>
#include "globals.h"
struct Data {	
	
	Data(){}
	Data(cv::Mat im, char lab='\0'):label{lab} {image=im;}
	//Data(const Data&) = delete;	
	//Data& operator=(const Data&) = delete;
	
    char label = '\0';
    cv::Mat image;
};

class Database {
public:
	virtual ~Database()=default;
	virtual const Data get_next()=0;
	virtual std::vector<Data> get_next(const cv::RotatedRect&)=0;
	inline virtual size_t total()const=0;
protected:
	Database(){};
};