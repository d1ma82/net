#pragma once

#include <opencv2/core.hpp>
#include <vector>
#include "globals.h"
struct Data {	
	
	Data(){}
	Data(const Data&) = delete;	
	Data& operator=(const Data&) = delete;
	
	uint32_t rows {0};
    uint32_t cols {0};
    char label = '\0';
    std::vector<unsigned char> image;
	
	void print(std::ostream& ostr) const {
		
		if (image.size() == 0) {
			LOGI(ostr, "No data\n");
			return;
		}
		LOGI(ostr, "label: "<<label<<"; size: "<<rows<<'x'<<cols<<'\n')
		for (int i=0; i<rows; i++) {
			for (int j=0; j<cols; j++)
				{LOGI(ostr, int(image[i*cols+j])<<',')}
			LOGI(ostr, '\n')
		}			
	}
};

class Database {
public:
	virtual ~Database()=default;
	virtual const Data& get_next()=0;
	virtual cv::Mat get_next(const cv::RotatedRect&)=0;
	inline virtual size_t total()const=0;
protected:
	Database(){};
};