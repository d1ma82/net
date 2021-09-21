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

const int INPUT_NODES = 784;        //  кол-во входящих узлов
const int HIDDEN_NODES = 100;       //  кол-во промежуточных узлов
const int OUTPUT_NODES = 10;        //  кол-во исходящих узлов
const int TRAINNIG_RECS = 600;     //  кол-во тренировочных примеров
const int TESTING_RECS = 100;       // кол-во тестовых примеров

typedef cv::Vec<float, INPUT_NODES> input_vec;
typedef cv::Vec<float, HIDDEN_NODES> hidden_vec;
typedef cv::Vec<float, OUTPUT_NODES> output_vec;

class NeuralNetwork {

   public: 
    /**
         * Конструктор
         * Создает новый экземпляр нейронной сети
         *
         * @param float learning_rate - шаг обучения в методе градиентного спуска
    */
    NeuralNetwork(float learning_rate);
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
   std::string outputs() {

     std::stringstream str;
     str << final_output;
     return str.str();
   }


    /**
         * Выводит результат обратного хода
         * 
         * @return результат обратного хода
    */
   void show_backquery(const std::string& label) {

    cv::Mat pic(28, 28, CV_32F, &inputs[0]);
    cv::Mat resized;
    cv::resize(pic, resized, cv::Size(84, 84), cv::INTER_AREA);
    cv::imshow(label, resized);
    cv::waitKey(0);
    cv::destroyAllWindows();
   }

   private:
      const output_vec ONES_O = cv::Mat(output_vec::ones());
      const hidden_vec ONES_H = cv::Mat(hidden_vec::ones());

      const std::map<std::string, std::function<float (float)>> func_map {

        std::make_pair("sigmoid", [] (float x) { return 1 / (1 + exp(-x)); }),
        std::make_pair("softmax", [] (float ) { return 0.0f; }),
        std::make_pair("ReLu", [] (float ) { return 0.0f; }),
        std::make_pair("inv_sigmoid", [](float x) { return log(x / abs(1 - x)); })
      };

      // Матрицы весовых коэффициентов связей.
      cv::Mat wih, who;
      float l_rate;
      input_vec inputs;
      hidden_vec hidden_input, hidden_output, hidden_error; 
      output_vec final_input, final_output, output_error;
      
        // функции активации
      template<typename Tp, int cn>
      void activation_func(const cv::Vec<Tp, cn>& vec_in, 
                                  cv::Vec<Tp, cn>& vec_out, 
                                        const std::string func_name) {

          for (int i=0; i<vec_in.rows; i++) 
              vec_out[i] = func_map.at(func_name) (vec_in[i]);   
      };
};

/************************************************************************************************************/

NeuralNetwork::NeuralNetwork(float learning_rate) {

    l_rate  = learning_rate;
    cv::theRNG().state = time(NULL);

        // веса между входящим и скрытым слоем, выбраны случайно с нормальным распред      
    wih = cv::Mat(HIDDEN_NODES, INPUT_NODES, CV_32F);
    cv::randn(wih, 0.001f, pow(HIDDEN_NODES, -0.5f));
    
    // веса между скрытым и выходным слоем, выбраны случайно с нормальным распред
    who = cv::Mat(OUTPUT_NODES, HIDDEN_NODES, CV_32F);
    cv::randn(who, 0.001f, pow(OUTPUT_NODES, -0.5f));
}

void NeuralNetwork::predict(const input_vec& layer) {

    // рассчитать входящие сигналы для скрытого слоя
  for (int i=0; i<wih.rows; i++)
      hidden_input[i] = (float) wih.row(i).dot(layer.t());

  // рассчитать выходящие сигналы для скрытого слоя
  activation_func(hidden_input, hidden_output, "sigmoid");

  //  рассчитать входящие сигналы для исходящего слоя
  for (int i=0; i<who.rows; i++)
      final_input[i] = (float) who.row(i).dot(hidden_output.t());

  // рассчитать выходящие сигналы для исходящего слоя
  activation_func(final_input, final_output, "sigmoid");
}

void NeuralNetwork::train(const input_vec& input_layer, const output_vec& targets) {

  predict(input_layer);
  // рассчитем производную функции ошибку
  output_error = -(targets - final_output);

  // рассчитаем ошибку скрытого слоя
  // это output_error распределенная пропорционально весовым коэффициентам
  cv::Mat who_t = who.t();
  for (int i=0; i<who_t.rows; i++)
    hidden_error[i] = (float) who_t.row(i).dot(output_error.t());

  // обновить весовые коэффициенты между скрытым и выходным слоем
  output_vec tmp_o = output_error.mul(final_output).mul(ONES_O - final_output);
  for (int i=0; i<tmp_o.rows; i++) {
    for (int j=0; j<hidden_output.rows; j++)
        who.at<float>(i, j) -= l_rate * (tmp_o[i] * hidden_output[j]);
  };

  // обновить весовые коэффициенты между входным и скрытым слоем
  hidden_vec tmp_h = hidden_error.mul(hidden_output).mul(ONES_H - hidden_output);
  for (int i=0; i<tmp_h.rows; i++) {
    for (int j=0; j<input_layer.rows; j++)
        wih.at<float>(i, j) -= l_rate * (tmp_h[i] * input_layer[j]);
  };
}

void NeuralNetwork::back_query(const output_vec& targets) {
  
    activation_func(targets, final_input, "inv_sigmoid");
        
    cv::Mat who_t = who.t();
    for (int i=0; i<who_t.rows; i++)
      hidden_output[i] = (float) who_t.row(i).dot(final_input.t());
    
    double min_val, max_val;
    cv::minMaxIdx(hidden_output, &min_val, &max_val);
    for (int i=0; i<hidden_output.rows; i++) {

      hidden_output[i] -= float(min_val);
      hidden_output[i] /= float(max_val);
      hidden_output[i] *= 0.98f;
      hidden_output[i] += 0.01f;
    } 

    activation_func(hidden_output, hidden_input, "inv_sigmoid");
    
    cv::Mat wih_t = wih.t();
    for (int i=0; i<wih_t.rows; i++)
      inputs[i] = (float) wih_t.row(i).dot(hidden_input.t());

    cv::minMaxIdx(inputs, &min_val, &max_val);
    for (int i=0; i<inputs.rows; i++) {

      inputs[i] -= (float) min_val;
      inputs[i] /= (float) max_val;
      inputs[i] *= 0.98f;
      inputs[i] += 0.01f;
    }
}