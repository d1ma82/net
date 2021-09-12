#include<iostream>
#include<opencv2/core.hpp>
//#include <opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include"mnist.hpp"

using namespace std;

const int INPUT_NODES = 2;
const int HIDDEN_NODES = 2;
const int OUTPUT_NODES = 1;

typedef cv::Vec<float, INPUT_NODES> input_vec;
typedef cv::Vec<float, HIDDEN_NODES> hidden_vec;
typedef cv::Vec<float, OUTPUT_NODES> output_vec;

class NeuralNetwork {

   public: 
    // размеры нейронной сети
    NeuralNetwork(float learning_rate, const output_vec& new_targets) {
        // коэффициент обучения
        l_rate  = learning_rate;
        targets = new_targets;

       // cv::theRNG().state = time(NULL);

          // веса между входящим и скрытым слоем, выбраны случайно       
        wih = cv::Mat(HIDDEN_NODES, INPUT_NODES, CV_32F);
        cv::randn(wih, 0.0f, pow(HIDDEN_NODES, -0.5f));
      
        // веса между скрытым и выходным слоем, выбраны случайно
        who = cv::Mat(OUTPUT_NODES, HIDDEN_NODES, CV_32F);
        cv::randn(who, 0.0f, pow(OUTPUT_NODES, -0.5f));
    };
   ~NeuralNetwork() {};

    // network training
   void train(const input_vec& input_layer);
   // network query
   void query(const input_vec& layer);

   void show() {

      cout << "Target: \n" << targets << endl << "Current: \n" << final_output << endl;
      cout <<"Updated who \n" << who << endl << "Updated wih: \n" << wih << endl;
   }

   // Матрицы весовых коэффициентов связей.
   // wih - вход скрытый 
   // who - скрытый выходной
   cv::Mat wih, who;

   private:
      float l_rate;
      hidden_vec hidden_input, hidden_output, hidden_error; 
      output_vec final_input, final_output, output_error, targets;
      
      float sigmoid(float x) { return 1 / (1 + exp(-x)); }
};

void NeuralNetwork::query(const input_vec& layer) {

    // рассчитать входящие сигналы для скрытого слоя
  for (int i=0; i<wih.rows; i++)
      hidden_input[i] = float(wih.row(i).dot(layer.t()));

  // рассчитать выходящие сигналы для скрытого слоя
  for (int i=0; i<hidden_input.rows; i++) 
    hidden_output[i] = sigmoid(hidden_input[i]);

  //  рассчитать входящие сигналы для исходящего слоя
  for (int i=0; i<who.rows; i++)
      final_input[i] = float(who.row(i).dot(hidden_output.t()));

  // рассчитать выходящие сигналы для исходящего слоя
  for (int i=0; i<final_input.rows; i++)
      final_output[i] = sigmoid(final_input[i]);
}

void NeuralNetwork::train(const input_vec& input_layer) {

  query(input_layer);
  // рассчитем ошибку
  output_error = targets - final_output;

  // рассчитаем ошибку скрытого слоя
  // это output_error распределенная пропорционально весовым коэффициентам
  cv::Mat who_t = who.t();

  for (int i=0; i<who_t.rows; i++)
    hidden_error[i] = float(who_t.row(i).dot(output_error.t()));

  // обновить весовые коэффициенты между скрытым и выходным слоем
  output_vec ones_o = cv::Mat(output_vec::ones());
  output_vec tmp_o = output_error.mul(final_output).mul(ones_o - final_output);

  for (int i=0; i<tmp_o.rows; i++) {
    for (int j=0; j<hidden_output.rows; j++)
        who.at<float>(i, j) += l_rate * (tmp_o[i] * hidden_output[j]);
  };

  // обновить весовые коэффициенты между входным и скрытым слоем
  hidden_vec onec_h = cv::Mat(hidden_vec::ones());
  hidden_vec tmp_h = hidden_error.mul(hidden_output).mul(onec_h - hidden_output);

  for (int i=0; i<tmp_h.rows; i++) {
    for (int j=0; j<input_layer.rows; j++)
        wih.at<float>(i, j) += l_rate * (tmp_h[i] * input_layer[j]);
  };
}

int main () {
 /*   Mnist mnist("./data/train-images.idx3-ubyte", "./data/train-labels.idx1-ubyte");
    vector<unique_ptr<Data>> training_data(10);
    for (int i=0; i<training_data.capacity(); i++) {
      training_data[i] = mnist.get_next();
      cv::Mat tmp(mnist.rows, mnist.cols, CV_8UC1, &training_data[i]->image[0]);
      cv::imshow(to_string(training_data[i]->label), tmp);
      cv::waitKey(0);
    }
    return 0;*/
    output_vec target {0.99f};
    input_vec input_layer {0.01f, 0.01f};
   // cv::randn(input_layer, 0.0f, pow(INPUT_NODES, -0.5f));

    NeuralNetwork net(0.9f, target);
    for (int i=0; i<100; i++) {
    //  cout << "Epoch: " << i << endl;
      net.train(input_layer);
     // net.show();
    }  
    net.query(input_layer);
    net.show();
    return 0;
}