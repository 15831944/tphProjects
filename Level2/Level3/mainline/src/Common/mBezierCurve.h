#ifndef __MATH_BEZIERCURVE_
#define __MATH_BEZIERCURVE_
#include "mFunctor.h"
#include "mCurve.h"
#include <vector>

class mBezierCurve : public mCurve
{
public:
	virtual double GetLength( int n = 100 ) const;
	virtual Point GetMidPoint(double t) const;	
	
protected:	
	double Basis(int n,int i,double t) const;    //t^i * (1-t)^(n-i) * Ni(n,i) 
private:
	
};

#endif