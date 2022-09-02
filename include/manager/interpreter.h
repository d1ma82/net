#pragma once

#include <functional>
#include "entities/full_connected_trainer.h"
#include "manager/io.h"
#include "utils/matrix.h"

using namespace std;

// Когда используешь в контейнерах, обрати внимание, что при условных проверках
// проверяются указатели, а не строки!

const char* QUIT=		"quit";
const char* CREATE=		"create";
const char* TRAIN=       "train";
const char* QUERY=		"query";
const char* REPO=		"repo";
const char* PRINT=		"print";
const char* SAVE=        "save";
const char* LOAD=		"load";
const char  REM=		'#';
const char* PREP=		"prepare";

static const map<const char*, const char*> helper{
	make_pair(CREATE, "use: create int input int hidden int final float rate"),
	make_pair(TRAIN, "use: train int records int epochs"),
	make_pair(QUERY, "use: query int records"),
	make_pair(REPO, "use: repo str type ... str images ... [str labels]")
};

template<typename T> T get(const char* command, ostream& ostr, istringstream& istr) {
		
	T t; istr>>t;
	
	if (istr.eof()) {
		LOG(ostr, command<< " EOF\n")
	}
	else if (istr.fail()) {
		istr.clear();
		error(ER_SYNTAX, helper.at(command));
	}
	return t;
}

template<class NeuroNetTrainer> class Interpreter {
private:
	NeuroNetTrainer* trainer{nullptr};
	istream& input;
	ostream& output;
	config conf;
		
	void command_print() {
		if (trainer==nullptr) error(ER_NULLPTR, "command_print, trainer==nullptr");
		LOGI(output, *trainer)
	}

	void command_create(istringstream& istr) {
		
		int input_nodes=  get<int>(CREATE, output, istr);
		int hidden_nodes= get<int>(CREATE, output, istr);
		int final_nodes=  get<int>(CREATE, output, istr);
		double lr=		  get<double>(CREATE, output, istr);
		
		trainer= new NeuroNetTrainer(output, input_nodes, hidden_nodes, final_nodes, lr);
		LOGI(output, trainer->get())
	}

	void command_repo(istringstream& istr) {
		
		conf.clear();		
		do {
			string type = get<string>(REPO, output, istr);
			if (not repo::valid(type)) error(ER_SYNTAX, helper.at(REPO));
			
			string value = get<string>(REPO, output, istr);
			conf[type] = value;			
		} while (not istr.eof());
	}

	void command_train(istringstream& istr) {
		
		if (trainer==nullptr) error(ER_NULLPTR, "command_train, trainer==nullptr");
		int records= get<int>(TRAIN, output, istr);
		int epochs=  get<int>(TRAIN, output, istr);

		trainer->train(records, epochs, conf, [&](int total, int read) {
			// Сколько звездочек?
			const int LEN=50;
			float stars=float(read)/total;
			char line[LEN];
			fill(&line[0], &line[int(stars*LEN)], '*');
			line[int(stars*LEN-1)]='\0';
			LOGI(output, line<<int(stars*100)<<'%'<<(read>=total?'\n':'\r'))
		});
	}

	void command_query(istringstream& istr) {
		
		if (trainer==nullptr) error(ER_NULLPTR, "command_query, trainer==nullptr");
		int records= get<int>(QUERY, output, istr);
		trainer->query(records, conf);
	}

	void command_save(istringstream& istr) {
		
		if (trainer==nullptr) error(ER_NULLPTR, "command_save, trainer==nullptr");
		filesystem::path filename; istr>>filename;
		io::IO<typename NeuroNetTrainer::value_type> io(filename);
		io->save(trainer->get());
		LOGI(output, "Net saved.\n")
	}
	void command_load(istringstream& istr) {
		
		if (trainer!=nullptr) error(ERROR, "Warn, clear before load, trainer!=nullptr\n");
		trainer = new NeuroNetTrainer(output);
		filesystem::path filename; istr>>filename;
		io::IO<typename NeuroNetTrainer::value_type> io(filename);
		Net<typename NeuroNetTrainer::value_type>& net = io->load();
		trainer->set(net);
		LOGI(output, net<<' '<<"loaded."<<'\n')
	}
	void command_prepare(istringstream& istr) {

		int w= get<int>(PREP, output, istr);
		int h= get<int>(PREP, output, istr);
		string where= get<string>(PREP, output, istr);
		repo::Repo repo(output, conf);
		repo->prepare(w, h, where.c_str());
	}
	
public:
	Interpreter(istream& input, ostream& output):input{input}, output{output} {};
	
	int run() {
		string command, command_line;
		
		while (not input.eof()) {
			
			getline(input, command_line);
			istringstream istr {command_line};
			istr >> command;
			try { 
				if (command == QUIT) return OK;
				else if (command[0] == REM) LOGI(output, istr.str()<<endl)
				else if (command == REPO) command_repo(istr);
				else if (command == CREATE) command_create(istr);
				else if (command == TRAIN) command_train(istr);
				else if (command == PRINT) command_print();
				else if (command == QUERY) command_query(istr);
				else if (command == SAVE) command_save(istr);
				else if (command == LOAD) command_load(istr);
				else if (command == PREP) command_prepare(istr);
				else error(ER_COMMAND, "Unknown command.\n");
			}
			catch(runtime_error& ex) {LOGI(output, ex.what()) continue;}
			LOGI(output, "> ")
		}
		return OK;
	}
	
	~Interpreter() {if (trainer) delete trainer;}
};