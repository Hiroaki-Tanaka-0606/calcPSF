#include <complex>
using namespace std;


complex<double>** alloc_zmatrix(int m, int n){
	complex<double>* buffer=new complex<double>[m*n];
	for(int i=0; i<m*n; i++){
		buffer[i]=complex<double>(0.0, 0.0);
	}
	complex<double>** mat=new complex<double>*[m];
	for(int i=0; i<m; i++){
		mat[i]=&buffer[i*n];
	}
	return mat;
}
complex<double>** alloc_zmatrix(int n){
	return alloc_zmatrix(n, n);
}

complex<double>*** alloc_zpmatrix(int m, int n){
	complex<double>** buffer=new complex<double>*[m*n];
	complex<double>*** mat=new complex<double>**[m];
	for(int i=0; i<m; i++){
		mat[i]=&buffer[i*n];
	}
	return mat;
}

complex<double>*** alloc_zpmatrix(int n){
	return alloc_zpmatrix(n, n);
}

void delete_zpmatrix(complex<double>*** mat){
	delete[] mat[0];
	delete[] mat;
}

void delete_zmatrix(complex<double>** mat){
	delete[] mat[0];
	delete[] mat;
}



double** alloc_dmatrix(int m, int n){
	double* buffer=new double[m*n];
	for(int i=0; i<m*n; i++){
		buffer[i]=0.0;
	}
	double** ret=new double*[m];
	for(int i=0; i<m; i++){
		ret[i]=&buffer[i*n];
	}
	return ret;
}
double** alloc_dmatrix(int n){
	return alloc_dmatrix(n, n);
}

void delete_dmatrix(double** mat){
	delete[] mat[0];
	delete[] mat;
}

int** alloc_imatrix(int m, int n){
	int* buffer=new int[m*n];
	for(int i=0; i<m*n; i++){
		buffer[i]=0;
	}
	int** ret=new int*[m];
	for(int i=0; i<m; i++){
		ret[i]=&buffer[i*n];
	}
	return ret;
}

double*** alloc_dcube(int l, int m, int n){
	double* buffer=new double[l*m*n];
	for(int i=0; i<l*m*n; i++){
		buffer[i]=0.0;
	}
	double*** ret=new double**[l];
	for(int i=0; i<l; i++){
		ret[i]=new double*[m];
		for(int j=0; j<m; j++){
			ret[i][j]=&buffer[i*m*n+j*n];
		}
	}
	return ret;
}

complex<double>*** alloc_zcube(int l, int m, int n){
	complex<double>* buffer=new complex<double>[l*m*n];
	for(int i=0; i<l*m*n; i++){
		buffer[i]=complex<double>(0.0, 0.0);
	}
	complex<double>*** ret=new complex<double>**[l];
	for(int i=0; i<l; i++){
		ret[i]=new complex<double>*[m];
		for(int j=0; j<m; j++){
			ret[i][j]=&buffer[i*m*n+j*n];
		}
	}
	return ret;
}