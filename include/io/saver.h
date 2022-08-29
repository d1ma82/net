#pragma once

#include <opencv2/core.hpp>
#include <entities/full_connected_net.h>

template<class Matrix> class Saver {
public:
	virtual ~Saver()=default;
	virtual void save(const Net<Matrix>&)=0;
	virtual Net<Matrix>& load()=0;
protected:
	Saver(){};
};


template<class Matrix> class YMLsaver: public Saver<Matrix> {
private: 
	const char* path;
public:	
	YMLsaver(const char* path):path{path} {}

	void save(const Net<Matrix>& net) final {
	
		cv::FileStorage fs(path, cv::FileStorage::WRITE);
		fs << "lr" << net.learning_rate 
		   << "input_nodes" << net.input_nodes() 
		   << "hidden_nodes" << net.hidden_nodes()
		   << "final_nodes" << net.final_nodes();
	   
		cv::Mat cv_hidden_weights(net.hidden_weights->size() , 1, CV_64F, net.hidden_weights->data);
		fs << "hidden_weights" << cv_hidden_weights;
	
		cv::Mat cv_final_weights(net.final_weights->size(), 1, CV_64F, net.final_weights->data);
		fs << "final_weights" << cv_final_weights;
	
		fs.release();
	}
	Net<Matrix>& load() final {
		
		double lr;
		int input_nodes, hidden_nodes, final_nodes;
		cv::Mat cv_hidden_weights, cv_final_weights;
		cv::FileStorage fs(path, cv::FileStorage::READ);
		if (not fs.isOpened()) {LOGE("Could not load net.\n") throw ERROR;}
		
		fs["lr"] >> lr;
		fs["input_nodes"] >> input_nodes;
		fs["hidden_nodes"] >> hidden_nodes;
		fs["final_nodes"] >> final_nodes;
		fs["hidden_weights"] >> cv_hidden_weights;
		fs["final_weights"] >> cv_final_weights;
	
		Net<Matrix>* net = new Net<Matrix>(input_nodes, hidden_nodes, final_nodes, lr, cout);

		memcpy (&net->hidden_weights->data[0], 
				&cv_hidden_weights.data[0],
				sizeof(typename Matrix::value_type)*net->hidden_weights->size());
			
		memcpy (&net->final_weights->data[0], 
			   &cv_final_weights.data[0], 
				sizeof(typename Matrix::value_type)*net->final_weights->size());
	
		fs.release();
		return *net;
	}
};