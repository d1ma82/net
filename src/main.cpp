#include <iostream>
#include <fstream>
#include <cstring>
#include <thread>

#include "manager/interpreter.h"

using namespace std;

typedef FullConnectedTrainer<Matrix<double>> fc_matrix;

template<class NeuroNetTrainer> using interpreter=Interpreter<NeuroNetTrainer>;

template<class NeuroNetTrainer> void run_command_mode() {
	
	interpreter<NeuroNetTrainer> interp(cin, cout);
	while (true) {
		
		cout << "> ";
		if (interp.run()==0) break;
	}
}

template<class NeuroNetTrainer> int run_file_mode(const char* filename, ostream& output) {
			
	ifstream file{filename};
	if (not file.is_open()) {
		LOGE("Could not open file, "<<filename<<endl)
		return ER_FILE;
	}
	interpreter<NeuroNetTrainer> interp(file, output);
	return interp.run();
}

template<class NeuroNetTrainer> 
	void run_parallel_file_mode(int argc, int idx, char* filenames[], vector<int>& results, vector<ostringstream>& outputs) {

	LOG(cout, "run_parallel_file_mode\n")
	vector<thread> threads;
	results.resize(0);
	outputs.resize(0);

	int i=0;
	while (idx<argc) {

		LOG(cout, "Loop: "<<argc<<','<<idx<<' ')
		outputs.push_back({});
		results.push_back(0);
		thread thrd {[&]() {results[i]=run_file_mode<NeuroNetTrainer>(filenames[idx], outputs[i]); }};
		threads.push_back(move(thrd));
		threads[i].join();
		LOG(cout, filenames[idx]<<endl)
		i++; idx++;
	}
	LOG(cout, "Exit\n")
}

const char* help_str = {
	"net <-p> -fc <-f file> <-fs file1 file 2 ...> # launch full connected net, and use configuration located in file\n"
	"-fc 	full connected net\n"
	"-p 	run parallel\n"
	"-f 	config file or files when parallel\n"
	"-h 	print this\n"
};

inline void help(const char* why){cout<<why<<'\n'<<help_str<<endl;}

		// net -f C:\dima\code\net\db\idx3\conf1.txt
		// net -f C:\dima\code\net\db\mnist\conf.txt
		// net -fc -f D:\git\cpp\net\db\idx3\conf1.txt
		// net -p -fc -f D:\git\cpp\net\db\idx3\conf1.txt
		// net -p -fc -f D:\git\cpp\net\db\idx3\conf1.txt D:\git\cpp\net\db\idx3\conf2.txt
int main(int argc, char* argv[]) {		
	
	if (argc==1)run_command_mode<fc_matrix>();

	else if (argc==2) {

		if (strcmp(argv[1], "-h")==0) help("");
		else if (strcmp(argv[1], "-fc")==0) run_command_mode<fc_matrix>();
		else help(argv[1]);
	} 
	else if (argc==4) {

		if (strcmp(argv[1], "-fc")==0) 
			if (strcmp(argv[2], "-f")==0) 
				return run_file_mode<fc_matrix>(argv[3], cout);	
			else help(argv[2]);
		else help(argv[1]);		
	} 
	else if (argc==5 || argc==6) {

		if (strcmp(argv[1], "-p")==0) {
			if (strcmp(argv[2], "-fc")==0) {

				if (strcmp(argv[3], "-f")==0) {

					vector<int> results;
					vector<ostringstream> outputs;
					run_parallel_file_mode<fc_matrix>(argc, 4, argv, results, outputs);

					for (int i=0; auto& str: outputs) 
						cout<<str.str()<<'\n'<<"result: "<<results[i++]<<'\n';
				}
				else help(argv[3]);	
			}
			else help(argv[2]);
		}
		else help(argv[1]);
	}
	else help("wrong number of argc");
}