/**
 * Полносвязная нейронная сеть, с одним промежуточным слоем
 * для распознования рукописных цифр
 * 
 *
 * @author: Dima ZV
 * @email: dimazverincevchannel@gmail.com
 *
 */

#include<opencv2/core.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/highgui.hpp>
#include<string>
#include<map>
#include<functional>

using namespace std;

const int INPUT_NODES = 784;        //  кол-во входящих узлов
const int HIDDEN_NODES = 100;       //  кол-во промежуточных узлов
const int OUTPUT_NODES = 10;        //  кол-во исходящих узлов
const int TRAINNIG_RECS = 600;     //  кол-во тренировочных примеров
const int TESTING_RECS = 100;       // кол-во тестовых примеров

typedef cv::Vec<double, INPUT_NODES> input_vec;
typedef cv::Vec<double, HIDDEN_NODES> hidden_vec;
typedef cv::Vec<double, OUTPUT_NODES> output_vec;

class NeuralNetwork {

   public: 
    /**
         * Конструктор
         * Создает новый экземпляр нейронной сети
         *
         * @param double learning_rate - шаг обучения в методе градиентного спуска
         
    */
    NeuralNetwork(double learning_rate);
   ~NeuralNetwork() {};

    /**
         * Тренировка сети 
         * 
         * @param input_vec& input_layer, данные из входящего слоя
         * @param output_vec& targets, целевые данные
    */
   void train(const input_vec& input_layer, const output_vec& targets);


    /**
         * Опрос сети
         * 
         * @param input_vec& input_layer, данные из входящего слоя
    */
   void predict(const input_vec& input_layer);


    /**
         * Обратный ход сети, те вход это целевой вектор, а входной вектор это результат
         * можно увидеть как сеть строит шаблоны для распознавания
         * 
         * @param output_vec& targets, целевые данные
    */
   void back_query(const output_vec& targets);

    /**
         * Стандартизация входных данных по формуле:
         * 
         * хn = sqrt(sum(pow(xn - x_avg))) / sqrt(n-1)
         * 
         * @param inputs входящие данные с числами типа int
         * @param input_vec& преобразованные данные типа double
    */
   void standartization(vector<char>& inputs, input_vec& outputs, int type=0);


    /**
         * Преобразование ответа сети в целое число
         * 
         * @return число которое по мнению сети изображено
    */
   int answear() {

      int max_idx;
      cv::minMaxIdx(final_output, nullptr, nullptr, nullptr, &max_idx);
      return max_idx;
   }


    /**
         * Выводит вектор вероятностей ответов
         * 
         * @return вектор вероятностей ответов
    */
   string outputs() {

     stringstream str;
     str << final_output;
     return str.str();
   }


    /**
         * Выводит результат обратного хода
         * 
         * @return результат обратного хода
    */
   void show_backquery(const string& label) {

    cv::Mat pic(28, 28, CV_64F, &inputs[0]);
    cv::Mat resized;
    cv::resize(pic, resized, cv::Size(84, 84), cv::INTER_AREA);
    cv::imshow(label, resized);
    cv::waitKey(0);
    cv::destroyAllWindows();
   }

   private:
      const output_vec ONES_O = cv::Mat(output_vec::ones());
      const hidden_vec ONES_H = cv::Mat(hidden_vec::ones());

      const std::map<string, function<double (double)>> func_map {

        make_pair("sigmoid", [] (double x) { return 1 / (1 + exp(-x)); }),
        make_pair("softmax", [] (double ) { return 0.0f; }),
        make_pair("ReLu", [] (double ) { return 0.0f; }),
        make_pair("inv_sigmoid", [](double x) { return log(x / abs(1 - x)); })
      };

      // Матрицы весовых коэффициентов связей.
      cv::Mat wih, who;
      double l_rate;
      input_vec inputs;
      hidden_vec hidden_input, hidden_output, hidden_error; 
      output_vec final_input, final_output, output_error;
      
        // функции активации
      template<typename Tp, int cn>
      void activation_func(const cv::Vec<Tp, cn>& vec_in, 
                                  cv::Vec<Tp, cn>& vec_out, 
                                        const string func_name) {

          for (int i=0; i<cn; i++) 
              vec_out[i] = func_map.at(func_name) (vec_in[i]);   
      };
      
      // Векторное произведение матрицы и вектора
      template<typename Tp, int cn, int cm>
      void dot(const cv::Mat& matrix, const cv::Matx<Tp, 1, cn>& vec, cv::Vec<Tp, cm>& vec_out) {

          for (int i=0; i<cm; i++)
            vec_out[i] = matrix.row(i).dot(vec);
      };
};

