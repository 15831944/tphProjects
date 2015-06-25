/*
*
*
*/
#include "xmath.h"
#include "tVector.h"

#pragma once
template <typename _T ,int _IntM, int _IntN>
class tMatrix
{
public:
	typedef _T ValueType;
	const static int mDim = _IntM;
	const static int nDim = _IntN;
	const static int nSize = mDim * nDim;
	typedef tMatrix<_T,_IntM,_IntN> MatrixType;
	typedef tMatrix<_T,_IntN,_IntM> TransposeType;
	// (M-1) x (N-1) matrix type
	typedef tMatrix<ValueType,_IntM-1,_IntN-1 > MinorMatType;

	MatrixType& Self(){ return *this; }
	const MatrixType& Self()const{ return *this; }

	//can be access as m[][];
	ValueType * operator[](int i) { return val[i]; }
	const ValueType* operator[](int i)const  { return val[i]; }	

	bool inline operator !=(const MatrixType& rM)const{
		if( this == &rM) return false;
		for(int i=0;i<mDim;++i)
			for(int j=0;j<nDim;++j)
				if( (*this)[i][j]!=rM[i][j] )return true;
		return false;
	}

	bool inline operator ==(const MatrixType& rM)const	{
		return !((*this)!=rM);
	}

	//constructors
	inline explicit tMatrix(){
		for(int i=0;i<nSize;++i)
			_val[i] = ValueType(0);
		if(nDim==mDim)
			for(int i=0;i<nDim;++i)
				val[i][i]=ValueType(1);
	}

	template<class _T2>
	inline explicit tMatrix( const tMatrix<_T2,mDim,nDim>& rMatrix ){
		for(int i=0;i<nSize;++i)
			_val[i] = ValueType(rMatrix._val[i]);
	}

	inline explicit tMatrix(  ValueType*const rVal ){
		for(int i=0;i<nSize;++i)
			_val[i] = rVal[i];					
	}			

	// transpose of M
	inline TransposeType GetTranspose() const{
		TransposeType mt;
		for(int i=0;i<mDim;++i)
			for(int j=0;j<nDim;++j)
				mt[j][i] = (*this)[i][j];
		return mt;
	}

	// return remain minor matrix
	inline MinorMatType GetRemainMinorMatrix(int x,int y) const	{
		MinorMatType reslt;

		int ix =0;
		for(int i=0;i<mDim;++i)
		{
			if(i==x){ continue; }
			int iy =0;
			for(int j=0;j<nDim;++j)
			{
				if(j==y) { continue; }
				reslt[ix][iy] = (*this)[i][j];
				++iy;
			}
			++ix;
		}
		return reslt;
	}

	//io
	inline friend std::ostream& operator <<	( std::ostream& o, const MatrixType& m ){
		o << "tMatrix: "<<std::endl;
		int i,j;
		for( i=0;i<mDim-1;++i){
			for(j=0;j<nDim;j++)
				o<< m[i][j]<<' ';
			o<<std::endl;
		}
		for(j=0;j<nDim;++j)
			o<<m[i][j]<<' ';				
		return o;
	}

public:
	union{
		ValueType val[mDim][nDim];
		ValueType _val[nSize];
	};
};	

//  M * M
template<typename _T, int _IntM,int _IntK, int _IntN>
inline tMatrix<_T,_IntM,_IntN> operator * (const tMatrix<_T,_IntM,_IntK>& lM, const tMatrix<_T,_IntK,_IntN>& rM){
	tMatrix<_T,_IntM,_IntN> reslt;
	const static int mDim = _IntM;
	const static int nDim = _IntN;
	const static int kDim = _IntK;

	for(int i=0;i<mDim;++i){
		for(int j=0;j<nDim;++j){
			for(int k=0;k<kDim;++k)
				reslt[i][j] += lM[i][k]*rM[k][j]; 
		}
	}
	return reslt;
}		

// M * V
template<typename _T, int _IntM, int _IntN>
inline tVector<_T,_IntM> operator* (const tMatrix<_T,_IntM,_IntN>& m, const tVector<_T,_IntN>& v ){
	const static int mDim = _IntM;
	const static int nDim = _IntN;
	tVector<_T,_IntM> reslt;

	for(int i=0;i<mDim;++i){
		for(int j=0;j<nDim;++j){
			reslt[i] += m[i][j] * v[j];
		}
	}

	return reslt;
}

//V * M
template<typename _T, int _IntM, int _IntN>
inline tVector<_T,_IntN> operator* (const tVector<_T,_IntM>& v, const tMatrix<_T,_IntM,_IntN>& m ){
	const static int mDim = _IntM;
	const static int nDim = _IntN;
	tVector<_T,_IntN> reslt;

	for(int i=0;i<nDim;++i){
		for(int j=0;j<mDim;++j)
			reslt[i] += v[j] * m[j][i];
	}
	return reslt;
}

//Determinant of M
template<class _T,int _IntN >
inline _T getDeterminant(const tMatrix<_T,_IntN,_IntN>& m){
	typedef _T ValueType;
	const static int nDim = _IntN;

	ValueType reslt = ValueType(0);
	for(int i=0;i<nDim;++i){
		reslt += ( (i)%2==0 ? ValueType(1):-ValueType(1) ) * m[i][0] * getDeterminant( m.GetRemainMinorMatrix(i,0) );
	}
	return reslt;
}	

template<class _T>					
inline _T getDeterminant(const tMatrix<_T,1,1>& m){
	return m[0][0];
}

//Identity the tMatrix
template<class _T,int _IntN >
inline tMatrix<_T,_IntN,_IntN>& identity( tMatrix<_T,_IntN,_IntN>& m ){
	const static int nDim = _IntN;
	zero( m );
	for(int i=0;i<nDim;++i)
		m[i][i] = _T(1);
	return m;
}

template<class MatrixType>
inline MatrixType& zero( MatrixType& m ){
	const static int mDim = MatrixType::mDim;
	const static int nDim = MatrixType::nDim;
	typedef MatrixType::ValueType ValueType; 
	for(int i=0;i<mDim;++i)
		for(int j=0;j<nDim;++j)
			m[i][j] =  ValueType(0);
	return m;
}