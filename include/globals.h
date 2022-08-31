#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include <chrono>

enum {ER_RANGE=-6, ER_SYNTAX, ER_COMMAND, ER_FILE, ER_NULLPTR, ERROR, OK, EoF};

template<typename M> concept MatrixConcept=requires(M m) {
	typename M::value_type; 
	//typename M::iterator;
	//{begin(m)}-> void*;
	//{end}
};

inline void error(int code, const char* s) {
	
	std::stringstream str;
	str<<code<<". "<<s;
	throw std::runtime_error(str.str());
}

template<typename T, bool deref> void print_vec(std::vector<T>& vec) {

	std::cout<<'\n'<<vec.size()<<":\n";
	for(int i=0; i<vec.size(); i++) {
		
		if (deref) std::cout<<*vec[i]<<',';
		else std::cout<<vec[i]<<',';
	}
}

inline std::string lower(const std::string str) {
	
	std::string result;
	result.resize(str.length());
	for (int i=0; auto ch: str) result[i++]=tolower(ch);
	return result;
}

inline void validate_path(std::string& path) {
	std::replace(path.begin(), path.end(), '\\', '/');
}

#define LOGE(msg) std::cerr<<"ERR: "<<msg;
#define LOGI(ostream, msg) ostream<<msg;

#ifdef LOGGING
inline auto global_log_begin=std::chrono::system_clock::now();
	#define LOG(ostream, fmt) \
		{	\
			auto log_point=std::chrono::system_clock::now();\
			ostream<<std::chrono::duration_cast<std::chrono::milliseconds>(log_point-global_log_begin).count()<<": "<<fmt;\
		}
#else
	#define LOG(fmt) ;
#endif