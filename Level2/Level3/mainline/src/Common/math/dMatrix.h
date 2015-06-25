/*
*
*
*/
#include "xmath.h"
#include "tVector.h"

#pragma once

template <typename _T>
class dTridagMatrix{
public:
	typedef _T ValueType;
	typedef std::vector<ValueType> ValArray;
	dTridagMatrix(size_t dim):nDim(dim) {
		a.resize(nDim);	b.resize(nDim);	c.resize(nDim);
	}

	void SetDim(int n){
		nDim = n;
		a.resize(nDim);	b.resize(nDim);	c.resize(nDim);
	}

	inline ValueType operator()(int x, int y)const{
		int didx = x - y;
		if( xmath::abs(didx) > 1 ) return ValueType(ZERO);
		if( didx == 0) return b[x];
		if( didx == 1) return a[x];
		if( didx == -1) return c[x];
	}
	//from book <<Numerical Recipes in C++>>
	bool SolveEquations(const ValArray& y,ValArray& x ){
		x.resize(nDim);
		ValueType bet;
		ValArray gam(nDim);
		if( b[0] == ValueType(ZERO) ) return (false);
		x[0]= y[0]/(bet=b[0]);
		for(int j=1;j<(int)nDim;++j){
			gam[j] = c[j-1] /bet;
			bet = b[j]-a[j]*gam[j];
			if( bet == ValueType(0.0) ) return false;
			x[j] = (y[j]-a[j]*x[j-1])/bet;
		}
		for( int j= int(nDim-2);j>=0;--j){
			x[j] -= gam[j+1] * x[j+1]; 
		}
		return true;
	}

	ValArray a;
	ValArray b;
	ValArray c;
	size_t nDim;
};