#pragma once

#include <ranges>
#include <concepts>

namespace test {

bool to_lower() {

    string s{".JpG"};
    string r(lower(s));
    return r==".jpg";
}

template<integral T> 
void conc(T a)  {
 
}
}