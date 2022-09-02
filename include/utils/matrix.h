#pragma once

#include <iterator>
#include <vector>
#include <numeric>

constexpr double e 	  = 2.71828183;

template<typename T> struct Matrix {
	
	using value_type=T;
	using iterator=T*;

	int rows, cols;
	T* data;

	Matrix(): cols{0}, rows{0}, data{nullptr}{}
	Matrix(int rows, int cols): rows{rows}, cols{cols} {data=new T[rows*cols];}
	Matrix(int rows, int cols, T* data): rows{rows}, cols{cols}, data{data}{}
	
	~Matrix() {if (data) delete[] data;}

	inline size_t size() const {return rows*cols;}
	inline T& operator[](int index) {return data[index];}
	inline const T& operator[](int index) const {return data[index];} // for constant references

	int maxI() const {
		
		T* max_el = max_element(&data[0], &data[size()]);
		return size() - (&data[size()] - max_el);
	}
	
	vector<iterator> get_row(int row) {

		vector<iterator> res(cols);
		for (int offcet=0; auto& r:res) r=&data[row*cols]+offcet++;
		return move(res);
	}

	vector<iterator> get_col(int col) {

		vector<iterator> res(rows);
		for (int offcet=0; auto& r:res) {r=&data[col]+offcet; offcet+=cols;}
		return move(res);
	}

	const iterator begin() const {return &data[0];}
	const iterator end() const {return &data[size()];}
}; 

template<typename T> using Mat=std::unique_ptr<Matrix<T>>;

template<typename T> void Not_Compatible(const Matrix<T>& A, const Matrix<T>& B, const char* rem) {
	stringstream str;
	str<<"Not compatible matrices for "<<rem<<' '<<A.rows<<A.cols<<" and "<<B.rows<<B.cols;
	error(ER_RANGE, str.str().c_str());
}

template<typename T> std::ostream& operator <<(std::ostream& os, const Matrix<T>& mat) {
	
	if (not mat.data) return os << "No data\n";
	for (int i=0; i<mat.rows; i++) {
		for (int j=0; j<mat.cols; j++) {
		
			os << mat[i*mat.cols+j] << ", ";
		}
		os << '\n';
	}
	return os;
}

template<typename T> std::ostream& operator <<(std::ostream& os, const Mat<T>& mat) {
	
	const Matrix<T>& matr = *mat;
	os<<matr;
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