#pragma once

#include "test/test_matrix.h"
#include "test/test_globals.h"
#include "test/photo.h"

namespace test {

void photos() {
    LOGI(cout, "Test photo\n\n")
    LOGI(cout, "Test photo: "<<setw(10)<<test::photo()<<'\n')
}

void matrices() {
    LOGI(cout, "Test matrices\n\n")

    LOGI(cout, "hello:"<<setw(10)<<test::hello()<<'\n')

    LOGI(cout, "get_row:"<<setw(6)<<test::get_row()<<'\n')

    LOGI(cout, "get_col:"<<setw(6)<<test::get_col()<<'\n')
}

void globals() {

    LOGI(cout, "Test globals\n\n")

    LOGI(cout, "lower:"<<setw(6)<<test::to_lower()<<'\n')

    test::conc<int>(2);
}}
