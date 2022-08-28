#pragma once
#include <iostream>
#include <iomanip>
#include <chrono>

enum {ER_RANGE=-6, ER_SYNTAX, ER_COMMAND, ER_FILE, ER_NULLPTR, ERROR, OK, EoF};

inline void error(int code, const char* s) {
	
	std::stringstream str;
	str<<code<<". "<<s;
	throw std::runtime_error(str.str());
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