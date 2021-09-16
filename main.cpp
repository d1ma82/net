#include<iostream>
#include<opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include"mnist.hpp"

using namespace std;

const int INPUT_NODES = 784;
const int HIDDEN_NODES = 200;
const int OUTPUT_NODES = 10;
const int TRAINNIG_RECS = 600;
const int TESTING_RECS = 100;

typedef cv::Vec<float, INPUT_NODES> input_vec;
typedef cv::Vec<float, HIDDEN_NODES> hidden_vec;
typedef cv::Vec<float, OUTPUT_NODES> output_vec;

class NeuralNetwork {

   public: 
    // размеры нейронной сети
    NeuralNetwork(float learning_rate) {
        // коэффициент обучения
        l_rate  = learning_rate;
        cv::theRNG().state = time(NULL);

          // веса между входящим и скрытым слоем, выбраны случайно       
        wih = cv::Mat(HIDDEN_NODES, INPUT_NODES, CV_32F);
        cv::randn(wih, 0.0f, pow(HIDDEN_NODES, -0.5f));
      
        // веса между скрытым и выходным слоем, выбраны случайно
        who = cv::Mat(OUTPUT_NODES, HIDDEN_NODES, CV_32F);
        cv::randn(who, 0.0f, pow(OUTPUT_NODES, -0.5f));
    };
   ~NeuralNetwork() {};

    // network training
   void train(const input_vec& input_layer, const output_vec& targets);
   // network query
   void query(const input_vec& layer);
    // обратный ход
   void back_query(const output_vec& targets);

   int answear() {

      int max_idx;
      cv::minMaxIdx(final_output, nullptr, nullptr, nullptr, &max_idx);
      return max_idx;
   }

   string outputs() {

     stringstream str;
     str << final_output;
     return str.str();
   }

   void show_backquery(const string& label) {

    cv::Mat pic(28, 28, CV_32F, &inputs[0]);
    cv::Mat resized;
    cv::resize(pic, resized, cv::Size(56, 56), cv::INTER_LINEAR);
    cv::imshow(label, resized);
    cv::waitKey(0);
   }

   private:
      // Матрицы весовых коэффициентов связей.
   // wih - вход скрытый 
   // who - скрытый выходной
      cv::Mat wih, who;
      float l_rate;
      input_vec inputs;
      hidden_vec hidden_input, hidden_output, hidden_error; 
      output_vec final_input, final_output, output_error;
      
      float sigmoid(float x) { return 1 / (1 + exp(-x)); }
      float inv_sigmoid(float x) { return log(x / abs(1 - x)); }
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

void NeuralNetwork::train(const input_vec& input_layer, const output_vec& targets) {

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

void NeuralNetwork::back_query(const output_vec& targets) {
  
    for (int i=0; i<final_input.rows; i++) final_input[i] = inv_sigmoid(targets[i]);
    
    cv::Mat who_t = who.t();
    for (int i=0; i<who_t.rows; i++)
      hidden_output[i] = float(who_t.row(i).dot(final_input.t()));
    
    double min_val, max_val;
    cv::minMaxIdx(hidden_output, &min_val, &max_val);
    for (int i=0; i<hidden_output.rows; i++) {

      hidden_output[i] -= float(min_val);
      hidden_output[i] /= float(max_val);
      hidden_output[i] *= 0.98f;
      hidden_output[i] += 0.001f;
    }  

    for (int i=0; i<hidden_input.rows; i++) hidden_input[i] = inv_sigmoid(hidden_output[i]);

    cv::Mat wih_t = wih.t();
    for (int i=0; i<wih_t.rows; i++)
      inputs[i] = float(wih_t.row(i).dot(hidden_input.t()));

    cv::minMaxIdx(inputs, &min_val, &max_val);
    for (int i=0; i<inputs.rows; i++) {

      inputs[i] -= (float) min_val;
      inputs[i] /= (float) max_val;
      inputs[i] *= 0.98f;
      inputs[i] += 0.001f;
    }
}

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

    for (int i=0; i<OUTPUT_NODES; i++) {

      output_vec target = {0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f, 0.001f};
      target[i] = 0.99f;
      net.back_query(target);
      cout << i << endl;
      net.show_backquery(to_string(i));
    }  
    return 0;

    cout << "Testing with " << TESTING_RECS << " records\n";
    vector<int> score(TESTING_RECS);
    Mnist testing_data("./data/t10k-images.idx3-ubyte", "./data/t10k-labels.idx1-ubyte");

    for (int i=0; i<TESTING_RECS; i++) {

      data = testing_data.get_next();
     /* cv::Mat pic(testing_data.rows, testing_data.cols, CV_8UC1, &data->image[0]);
      cv::imshow(to_string(data->label), pic);
      cv::waitKey(0);*/

      cv::Mat tmp(INPUT_NODES, 1, CV_8UC1, &data->image[0]);
      tmp /= 255;
      tmp.convertTo(input_layer, CV_32F, 0.99f, 0.001f);
      net.query(input_layer);

      if (int(data->label) == net.answear()) score.push_back(1); else score.push_back(0);

      cout << "Correct: " << int(data->label) << " Net: " 
           << net.answear() << " : " << net.outputs() << endl;           
    }  
    float eff = (float) cv::sum(score)[0] / TESTING_RECS * 100;
    cout << "Efficiency = " << eff << '%';
    //net.show();

    return 0;
}