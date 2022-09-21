#include <fstream>
#include <filesystem>
#include <sstream>
#include <vector>
#include "repo/mnist.h"
#include "globals.h"

static int reverse_int (int i) {
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

Mnist::Mnist (ostream& ostr, const char* images, const char* labels) {
	
	image_file_name=images; label_file_name=labels;
	image_file.open(image_file_name, ios_base::binary);
	label_file.open(label_file_name, ios_base::binary);

	if (!image_file.is_open()) error(ER_FILE, (string("Could not open image file ")+image_file_name).c_str());
	if (!label_file.is_open()) error(ER_FILE, (string("Could not open image file ")+label_file_name).c_str());

	image_file.read((char*) &img_magic, sizeof(img_magic));
	img_magic = reverse_int(img_magic);
	if (img_magic != 2051) error(ERROR, "Incorrect image file magic");

	label_file.read((char*) &lab_magic, sizeof(lab_magic));
	lab_magic = reverse_int(lab_magic);
	if (lab_magic != 2049) error(ERROR, "Incorrect label file magic");
	
	image_file.read((char*) &num_images, sizeof(num_images));
	num_images = reverse_int(num_images);
	label_file.read((char*) &num_labels, sizeof(num_labels));
	num_labels = reverse_int(num_labels);
	if (num_images != num_labels) error(ERROR, "image file nums should equal to label num");
	
	image_file.read((char*) &rows, sizeof(rows));
	image_file.read((char*) &cols, sizeof(cols));
	cols = reverse_int(cols);
	rows = reverse_int(rows);
	data.image = cv::Mat(rows, cols, CV_8UC1);
	data.image.reserveBuffer(rows*cols);
	LOGI(ostr, "Mnist: "<<num_images<<" recs, "<<rows<<'x'<<cols<<'\n')
}

Mnist::Mnist (ostream& ostr, const char* image_file_name) {

	filesystem::path img_file {image_file_name};
	image_file.open(image_file_name, ios_base::binary);
	if (!image_file.is_open()) error(ER_FILE, (string("Could not open image file ")+image_file_name).c_str());
	
	image_file.read((char*) &rows, sizeof(rows));
	image_file.read((char*) &cols, sizeof(cols));
	image_file.read((char*) &num_images, sizeof(num_images));
	data.image = cv::Mat(rows, cols, CV_8UC1);
	data.image.reserveBuffer(rows*cols);
	data.label = img_file.stem().string()[0];
	LOGI(ostr, "IDX3: "<<num_images<<" recs, "<<rows<<'x'<<cols<<", label: "<<unsigned(data.label)<<'\n')
}

		// digits D:\git\cpp\net\mnist\train
void separate(Mnist& mnist) {

	LOGI(cout, "Split to digits\n")
	constexpr uint32_t num_recs_pos = 2*sizeof(uint32_t);
	constexpr uint32_t data_pos = 10*3*sizeof(uint32_t);
	vector<ofstream> digits(10);
	vector<uint32_t> total(10);

	for (int i=0; i<10; i++) {

		stringstream path;
		path<<"./mnist/digits/";
		if (not create_dir(path.str())) goto exit;
		path<<i<<".idx3-ubyte";

		digits[i].open(path.str(), ios_base::binary);
		if (digits[i].is_open()){
			
			digits[i].write((char*) &mnist.rows, sizeof(uint32_t));
			digits[i].write((char*) &mnist.cols, sizeof(uint32_t));
			digits[i].write((char*) &num_recs_pos, sizeof(uint32_t));
		}
	}
	for (int i=0; i<mnist.num_images; i++) {
		
		mnist.get_next();
		total[(int) mnist.data.label]++;
		//digits[(int) mnist.data.label].write((char*) mnist.data.image.data, mnist.data.image.size());
	}

	exit:
	for (int i=0; i<10; i++) {

		if (digits[i].is_open()) {

			digits[i].seekp(num_recs_pos);
			digits[i].write((char*) &total[i], sizeof(uint32_t));
			digits[i].close();
			LOGI(cout, i<<':'<<total[i]<<endl)
		}
	}
}