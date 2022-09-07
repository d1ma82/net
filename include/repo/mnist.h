#pragma once

#include <fstream>
#include "database.h"

using namespace std;

class Mnist: public Database {
    
private:
        // magic and the meta data
        uint32_t img_magic = 0, lab_magic = 0;
        uint32_t num_images;
        uint32_t num_labels;
        uint32_t rows;
        uint32_t cols;
		
		char label='\0';
		string image_file_name, label_file_name;
        ifstream image_file, label_file;
		
		ofstream conf;
		ofstream split_image;
		ofstream split_label;
		
		Data data;
		bool detect_files(const string& config);
public:
	/**
	 * Подключение к базе Mnist
	 * @param images - путь к изображениям
	 * @param labels - путь к меткам
	*/
	Mnist (std::ostream&, const char*, const char*);
	Mnist (std::ostream&, const char*);

	inline size_t total() const noexcept {return num_images;}
		
	const Data& get_next() final {

		if (label_file.is_open()) label_file.read((char*)&data.label, 1);
		image_file.read((char*) &data.image[0], rows * cols);
		return data;
	}

	friend void separate (Mnist& mnist);

   ~Mnist() {
	   if (conf.is_open()) conf.close();
	   if (image_file.is_open()) image_file.close();
	   if (label_file.is_open()) label_file.close();
   }
};