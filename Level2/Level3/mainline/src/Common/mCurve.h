#ifndef __MATH_CURVE_
#define __MATH_CURVE_
#include "Point.h"

typedef std::vector<Point> PointArray;

class mCurve
{
public:
	virtual double GetLength( int n = 100 )const =0;
	virtual Point GetMidPoint(double t) const =0;
	virtual void SetKeyPointArray(PointArray * keyPts){ m_pKeyPoints = keyPts; }
	
protected:
	PointArray * m_pKeyPoints;
};


#endif