/************************************************************************************************************/

NeuralNetwork::NeuralNetwork(double learning_rate) {

    l_rate  = learning_rate;
    cv::theRNG().state = time(NULL);

        // веса между входящим и скрытым слоем, выбраны случайно с нормальным распред      
    wih = cv::Mat(HIDDEN_NODES, INPUT_NODES, CV_64F);
    cv::randn(wih, 0.001, pow(HIDDEN_NODES, -0.5));
    
    // веса между скрытым и выходным слоем, выбраны случайно с нормальным распред
    who = cv::Mat(OUTPUT_NODES, HIDDEN_NODES, CV_64F);
    cv::randn(who, 0.001, pow(OUTPUT_NODES, -0.5));
}

void NeuralNetwork::predict(const input_vec& layer) {

       // рассчитать входящие сигналы для скрытого слоя
  dot(wih, layer.t(), hidden_input);

  // рассчитать выходящие сигналы для скрытого слоя
  activation_func(hidden_input, hidden_output, "sigmoid");

  //  рассчитать входящие сигналы для исходящего слоя
  dot(who, hidden_output.t(), final_input);

  // рассчитать выходящие сигналы для исходящего слоя
  activation_func(final_input, final_output, "sigmoid");
}

void NeuralNetwork::train(const input_vec& input_layer, const output_vec& targets) {

  predict(input_layer);
  // рассчитем производную функции ошибку
  output_error = -(targets - final_output);

  // рассчитаем ошибку скрытого слоя
  // это output_error распределенная пропорционально весовым коэффициентам
  dot(who.t(), output_error.t(), hidden_error);

  // обновить весовые коэффициенты между скрытым и выходным слоем
  output_vec tmp_o = output_error.mul(final_output).mul(ONES_O - final_output);
  for (int i=0; i<OUTPUT_NODES; i++) {
    for (int j=0; j<HIDDEN_NODES; j++)
        who.at<double>(i, j) -= l_rate * tmp_o[i] * hidden_output[j];
  };

  // обновить весовые коэффициенты между входным и скрытым слоем
  hidden_vec tmp_h = hidden_error.mul(hidden_output).mul(ONES_H - hidden_output);
  for (int i=0; i<HIDDEN_NODES; i++) {
    for (int j=0; j<INPUT_NODES; j++)
        wih.at<double>(i, j) -= l_rate * tmp_h[i] * input_layer[j];
  };
}

void NeuralNetwork::back_query(const output_vec& targets) {
  
    activation_func(targets, final_input, "inv_sigmoid");
        
    cv::Mat who_t = who.t();
    for (int i=0; i<HIDDEN_NODES; i++)
      hidden_output[i] = who_t.row(i).dot(final_input.t());
    
    double min_val, max_val;
    cv::minMaxIdx(hidden_output, &min_val, &max_val);
    for (int i=0; i<hidden_output.rows; i++) {

      hidden_output[i] -= min_val;
      hidden_output[i] /= max_val;
      hidden_output[i] *= 0.98f;
      hidden_output[i] += 0.01f;
    } 

    activation_func(hidden_output, hidden_input, "inv_sigmoid");
    
    cv::Mat wih_t = wih.t();
    for (int i=0; i<INPUT_NODES; i++)
      inputs[i] = wih_t.row(i).dot(hidden_input.t());

    cv::minMaxIdx(inputs, &min_val, &max_val);
    for (int i=0; i<inputs.rows; i++) {

      inputs[i] -= min_val;
      inputs[i] /= max_val;
      inputs[i] *= 0.98f;
      inputs[i] += 0.01f;
    }
}

void NeuralNetwork::standartization(vector<char>& input_data, input_vec& outputs, int type) {

  cv::Mat tmp(INPUT_NODES, 1, CV_8UC1, &input_data[0]);
  switch (type) {

    case 1: {
        double x_avg = cv::sum(input_data)[0] / input_data.capacity();
        double s = 0.0f;
        for (auto x : input_data) s += pow(x - x_avg, 2);
        double dev = sqrt(s) / sqrt(input_data.capacity() - 1.0);
        for (int i=0; i<INPUT_NODES; i++) {

          outputs[i] = (input_data[i] - x_avg) / dev;
        }
      break;
    }
    case 2:
        cv::normalize(tmp, outputs, 1.0, 0.0, cv::NORM_INF);
        break;
    default:
      tmp /= 255;
      tmp.convertTo(outputs, CV_64F, 0.99, 0.001);   
  }

  //cout << outputs << endl;
}