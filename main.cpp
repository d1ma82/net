#include<iostream>
#include"neural_network.hpp"
#include"mnist.hpp"

using namespace std;

void rotate(cv::Mat& image, double angle) {

  cv::Mat M = cv::getRotationMatrix2D(cv::Point2f(10, 10), angle, 1.0);
  cv::warpAffine(image, image, M, cv::Size(image.cols, image.rows));
 // cv::imshow("-", image);
//  cv::waitKey(0);
}

int main () {

    int std_type = 2;

    cout << "Training with " << TRAINNIG_RECS << " records\n";

    Mnist training_data("./data/train-images.idx3-ubyte", "./data/train-labels.idx1-ubyte");

    NeuralNetwork net(0.2f);
    input_vec input_layer;
    unique_ptr<Data> data;

    for (int i=0; i<TRAINNIG_RECS; i++) {

      data = training_data.get_next();
      net.standartization(data->image, input_layer, std_type);    
      output_vec target = {0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f};
      target[(int) data->label] = 0.99f;

      net.train(input_layer, target);
    }  

    cout << "Testing with " << TESTING_RECS << " records\n";
    vector<int> score(TESTING_RECS);
    Mnist testing_data("./data/t10k-images.idx3-ubyte", "./data/t10k-labels.idx1-ubyte");

    for (int i=0; i<TESTING_RECS; i++) {

      data = testing_data.get_next();  
      net.standartization(data->image, input_layer, std_type);
  /*  rotate(tmp, 30);
      tmp = tmp.reshape(0, INPUT_NODES);
  */
      net.predict(input_layer);

      if ((int) data->label == net.answear()) score.push_back(1); else score.push_back(0);

      cout << "Correct: " << int(data->label) << " Net: " 
           << net.answear() << " : " << net.outputs() << endl;           
    }  
    float eff = (float) cv::sum(score)[0] / TESTING_RECS * 100;
    cout << "Efficiency = " << eff << '%' << endl;
    
   /* cout << "Show back query\n";
    for (int i=0; i<OUTPUT_NODES; i++) {

      output_vec target = {0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f};
      target[i] = 0.99f;
      net.back_query(target);
      cout << i << endl;
      net.show_backquery(to_string(i));
    }  */

    return 0;
}