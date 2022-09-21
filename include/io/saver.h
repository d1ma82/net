#pragma once

#include <opencv2/core.hpp>
#include <entities/full_connected_net.h>

template<class NeuroNet> class Saver {
public:
	virtual ~Saver()=default;
	virtual void save(const NeuroNet&)=0;
	virtual NeuroNet& load()=0;
protected:
	Saver(){};
};


template<class NeuroNet> class YMLsaver: public Saver<NeuroNet> {
private:
	string file;
public:	
	YMLsaver(const string path): file{path} {}

	void save(const NeuroNet& net) final {
		
		cv::FileStorage fs(file, cv::FileStorage::WRITE);
		fs << "type" << net.type
		   << "lr" << net.learning_rate 
		   << "input_nodes" << net.input_nodes() 
		   << "hidden_nodes" << net.hidden_nodes()
		   << "final_nodes" << net.final_nodes();
	   
		cv::Mat cv_hidden_weights(net.hidden_weights->size() , 1, CV_64F, net.hidden_weights->data);
		fs << "hidden_weights" << cv_hidden_weights;
	
		cv::Mat cv_final_weights(net.final_weights->size(), 1, CV_64F, net.final_weights->data);
		fs << "final_weights" << cv_final_weights;
	
		fs.release();
	}
	NeuroNet& load() final {
		
		double lr;
		string type;
		int input_nodes, hidden_nodes, final_nodes;
		cv::Mat cv_hidden_weights, cv_final_weights;
		cv::FileStorage fs(file, cv::FileStorage::READ);
		if (not fs.isOpened()) {LOGE("Could not load net.\n") throw ERROR;}
		
		fs["lr"] >> lr;
		fs["type"] >> type;
		fs["input_nodes"] >> input_nodes;
		fs["hidden_nodes"] >> hidden_nodes;
		fs["final_nodes"] >> final_nodes;
		fs["hidden_weights"] >> cv_hidden_weights;
		fs["final_weights"] >> cv_final_weights;
	
		NeuroNet* net = new NeuroNet(input_nodes, hidden_nodes, final_nodes, lr, cout, type);

		memcpy (&net->hidden_weights->data[0], 
				&cv_hidden_weights.data[0],
				sizeof(typename NeuroNet::value_type)*net->hidden_weights->size());
			
		memcpy (&net->final_weights->data[0], 
			   &cv_final_weights.data[0], 
				sizeof(typename NeuroNet::value_type)*net->final_weights->size());
	
		fs.release();
		return *net;
	}
};