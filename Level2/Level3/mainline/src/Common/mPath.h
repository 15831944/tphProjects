#ifndef __MATH_PATH_
#define __MATH_PATH_
#pragma once

#include "mBezierCurve.h"

class mPath : public PointArray
{
public:
	Point StartPoint()const { return *m_vPoints.begin();}
	Point EndPoint()const { return *m_vPoints.rbegin(); }
	
	// 0<t<1
	Point MidCurvePoint(double t) const { return m_Curve->GetMidPoint(t); }
	Point MidLinePoint(double t) const ;
	//
	void InsertPoint(const Point& pt,int k){ insert( begin()+k , pt); }
	void DeletePoint(int k){ erase(begin()+k); }

	// the line length between keypoints
	double LinesLength() const;
	// the curve length by calculous
	double CurveLength() const { return m_Curve->GetLength(); }
	
	double SetCurve(mCurve * otherCurve){ m_Curve(otherCurve); m_Curve->SetKeyPointArray(this); }
private:	
	mCurve * m_Curve;
	
};

#endif