#pragma once

#include <vector>
#include "manager/repo.h"
#include "entities/full_connected_net.h"

constexpr int MAX_DIGITS=10;

static void is_valid(int input_nodes, int hidden_nodes, int final_nodes, double lr) {
	
	if (input_nodes <= 0 or input_nodes > 1000) {
		error(ER_RANGE,"Input nodes have been in range: 1:1000\n");
	}
	if (hidden_nodes <=0 or hidden_nodes > 1000) {
		error(ER_RANGE,"Hidden nodes have been in range: 1:1000\n");
	}
	if (final_nodes <= 0 or final_nodes > 1000) {
		error(ER_RANGE,"Final nodes have been in range: 1:1000\n");
	}
	if (abs(lr)<delta) error (ER_RANGE, "Learning rate have been not equal zero.\n");
}

typedef struct tag{
	int read{0};
	int records {0};
	int time {0};
	int total_records {0};
	int total_time {0};
} statistic;


template<class Matrix> class FullConnectedTrainer {
private:
	Net<Matrix>* net;
	ostream& ostr;
	repo::config repo_config;
	statistic stat;
	int records_queried{0};
	vector<char> score;								// очки, 1- правильно 0-неправильно
	vector<int> digits {0,0,0,0,0,0,0,0,0,0};		// кол-во цифр
	vector<int> guess {0,0,0,0,0,0,0,0,0,0};		// кол-во правильно угаданых цифр
public:
	using value_type=Matrix;
	
	FullConnectedTrainer(ostream& ostr):ostr{ostr} {};

    FullConnectedTrainer(ostream& ostr, int input_nodes, int hidden_nodes, int final_nodes, double lr): ostr{ostr} {
        
        is_valid(input_nodes, hidden_nodes, final_nodes, lr);
		net = new Net<Matrix>(input_nodes, hidden_nodes, final_nodes, lr, ostr);
    }

    void train(int records, int epochs, function<void (int, int)> progress) {

		repo::Repo repo(ostr, repo_config);
		
		if (records <= 0 or records > repo->total()) records = repo->total();
		
		int ep=epochs; int k=0;
		auto t1 = chrono::system_clock::now();
		while (epochs-- > 0) {
			for (int n=0; n<records; n++) {
			
				if (n == repo->total()) {records=n; break;}		
				const Data& next = repo->get_next();
				net->train(&next.image[0], next.label);
				stat.read++;
				if (progress) progress(records*ep, ++k);
			}
		}
		auto t2 = chrono::system_clock::now();
		stat.records = records;
		stat.total_records += stat.records;
		stat.time = chrono::duration_cast<chrono::milliseconds>(t2-t1).count();
		stat.total_time += stat.time;
    }

    void query(int records) {

        repo::Repo repo(ostr, repo_config);
	
		if (records <= 0 or records > repo->total()) records = repo->total();

		score.resize(records);
	
		for (int n=0; n<records; n++) {
		
			if (n==repo->total()) {records=n; break;}
			const Data& next = repo->get_next();
			auto result = net->query(&next.image[0])->maxI();
			digits[(int)next.label]++; 

			if ((int) next.label == result) {
				score.push_back(1);
				guess[(int)next.label]++;  
			}else{
				score.push_back(0);
			}
		}
		records_queried=records;
    }

	void repo_config_append(const string& config_line) {
		
		istringstream istr{config_line};
		string key, value;
		istr>>key; istr>>value;
		repo_config[key] = value;
	}

    inline const Net<Matrix>& get() const noexcept {return *net;} 
    void set(Net<Matrix>& net) noexcept {this->net=&net;}

	friend std::ostream& operator <<(std::ostream& os, FullConnectedTrainer<Matrix>& trainer) {
		
		int eff = trainer.records_queried==0 ? 0: 
						accumulate(trainer.score.begin(), 
										trainer.score.end(), 0) / 
												(float) trainer.records_queried * 100;
		
		os<<setw(2)<<trainer.net->id<<
			setw(8)<<trainer.stat.records<<
			setw(8)<<trainer.stat.time<<" ms"<<
			setw(8)<<trainer.stat.total_records<<
			setw(8)<<trainer.stat.total_time<<" ms\n";

		for (int i=0; i<MAX_DIGITS; i++) {
			os<<setw(2)<<trainer.net->id<<':'<<
			setw(5)<<i<<':'<<
			setw(5)<<trainer.digits[i]<<':'<<
			setw(5)<<trainer.guess[i]<<':'<<
			setw(9)<<trainer.guess[i]/(float)trainer.digits[i]*100<<"%\n";
		}
		os<<"Efficiency: "<<eff<<"%\n";
		return os;
	}
};



