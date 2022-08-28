#pragma once

#include <numeric>
#include "globals.h"

constexpr double e 	  = 2.71828183;

template<typename T> struct Matrix {
	
	using value_type=T;
	using iterator_type=T*;
	
	int rows, cols;
	T* data;
	
	Matrix(): cols{0}, rows{0}, data{nullptr}{}
	Matrix(int rows, int cols): rows{rows}, cols{cols}{data=new T[rows*cols];}
	Matrix(int rows, int cols, T* data): rows{rows}, cols{cols}, data{data}{}
	
	~Matrix(){

		if (data) {
			//LOG("%s; Delete %d x %d \n", "LOG: " __FILE__, rows, cols)
			delete[] data;
			data = nullptr;
		}
	}
	
	inline T& operator[](int index) {return data[index];}
	inline const T& operator[](int index) const {return data[index];} // for constant references
	int maxI() {
		
		T* max_el = max_element(&data[0], &data[size()]);
		return size() - (&data[size()] - max_el);
	}
	inline size_t size() const {return rows*cols;}
	iterator_type begin() {return &data[0];}
	iterator_type end() {return &data[rows*cols];}
}; 

template<typename T> using Mat=std::unique_ptr<Matrix<T>>;

template<typename T> void Not_Compatible(const Matrix<T>& A, const Matrix<T>& B, const char* rem) {
	stringstream str;
	str<<"Not compatible matrices for "<<rem<<' '<<A.rows<<A.cols<<" and "<<B.rows<<B.cols;
	error(ER_RANGE, str.str().c_str());
}

template<typename T> std::ostream& operator <<(std::ostream& os, const Mat<T>& mat) {
	
	const Matrix<T>& matr = *mat;
	if (not matr.data) return os << "No data\n";
	for (int i=0; i<matr.rows; i++) {
		for (int j=0; j<matr.cols; j++) {
		
			os << matr[i*matr.cols+j] << ", ";
		}
		os << '\n';
	}
	return os;
}

template<typename T> Mat<T> operator -(const Mat<T>& mA, const Mat<T>& mB) {
	
	const Matrix<T>& A = *mA;
	const Matrix<T>& B = *mB;

	if ((A.rows != B.rows) or (A.cols != B.cols)) Not_Compatible<T>(A, B, "-");
	Mat<T> R = make_unique<Matrix<T>>(A.rows, A.cols, new T[A.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = A[i] - B[i]; 
	return R;
}

template<typename T> Mat<T> operator -(const T A, const Mat<T>& mB) {
	
	const Matrix<T>& B = *mB;
	
	Mat<T> R = make_unique<Matrix<T>>(B.rows, B.cols, new T[B.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = A - B[i]; 
	return R;
}

template<typename T> Mat<T> operator +(const Mat<T>& mA, const Mat<T>& mB) {
	
	const Matrix<T>& A = *mA;
	const Matrix<T>& B = *mB;
	
	if ((A.rows != B.rows) or (A.cols != B.cols)) Not_Compatible<T>(A, B, "+");
	Mat<T> R = make_unique<Matrix<T>>(A.rows, A.cols, new T[A.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = A[i] + B[i]; 
	return R;
}

template<typename T> Mat<T>& operator +=(Mat<T>& mA, const Mat<T>& mB) {
	
	Matrix<T>& A = *mA;
	const Matrix<T>& B = *mB;
	
	if ((A.rows != B.rows) or (A.cols != B.cols)) Not_Compatible<T>(A, B, "+=");
	for (int i=0; i<A.size(); i++) A[i] += B[i]; 
	return mA;
}

template<typename T> Mat<T> transpose(const Mat<T>& mA) {

	const Matrix<T>& A = *mA;
	Mat<T> R = make_unique<Matrix<T>>(A.cols, A.rows, new T[A.size()]);

	for (int i=0; i<R->rows; i++) 
		for (int j=0; j<R->cols; j++) 
			R->data[i*R->cols+j] = A[j*A.cols+i];
		
	return R;
}

template<typename T> Mat<T> operator *(const Mat<T>& mA, const Mat<T>& mB) {

	const Matrix<T>& A = *mA;
	const Matrix<T>& B = *mB;
	
	if (A.cols != B.rows) Not_Compatible<T>(A, B, "*");
	
	Mat<T> R = make_unique<Matrix<T>>(A.rows, B.cols, new T[A.rows*B.cols]);
	Mat<T> mBT = transpose(mB); // when transpose cols A and B are equal
	Matrix<T>& BT = *mBT;
	
	for (int i=0; i<R->rows; i++) {
		for (int j=0; j<R->cols; j++) {
			
			R->data[i*R->cols+j]=
				inner_product(&A[i*A.cols], &A[i*A.cols+A.cols], &BT[j*BT.cols], (T)0);
		}
	}
	return R;
}

template<typename T> Mat<T> operator *(const T A, const Mat<T>& mB) {
	
	const Matrix<T>& B = *mB;
	
	Mat<T> R = make_unique<Matrix<T>>(B.rows, B.cols, new T[B.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = A * B[i]; 
	return R;
}

template<typename T> Mat<T> mul(const Mat<T>& mA, const Mat<T>& mB) {
	
	const Matrix<T>& A = *mA;
	const Matrix<T>& B = *mB;
	
	if ((A.rows != B.rows) or (A.cols != B.cols)) Not_Compatible<T>(A, B, "mul");
	Mat<T> R = make_unique<Matrix<T>>(A.rows, A.cols, new T[A.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = A[i] * B[i]; 
	return R;	
} 

template<typename T> Mat<T> sigmoid(const Mat<T>& mA) {

	const Matrix<T>& A = *mA;
	Mat<T> R = make_unique<Matrix<T>>(A.rows, A.cols, new T[A.size()]);
	for (int i=0; i<R->size(); i++) R->data[i] = 1/(1+pow(e, -A[i]));
	return R;
}