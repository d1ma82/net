#include<iostream>
#include <fstream>
#include<vector>
#include<string>

using namespace std;

int reverse_int (int i) {
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

struct Data {
    char label;
    vector<char> image;
};

class Mnist {
    private:
        ifstream image_file;
        ifstream label_file;
    public:
        Mnist(const string& image_file_name, const string& label_file_name) {

            image_file.open(image_file_name, ios::in | ios::binary);
            label_file.open(label_file_name, ios::in | ios::binary);

            if (!image_file.is_open()) {

                error = -1;
                cout << "Cant open images file: " << image_file_name << endl;
                return;
            }   

            if (!label_file.is_open()) {

                error = -1;
                cout << "Cant open labels file: " << label_file_name << endl;
                return;
            }

            image_file.read((char*) &magic, sizeof(magic));
            magic = reverse_int(magic);
            cout << magic << endl;
 
            if (magic != 2051) {
                error = 1;
                cout << "Incorrect image file magic: " << magic << endl;
                return;
            }

            label_file.read((char*) &magic, sizeof(magic));
            magic = reverse_int(magic);
            if (magic != 2049) {
                error = 2;
                cout << "Incorrect image file magic: " << magic << endl;
                return;
            }
            
            image_file.read((char*) &num_images, sizeof(num_images));
            num_images = reverse_int(num_images);
            label_file.read((char*) &num_labels, sizeof(num_labels));
            num_labels = reverse_int(num_labels);
            if (num_images != num_labels) {
                error = 3;
                cout << "image file nums should equal to label num" << endl;
                return;
            }

            image_file.read((char*) &rows, sizeof(rows));
            image_file.read((char*) &cols, sizeof(cols));
            cols = reverse_int(cols);
            rows = reverse_int(rows);
            cout << "image and label num is: " << num_images << endl;
            cout << "image rows: " << rows << ", cols: " << cols << endl;
        }

        unique_ptr<Data> get_next() {

            unique_ptr<Data> result = make_unique<Data>();
            result->image.resize(rows * cols);
            label_file.read(&result->label, 1);
            image_file.read(&result->image[0], rows * cols);
            cout << to_string(result->label) << endl;
            return result;
        }

       ~Mnist() {
           if (image_file.is_open()) image_file.close();
           if (label_file.is_open()) label_file.close();
       }
        int error = 0;
        // magic and the meta data
        uint32_t magic = 0;
        uint32_t num_images;
        uint32_t num_labels;
        uint32_t rows;
        uint32_t cols;
};