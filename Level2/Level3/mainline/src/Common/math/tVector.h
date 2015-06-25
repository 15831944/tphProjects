/*
*
*
*/
#include "xmath.h"


#pragma once

template <class _T ,int _N>
class tVector
{
public:
	typedef tVector<_T,_N> VectorType;
	typedef _T ValueType;
	const static int nDim = _N;
	inline VectorType& Self(){ return *this; }
	inline const VectorType& Self()const{ return *this; }

	inline operator ValueType*() { return val; }
	inline operator const ValueType*() const { return val; }	

	inline explicit tVector(){
		for(int i=0;i<nDim;i++)
			val[i] = ValueType(0);
	}

	inline explicit tVector(const ValueType& _val){
		for(int i=0;i<nDim;++i) 
			val[i] = _val;
	}

	template <class _T2>
	inline explicit tVector( _T2 r[] ){
		for(int i=0;i<nDim;i++)
			val[i] = ValueType(r[i]);
	}

	template <class _T2, int _M>
	inline explicit tVector( const tVector<_T2,_M> & rkVector ){
		const static int mDim = _M;
		const static int minDim = xmath::min(nDim,mDim);
		const static int maxDim = xmath::max(nDim,mDim);
		for(int i=0;i<minDim;++i)
			val[i] = ValueType(rkVector[i]);
		for(int i=minDim;i<maxDim;++i)
			val[i] = ValueType(0);
	}
	// v == v
	inline bool operator == ( const VectorType& rkVector ) const {
		return ! ( (*this) != rkVector );
	}
	// v != v
	inline bool operator != ( const VectorType& rkVector ) const {
		for(int i=0;i<nDim;i++) 
			if( val[i] != rkVector[i] ) return true;
		return false;
	}
	// v + v
	inline VectorType operator + ( const VectorType& rkVector ) const{
		return VectorType(Self()) += rkVector;
	}
	// v - v
	inline VectorType operator - ( const VectorType& rkVector ) const {
		return VectorType(Self()) -= rkVector;
	}
	// v * v
	inline VectorType operator * ( const VectorType& rkVector ) const {
		return VectorType(Self()) *= rkVector;
	}
	// v / v
	inline VectorType operator / ( const VectorType& rkVector ) const {
		return VectorType(Self()) /= rkVector;
	}
	// v * a
	inline VectorType operator * ( const ValueType& scalar ) const {
		return VectorType(Self()) *= scalar;		
	}
	// v / a
	inline VectorType operator / ( const ValueType& scalar ) const {
		return VectorType(Self()) /= scalar;	
	}				
	// - v
	inline VectorType operator - () const {
		VectorType reslt;
		for(int i=0;i<nDim;i++)
			reslt[i] = -val[i];
		return reslt;
	}
	// a * v
	inline friend VectorType operator * ( const ValueType& fScalar, const VectorType& rkVector ) {
		return rkVector*fScalar;
	}				
	// v += v
	inline VectorType& operator += ( const VectorType& rkVector ) {
		for(int i=0;i<nDim;i++)
			val[i] += rkVector[i];
		return Self();
	}
	// v -= v
	inline VectorType& operator -= ( const VectorType& rkVector ) {
		for(int i=0;i<nDim;i++)
			val[i] -= rkVector[i];
		return Self();
	}
	// v *= a 
	inline VectorType& operator *= ( const ValueType& scalar ) {
		for(int i=0;i<nDim;i++)
			val[i] *= scalar;
		return Self();
	}
	// v *= v
	inline VectorType& operator *= (const VectorType& rkVector) {
		for(int i=0;i<nDim;i++)
			val[i] *= rkVector[i];
		return Self();
	}
	// v /= v
	inline VectorType& operator /= (const VectorType& rkVector) {
		for(int i=0;i<nDim;i++)
			val[i] /= rkVector[i];
		return Self();
	}

	// v /= a 
	inline VectorType& operator /= ( const ValueType& scalar ) {
		for(int i=0;i<nDim;i++)
			val[i] /= scalar;
		return Self();
	}				
	// v.v
	inline ValueType dot(const VectorType& vec) const { 
		ValueType reslt = ValueType(0);
		for(int i=0;i<nDim;i++)
			reslt += val[i] * vec[i];
		return reslt;
	}
	// |v|^2
	inline ValueType GetLength2()const{
		ValueType mag(0);
		for(int i=0;i<nDim;++i)
			mag += val[i]*val[i];
		return mag;
	}
	// |v|
	inline ValueType length()const { return xmath::sqrt(GetLength2());}

	inline VectorType& normalize(){
		ValueType scale = ValueType(1) / length() ;
		return Self() *= scale;
	}
	// unit v 
	inline VectorType GetNormal()const{
		return VectorType(Self()).normalize();
	}			
	//io
	inline friend std::ostream& operator <<	( std::ostream& o, const VectorType& v ){
		o << "tVector [ "; 
		for(int i=0;i<nDim;i++)
			o<< v[i]<<' ';
		o<<"]";
		return o;
	}


public:
	
		ValueType val[nDim];		
	
};

template<class _T>
inline tVector<_T,2> cross(const tVector<_T,2>& rv){
	tVector<_T,2> ret;
	ret[0] = -rv[1]
	ret[1] = rv[0];
	return ret;
}

template<class _T>
inline tVector<_T,3> cross(const tVector<_T,3>& rv, const tVector<_T,3>& lv){
	tVector<_T,3> ret;
	ret[0] = lv[1]*rv[2]-rv[1]*lv[2];
	ret[1] = lv[2]*rv[0]-rv[2]*lv[0];
	ret[2] = lv[0]*rv[1]-rv[0]*lv[1]; 
	return ret;
}

