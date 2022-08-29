#pragma once

#include <map>
#include <sstream>
#include <functional>
#include "entities/full_connected_trainer.h"
#include "manager/io.h"
#include "utils/matrix.h"
#include "globals.h"

using namespace std;

static const char* QUIT=		"quit";
static const char* CREATE=		"create";
static const char* TRAIN=       "train";
static const char* QUERY=		"query";
static const char* REPO=		"repo";
static const char* PRINT=		"print";
static const char* SAVE=        "save";
static const char* LOAD=		"load";
static const char  REM=			'#';

static map<const char*, const char*> helper{
	make_pair(CREATE, "Use: create int input int hidden int final float rate"),
	make_pair(TRAIN, "Use: train int records int epochs"),
	make_pair(QUERY, "Use: query int records"),
	make_pair(REPO, "Use: repo str type ... str images ... [str labels]")
};

static void validate_path(string& path) {
	replace(path.begin(), path.end(), '\\', '/');
}

template<typename T> T get(const char* command, ostream& output, istringstream& istr, bool throw_on_eof=false) {
	
	// TODO: Неправильно работает в случае: "create 784 100 10 " когда есть пробел в конце
	if (throw_on_eof and istr.eof()) error(ER_SYNTAX, helper[command]);
	
	T t{}; istr>>t;
	
	if (istr.fail() and not istr.eof()) {
		istr.clear();
		error(ER_SYNTAX, helper[command]);
	}
	return t;
}

template<class NeuroNetTrainer> class Interpreter {
private:
	NeuroNetTrainer* trainer{nullptr};
	istream& input;
	ostream& output;
	
	void command_print() {
		if (trainer==nullptr) error(ER_NULLPTR, "command_print, trainer==nullptr");
		LOGI(output, *trainer)
	}

	void command_create(istringstream& istr) {
		
		int input_nodes=  get<int>(CREATE, output, istr, true);
		int hidden_nodes= get<int>(CREATE, output, istr, true);
		int final_nodes=  get<int>(CREATE, output, istr, true);
		double lr=		  get<double>(CREATE, output, istr, true);
		
		trainer= new NeuroNetTrainer(output, input_nodes, hidden_nodes, final_nodes, lr);
		LOGI(output, trainer->get())
	}

	void command_repo(istringstream& istr) {

		if (trainer==nullptr) error(ER_NULLPTR, "command_repo, trainer==nullptr");
		
		do {
			string type = get<string>(REPO, output, istr);
			if (type!="type" and type!="images") error(ER_SYNTAX, helper[REPO]);
			
			string val = get<string>(REPO, output, istr);
			trainer->repo_config_append(type+' '+val);			
		} while (not istr.eof());
	}

	void command_train(istringstream& istr) {
		
		if (trainer==nullptr) error(ER_NULLPTR, "command_train, trainer==nullptr");
		int records= get<int>(TRAIN, output, istr, true);
		int epochs=  get<int>(TRAIN, output, istr, true);

		trainer->train(records, epochs, [&](int total, int read) {
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
		int records= get<int>(QUERY, output, istr, true);
		trainer->query(records);
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
				else error(ER_COMMAND, "Unknown command.\n");
			}
			catch(runtime_error& ex) {LOGI(output, ex.what()) continue;}
			LOGI(output, "> ")
		}
		return EoF;
	}
	
	~Interpreter() {if (trainer) delete trainer;}
};