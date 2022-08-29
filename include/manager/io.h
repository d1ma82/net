#pragma once

#include <filesystem>
#include "io/saver.h"

namespace io {
template<class Matrix> class IO {
private:
	Saver<Matrix>* saver{nullptr};
	
public:
	IO(filesystem::path p) {

		if (p.extension().string()==".yml") {
			saver = new YMLsaver<Matrix>(p.string().c_str());
		} 
		else
			LOGI(cout, "Unknown format "<<p.extension().string()<<'\n') 
	}
	~IO() {if (saver) delete saver;}
	
	inline Saver<Matrix>* operator ->(){return saver;}
};}