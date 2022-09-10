#pragma once

#include <set>
#include <filesystem>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "repo/database.h"

class Pictures: public Database {
	
private:
	ostream& ostr;
	cv::Mat mat;
	size_t count {0};
	int cursor {-1};
	int file_cursor {-1};
	filesystem::path pictures;
	Data data;
	const set<std::string> formats {".png", ".jpg", ".bmp"};
	vector<pair<char, vector<std::string>>> database;

	void read_directory(const filesystem::path& pics) {

		char label = pics.stem().string()[0];
		vector<string> files;

		for (const filesystem::directory_entry& de: filesystem::directory_iterator(pics)) {

			const filesystem::path f=de;
			
			if (filesystem::is_directory(f)) read_directory(f);
			else {
				if (formats.contains(lower(f.extension().string()))) files.push_back(f.string());
			}
		}
		database.push_back({ label, move(files) });
		count+=database[database.size()-1].second.size();
	} 
	
public:
	Pictures(ostream& ostr, const char*  pics): ostr{ostr}, pictures{pics} {
		
		if (not filesystem::exists(pictures))
			error(ER_FILE, (string("Could not open image file ")+pictures.string()).c_str());
		
		if (filesystem::is_directory(pictures)) read_directory(pictures);
		else error(ER_FILE, (string("Could not read directory ")+pictures.string()).c_str());

		if (count>0) {cursor=0; file_cursor=0;}
		LOGI(ostr, "Pictures: "<<count<<" files\n")
	}
	cv::Mat get_next(const cv::RotatedRect&) final {error(ERROR, "Not implemented");return mat;}	
	const Data& get_next() final {

		if (cursor<0 or cursor>=database.size()) error(EoF, "End of file\n");

		const auto& [label, files] = database[cursor];

		if (file_cursor>=files.size()) {file_cursor=0; cursor++;}

		cv::Mat pic = cv::imread(files.at(file_cursor++), cv::IMREAD_GRAYSCALE);
		if (!pic.data) 
			error(ER_FILE, (string("Could not open file ")+files.at(file_cursor-1)).c_str());

		data.label = label;
		data.rows = pic.rows;
		data.cols = pic.cols;
		data.image.resize(pic.rows*pic.cols);
		copy(&pic.data[0], &pic.data[pic.rows*pic.cols], begin(data.image));
		return data;
	}

	inline size_t total()  const noexcept final {return count;} 
	friend void convert(const Pictures& pictures, int w, int h, const string& where) {

		if (not create_dir(where)) 
			error(ER_FILE, (string("Could not create directory, ")+where).c_str());

		LOG(pictures.ostr, "Prepare to "<<w<<'x'<<h<<'\n')
		for (auto& [label, files]: pictures.database) {
			
			string dir=where+label+'/';
			if (not create_dir(dir)) 
				error(ER_FILE, (string("Could not create directory, ")+dir).c_str());

			for (auto& file: files) {
				
				//LOG(ostr, "proccess: "<<file<<'\n')
				cv::Mat pic= cv::imread(file, cv::IMREAD_UNCHANGED); // with alpha channel

				cv::Mat alpha;
				cv::extractChannel(pic, alpha, 3);
											
				cv::Mat scaled;
				cv::resize(alpha, scaled, {w,h}, 0.0, 0.0, cv::INTER_AREA);

				filesystem::path f{file};
				cv::imwrite(dir+f.filename().string(), scaled);
			}
		}
	}
	
	~Pictures(){}
};