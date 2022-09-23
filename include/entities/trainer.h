#pragma once

#include "manager/repo.h"
#include "entities/full_connected_net.h"

static void is_valid(int input_nodes, int hidden_nodes, double lr) {
	
	if (input_nodes <= 0 or input_nodes > 1000) {
		error(ER_RANGE,"Input nodes have been in range: 1:1000\n");
	}
	if (hidden_nodes <=0 or hidden_nodes > 1000) {
		error(ER_RANGE,"Hidden nodes have been in range: 1:1000\n");
	}
	if (abs(lr)<delta) error (ER_RANGE, "Learning rate have been not equal zero.\n");
}

typedef struct tag{
	int epochs {0};
	int records {0};
	int time {0};
} statistic;


template<class NeuroNet> class Trainer {
private:
	NeuroNet* net;
	ostream& ostr;
	statistic stat;
	int records_queried{0};
	vector<char> score;							// очки, 1- правильно 0-неправильно
	map<Label, pair<int, int>> symbols;		// символы, всего, кол-во правильно угаданых
	const map<string, Labels> supported {{"digit", digit}};
	string type;

	void do_after() {
		
		this->type=net->type;
		global_scale_factor=sqrt(net->input_nodes());
	}
public:  
	using value_type=NeuroNet;
	
	Trainer(ostream& ostr):ostr{ostr} {};

    Trainer(ostream& ostr, int input_nodes, int hidden_nodes, const string type, double lr): 
		ostr{ostr}, type{type}
	{
		int final_nodes=supported.contains(type)? supported.at(type).size():0; 
		if (final_nodes==0) error(ER_RANGE, (type + " not supported.").c_str());
        is_valid(input_nodes, hidden_nodes, lr);
		net = new NeuroNet(input_nodes, hidden_nodes, final_nodes, lr, ostr, type);
		do_after();
    }

	void set(NeuroNet& net) {
		
		is_valid(net.input_nodes(), net.hidden_nodes(), net.learning_rate);
		this->net=&net; 
		do_after();
	}

    void train(int records, int epochs, const config& conf, function<void (ostream&, int, int)> progress) {

		repo::Repo repo(ostr, conf);
		
		if (records <= 0 or records > repo->total()) records = repo->total();
		LOG(ostr, "Begin train "<<records<<" recs\n")
		int ep=epochs; int k=0;
		auto t1 = chrono::system_clock::now();
		while (epochs-- > 0) {
			for (int n=0; n<records; n++) {
					
				const Data next = repo->get_next();
				net->train(next.image.data, next.label, supported.at(type));
				if (progress) progress(ostr, records*ep, ++k);
			}
		}
		auto t2 = chrono::system_clock::now();
		stat.epochs=	ep;
		stat.records= 	records;
		stat.time = chrono::duration_cast<chrono::milliseconds>(t2-t1).count();
		
		LOG(ostr, setw(2)<<stat.epochs<<
			setw(8)<<stat.records<<
			setw(8)<<stat.time<<" ms\n");
    }

    void query(int records, const config& conf, function<void (ostream&, int, int)> progress) {

        repo::Repo repo(ostr, conf);
	
		if (records <= 0 or records > repo->total()) records = repo->total();
		LOG(ostr, "Begin query "<<records<<" recs\n")
		score.resize(records);
		
		for (int n=0; n<records; n++) {
		
			const Data next = repo->get_next();
			Label result=net->query(next.image.data, supported.at(type));			
			symbols[next.label].first++;
			
			if (next.label == result) {
				score.push_back(1);
				symbols[next.label].second++;  
			}else{
				score.push_back(0);
			}
			if (progress) progress(ostr, records, n+1);
		}
		records_queried=records;
		LOG(ostr, '\n'<<*this)
    }

	void query(const config& conf) {

	}

    inline const NeuroNet& get() const noexcept {return *net;} 
	
	friend std::ostream& operator <<(std::ostream& os, Trainer<NeuroNet>& trainer) {
		
		int eff = trainer.records_queried==0 ? 0: 
						accumulate(trainer.score.begin(), 
										trainer.score.end(), 0) / 
												(float) trainer.records_queried * 100;
		
		for (const auto [sym, count]: trainer.symbols) {

			const auto [all, guess]=count;

			os<<setw(2)<<(trainer.type=="digit"?unsigned(sym)-'0':unsigned(sym))<<':'<<
			setw(7)<<all<<':'<<
			setw(7)<<guess<<':'<<
			setw(9)<<guess/float(all)*100<<"%\n";
		}
		
		os<<"Efficiency: "<<eff<<"%\n";
		return os;
	}
};



