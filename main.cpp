#include<iostream>
#include"neural_network.hpp"
#include"mnist.hpp"

using namespace std;

int main () {
    cout << "Training with " << TRAINNIG_RECS << " records\n";

    Mnist training_data("./data/train-images.idx3-ubyte", "./data/train-labels.idx1-ubyte");

    NeuralNetwork net(0.2f);
    input_vec input_layer;
    unique_ptr<Data> data;

    for (int i=0; i<TRAINNIG_RECS; i++) {

      data = training_data.get_next();

      // Преобразуем картинку в формат OpenCV float Vector
      cv::Mat tmp(INPUT_NODES, 1, CV_8UC1, &data->image[0]);
      tmp /= 255;
      tmp.convertTo(input_layer, CV_32F, 0.99f, 0.001f);
      
      output_vec target = {0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f};
      target[int(data->label)] = 0.99f;

      net.train(input_layer, target);
    }  
    cout << "Training ok\n";

    cout << "Testing with " << TESTING_RECS << " records\n";
    vector<int> score(TESTING_RECS);
    Mnist testing_data("./data/t10k-images.idx3-ubyte", "./data/t10k-labels.idx1-ubyte");

    for (int i=0; i<TESTING_RECS; i++) {

      data = testing_data.get_next();
      cv::Mat tmp(INPUT_NODES, 1, CV_8UC1, &data->image[0]);
      tmp /= 255;
      tmp.convertTo(input_layer, CV_32F, 0.99f, 0.001f);
      net.predict(input_layer);

      if (int(data->label) == net.answear()) score.push_back(1); else score.push_back(0);

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