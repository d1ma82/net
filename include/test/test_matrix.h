#pragma once

#include "utils/matrix.h"
#include "globals.h"

using namespace std;

bool hello() {return true;}; 

bool test_get_row() {

    int* data = new int[6] {1,2,3,4,5,6};
    Matrix<int> mtx(2,3, data);
   // cout << mtx<<'\n';
    vector<Matrix<int>::iterator> row = mtx.get_row(1);
    //print_vec<Matrix<int>::iterator>(row);
    return row.size()==3 ? *row[0]==4 and *row[1]==5 and *row[2]==6: false;
}

bool test_get_col() {

    int* data = new int[6] {1,2,3,4,5,6};
    Matrix<int> mtx(2,3, data);
   // cout << mtx<<'\n';
    vector<Matrix<int>::iterator> col = mtx.get_col(0);
    //print_vec<Matrix<int>::iterator, true>(col);
    return col.size()==2 ? *col[0]==1 and *col[1]==4: false;
}