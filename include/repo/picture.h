#pragma once

#include <fstream>
#include<opencv2/core.hpp>
#include<opencv2/imgcodecs.hpp>

#include "database.h"

class Pictures: public Database {
	
private:
	string property;
	uint32_t counter {0}; // do not use, use data_vec size instead
	uint32_t row_const {0};
	uint32_t col_const {0};
	const char* pictures;
	vector<string> data_vec;
	Data data;
	bool opened;
	
public:
	Pictures(const char* pictures): pictures{pictures} {
		
		ifstream ifs{(string)pictures + "conf.txt"};
		if (!ifs) {
			LOGE("Could not open configuration file.\n")
			return;
		}

		while (ifs >> property) {

			if (property == "count") {
				ifs>>counter;
				data_vec.resize(counter);
			} 
			else if (property == "row_constraint") {
				ifs>>row_const;
			}
			else if (property == "col_constraint") {
				ifs>>col_const;
				data.image.resize(row_const*col_const);
			}
			else if (property == "data") {
				ifs>>data_vec[--counter];
			}
		}
		opened=true;
		//LOG("Got "<<data_vec.size()<<" files with size "<<row_const<<'x'<<col_const<<'\n')
	}
	
	int total()const noexcept {return data_vec.size();}
	void split(int, int&) final {};
	void separate() final {};
	
	const Data& get_next() final {
		
		string label, path;
		replace(data_vec[counter].begin(), data_vec[counter].end(), ',', ' ');
		istringstream istr{data_vec[counter++]};
		istr >> label >> path;
		data.label = atoi(label.c_str()); 
		cv::Mat pic = cv::imread(path.c_str(), cv::IMREAD_GRAYSCALE);
		if (!pic.data) {
			LOGE("Could not open file,"<<path.c_str()<<'\n')
			throw ERROR;
		}
		data.rows = pic.rows;
		data.cols = pic.cols;
		copy(&pic.data[0], &pic.data[pic.rows*pic.cols], begin(data.image));
		return data;
	}
	
	~Pictures(){}
};