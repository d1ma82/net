#pragma once

#include <random>
#include <memory>
#include <functional>

constexpr double Ze = 0.001;
constexpr double delta = 0.00000001;

typedef unsigned char uchar;

template<MatrixConcept Matrix> struct Net {
	
	using Mat=std::unique_ptr<Matrix>;
	using value_type=typename Matrix::value_type;
	
	const string type;
	ostream& ostr;
	std::default_random_engine engine {};

	double learning_rate;
	Mat input;		// Входящий слой
	Mat target;	// Целевые значения
	Mat hidden_weights;	// Веса между входным и скрытым слоем
	Mat hidden_input; // Скрытый слой вход (сумма сглаженных сигналов со входа)
	Mat hidden_output; // Скрытый слой выход (активация)
	Mat final_weights ; // Веса между скрытым и последним слоем
	Mat final_input; 
	Mat final_output;
	Mat error; // Ошибка (разность между целевым и последним слоем)
	Mat final_error; // Ошибка распределенная между последним и скрытым слоем
	Mat hidden_error; // Ошибка распределенная между скрытым и входным слоем
	Mat array[4]; // Служебные массивы для хранения промежуточных вычислений*/
	
	Net(int input_count, int hidden_count, int target_count, double lr, ostream& ostr, const string type):
		ostr{ostr},
		type{type},
		learning_rate{lr},
		input{std::make_unique<Matrix>(input_count, 1)},
		target{std::make_unique<Matrix>(target_count, 1)},
		final_weights{std::make_unique<Matrix>(target_count, hidden_count)},
		hidden_weights{std::make_unique<Matrix>(hidden_count, input_count)}
	{
		std::fill(target->begin(), target->end(), 0);
		auto distribution = std::normal_distribution{0.0, pow(hidden_count, -0.5)};
		auto gen = [&](){return distribution(engine);};
		std::generate(hidden_weights->begin(), hidden_weights->end(), gen);
		distribution = std::normal_distribution{0.0, pow(target_count, -0.5)};
		std::generate(final_weights->begin(), final_weights->end(), gen);
	}

	~Net() {}
	
	Label query(const uchar* input_data, const Labels& labels) {

		int i=0;
		std::for_each(&input_data[0], &input_data[input->rows], [&](auto k) {input->data[i++]=scale(k);});
		forward();
		i=final_output->maxI();
		for (auto [key, value]: labels) 
				if (value==i) return key;
		return '\0';
	}
	// TODO: Нужно както по другому определять цель
	void train(const uchar* input_data, Label target, const Labels& labels) {
		
		int i=0;
		std::for_each(&input_data[0], &input_data[input->rows], [&](auto k) {input->data[i++]=scale(k);});
		//LOG(ostr, "Label: "<<unsigned(target)<<", index: "<<labels.at(target)<<'\n')
		this->target->data[labels.at(target)] = 0.99;
		forward();
		back_propagation();
		this->target->data[labels.at(target)] = Ze;
	}
	
	inline int input_nodes() const {return input->rows;}
	inline int hidden_nodes() const {return hidden_weights->rows;}
	inline int final_nodes() const {return final_weights->rows;}
	
private:
	
	inline double scale(uchar k) {return k/255.0*0.99+Ze;}
	
	void forward() {
		
		hidden_input = hidden_weights * input;
		hidden_output = sigmoid(hidden_input);
	
		final_input = final_weights * hidden_output;
		final_output = sigmoid(final_input);
	};
	
	void back_propagation() {
		
		error = target - final_output;
	
	// ошибки скрытого слоя - это ошибки errors,
	// распределенные пропорционально весовым коэффициентам связей
		hidden_error = transpose(final_weights) * error;
	
	// обновим весовые коэффициенты связей между скрытым и выходным слоями
		array[0] = mul(error, final_output);
		array[1] = mul(array[0], value_type(1) - final_output);
		array[2] = array[1] * transpose(hidden_output);
		array[3] = learning_rate * array[2];
		final_weights += array[3];
	
	// обновим весовые коэффициенты связей между входным и скрытым
		array[0] = mul(hidden_error, hidden_output);
		array[1] = mul(array[0], value_type(1) - hidden_output);
		array[2] = array[1] * transpose(input);
		array[3] = learning_rate * array[2];
		hidden_weights += array[3];
	};
};

template<typename T> std::ostream& operator <<(std::ostream& os, const Net<T>& net) {
	
	os<<"Net: "<<net.type<<
	", "<<net.input_nodes()<<
	", "<<net.hidden_weights->rows<<'x'<<net.hidden_weights->cols<<
	", "<<net.final_weights->rows<<'x'<<net.final_weights->cols <<
	", "<<net.learning_rate<<
	", "<<sizeof(typename Net<T>::value_type)<<'\n';
	return os;
}