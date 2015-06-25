/*
*
*
*/
#include "xmath.h"
#include "tVector.h"
#include "tMatrix.h"
#include "dMatrix.h"


#pragma once

//Polynome
template <typename _T, int _IntN> 
class tPolynome{
public:
	typedef _T ValueType;
	const static int nDim = _IntN;
	typedef tPolynome<_T ,nDim-1> PriorPoly;        // differential of the Polynome
	//typedef tPolynome<_T, 1> ConstPoly;

	inline PriorPoly getDiff()const{
		PriorPoly reslt;
		for(int i=0;i<nDim;++i){
			reslt.coff[i] = coff[i+1] * ValueType(i+1);
		}
		return reslt;
	}

	operator ValueType*() { return coff; }
	operator const ValueType*() const { return coff; }	

public:	

	template <typename ArgType>
	inline ValueType eval(const ArgType& arg )const {
		ValueType tmp = ValueType(coff[nDim]);
		for(int i= nDim-1;i>=0;--i){
			tmp *= ValueType(arg);
			tmp += coff[i];
		}
		return ValueType(tmp);
	}

protected:
	ValueType coff[nDim+1];          //cofficent ascending order
};


template <typename _T>
class tCubicSpline{

public:
	typedef _T ValueType;
	typedef std::vector<ValueType> PointList; 
	typedef tPolynome< ValueType,3 > Polynome3;
	typedef std::vector< Polynome3 > Evaluator;
	typedef double ArgType;
		
public:		
	// the t indicate the point number and the offset of the point 
	inline ValueType eval(ArgType t){
		
		if(evaluator.size()<1)Calculate();
		int nDim = (int) evaluator.size();
		assert(nDim>0);

		int nt = int(t);
		ArgType dt = t - ArgType(nt);
		if(nt>nDim) nt = nDim; 
		if(nt<0) return ctrlpoints[0];
		if(nt==nDim) return *ctrlpoints.rbegin();
		return evaluator[nt].eval(dt);
	}

	inline ValueType evaldiff(ArgType t){
		
		if(evaluator.size()<1)Calculate();
		int nDim = (int) evaluator.size();
		assert(nDim>0);

		int nt = int(t);
		ArgType dt = t - ArgType(nt);
		if(nt>nDim) nt = nDim; 
		if(nt<0) return ValueType(ZERO);
		if(nt==nDim) return ValueType(ZERO);
		return evaluator[nt].getDiff().eval(dt);
	}

	inline ValueType evaldiff_diff(ArgType t){		
		if(evaluator.size()<1)Calculate();
		int nDim = (int) evaluator.size();
		assert(nDim>0);

		int nt = int(t);
		ArgType dt = t - ArgType(nt);
		if(nt>nDim) nt = nDim; 
		if(nt<0) return ValueType(ZERO);
		if(nt==nDim) return ValueType(ZERO);
		return evaluator[nt].getDiff().getDiff().eval(dt);
	}

public:
	PointList ctrlpoints;
	void SetControlPoints(typename PointList::const_iterator _begin,typename PointList::const_iterator _end){ ctrlpoints = PointList(_begin,_end);	}
	//void SetClose(bool _bclose){ bclose = _bclose; }
	tCubicSpline(){ bclose = false; }

protected:
	bool bclose;	
	Evaluator evaluator;  
	void Calculate();
};

//calculate the evaluators;
template<typename _T> 
void tCubicSpline<_T>::Calculate(){

	int nDim = (int) ctrlpoints.size();

	if(nDim<2) return;
	std::vector<ValueType> D; D.resize(nDim);
	std::vector<ValueType> Y; Y.resize(nDim);
	
	// calculate the D;
	if(!bclose){
		Y[0] = 3 * (ctrlpoints[1] - ctrlpoints[0]);
		for(int i=1;i<nDim-1;++i){
			Y[i] = 3 * (ctrlpoints[i+1] - ctrlpoints [i-1]);
		}
		Y[nDim-1] = 3 * (ctrlpoints[nDim-1] - ctrlpoints[nDim-2]);
		//
		dTridagMatrix<ValueType> mat(nDim);

		for(int i=0;i<nDim;i++){
			mat.c[i] = ValueType(1);
			mat.a[i] = ValueType(1);
			mat.b[i] = ValueType(4);
		}
		mat.b[0] = mat.b[nDim-1] = ValueType(2);
		mat.SolveEquations(Y,D);	
	}

	// calculate the polynome
	evaluator.resize(nDim-1);
	for(int i=0;i<nDim-1;++i){
		ValueType dp = ctrlpoints[i+1]-ctrlpoints[i];

		evaluator[i][0] = ctrlpoints[i];
		evaluator[i][1] = D[i];
		evaluator[i][2] = 3 * dp - 2 * D[i] - D[i+1];
		evaluator[i][3] = D[i] + D[i+1] - 2 * dp ;
	}

}
