#pragma once
#include "path2008.h"
//--------------------------------------------------------------------
//Cubic
class Cubic
{
public:
	Cubic():_a(0),_b(0),_c(0),_d(0){}
	Cubic(double a, double b, double c, double d)
		:_a(a),_b(b),_c(c),_d(d)
	{}

	/** evaluate cubic */
	double eval(double u) {
		return (((_d*u) + _c)*u + _b)*u + _a;
	}
private:
	double _a,_b,_c,_d;  
};


//--------------------------------------------------------------------
//NaturalCubicSpline
class NaturalCubicSpline
{
public:
	static void calcNaturalCubic( const CPath2008* pSrcPath, CPath2008& destPath);
};


//--------------------------------------------------------------------
//ClosedNaturalCubicSpline
class ClosedNaturalCubicSpline
{
public:
	static void calcNaturalCubic(const CPath2008* pSrcPath, CPath2008& destPath, int STEPS = 12);
private:
	static void calcNaturalCubic(int n, double* points, std::vector<Cubic>& vectCubic);
};
