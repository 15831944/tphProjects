#ifndef __BIZIERCURVE_
#define __BIZIERCURVE_
#include "Common\POINT.H"
#include "Common\POINT2008.H"
#include "Common\ARCVector.h"
#pragma once

class BizierCurve
{
public:	
	void static GenCurvePoints(const std::vector<Point> & input,std::vector<Point> & output,int nOutCounts);
	void static GenCurvePoints(const std::vector<CPoint2008> & input,std::vector<CPoint2008> & output,int nOutCounts);
	void static GenCurvePoints(const std::vector<ARCVector3> & input,std::vector<ARCVector3> & output,int nOutCounts);
private:
	static int ntop;
	static double a[33];
	static double factrl(int n);
	static double Ni(int n,int i);	
	static double Basis(int n,int i,double t);	
};
#endif
