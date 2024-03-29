#pragma once

#include <filesystem>
#include "io/saver.h"

namespace io {
template<class NeuroNet> class IO {
private:
	Saver<NeuroNet>* saver{nullptr};
	
public:
	IO(filesystem::path p) {

		if (p.extension().string()==".yml") {
			saver = new YMLsaver<NeuroNet>(p.string());
		} 
		else
			LOGI(cout, "Unknown format "<<p.extension().string()<<'\n') 
	}
	~IO() {if (saver) delete saver;}
	
	inline Saver<NeuroNet>* operator ->(){return saver;}
};}