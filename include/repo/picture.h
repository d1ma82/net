#pragma once

#include <set>
#include <filesystem>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include "repo/database.h"
#include "utils/opencv.h"

class Pictures: public Database {
	
private:
	ostream& ostr;
	std::vector<Data> dat;
	int scale_to {0};
	size_t count {0};
	int cursor {-1};
	int file_cursor {-1};
	filesystem::path pictures;
	Data data;
	const set<std::string> formats {".png", ".jpg", ".bmp"};
	vector<pair<Label, vector<std::string>>> database;

	void read_directory(const filesystem::path& pics) {

		char label = pics.stem().string()[0];
		//LOG(ostr, "Label: "<<label<<'\n')
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
	Pictures(ostream& ostr, const char*  pics, int scale_to): 
				ostr{ostr}, scale_to{scale_to} {
		
		string p{pics};
		if (p[p.length()-1]=='\\' or p[p.length()-1]=='/') p[p.length()-1]='\0';
		pictures=p;
		if (not filesystem::exists(pictures))
			error(ER_FILE, (string("Could not open image file ")+pictures.string()).c_str());
		
		if (filesystem::is_directory(pictures)) read_directory(pictures);
		else error(ER_FILE, (string("Could not read directory ")+pictures.string()).c_str());

		if (count>0) {cursor=0; file_cursor=0;}
		LOGI(ostr, "Pictures: "<<count<<" files\n")
	}
	std::vector<Data> get_next(const cv::RotatedRect&) final {error(ERROR, "Not implemented");return dat;}	
	
	const Data get_next() final {

		if (cursor<0 or cursor>=database.size()) error(EoF, "End of file\n");

		const auto& [label, files] = database[cursor];

		if (file_cursor>=files.size()) {file_cursor=0; cursor++;}
		data.label = label;
		cv::Mat input = cv::imread(files.at(file_cursor++));
		if (!input.data) error(ER_FILE, (string("Could not open file ")+files.at(file_cursor-1)).c_str());
		cv::Mat scaled;
		cv::resize(input, scaled, {scale_to, scale_to}, 0.0, 0.0, cv::INTER_AREA);
        data.image = binarize(scaled);
		return data;
	}

	inline size_t total()  const noexcept final {return count;} 
	/*friend void convert(const Pictures& pictures, int w, int h, const string& where) {

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
	}*/
	
	~Pictures(){}
};