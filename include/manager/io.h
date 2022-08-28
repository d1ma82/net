#pragma once

#include "io/saver.h"

namespace io {
template<class Matrix> class IO {
private:
	Saver<Matrix>* saver{nullptr};
public:
	IO(const char* format) {
		if (format=="yml") {
			saver = new YMLsaver<Matrix>();
		} 
	}
	~IO() {if (saver) delete saver;}
	
	inline Saver<Matrix>* operator ->(){return saver;}
};}