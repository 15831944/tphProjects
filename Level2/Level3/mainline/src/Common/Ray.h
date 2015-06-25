#pragma once
#include "fsstream.h"
#include "point.h"
#include "util.h"
#include "path.h"

/************************************************************************/
/* copy from geometric tools : WM4DistRay3Segment3                                                              */
/************************************************************************/
class COMMON_TRANSFER ARCRay
{
public:
	ARCRay(const Point& orig, const Point& vdir );

	Point GetDistPoint(DistanceUnit dist)const;
	Point Origin;
	Point Direction;
};


class COMMON_TRANSFER DistanceRay3Line3
{
public:
	DistanceRay3Line3(const ARCRay& ray,const Line& line);

	DistanceUnit GetSquared(DistanceUnit dTolerance);
	bool intersects();//ray intersect with line

	DistanceUnit m_fRayParameter;
	DistanceUnit m_fLineParameter;

protected:
	const Line& m_line;
	const ARCRay& m_ray;
};

class COMMON_TRANSFER DistanceRay3Path3
{
public:
	DistanceRay3Path3(const ARCRay& ray, const Path& path);
	DistanceUnit GetSquared(DistanceUnit dTolerance);
	bool intersects();//ray intersect with line

protected:
	const ARCRay& m_ray;
	const Path& m_path;
public:
	DistanceUnit m_fRayParameter;
	DistanceUnit m_fPathParameter;


};