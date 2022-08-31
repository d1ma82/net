#pragma once

#include "test/test_matrix.h"


void test_matrices() {
    
    LOGI(cout, "hello:"<<setw(10)<<(hello()? "OK": "Fail")<<'\n')

    LOGI(cout, "test_get_row:"<<setw(6)<<(test_get_row()? "OK\n": "Fail\n"))

    LOGI(cout, "test_get_col:"<<setw(6)<<(test_get_col()? "OK\n": "Fail\n"))
}
