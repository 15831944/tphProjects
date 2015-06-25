#pragma once


#include "Path2008.h"
class PathSnapFunctor
{
public:
	PathSnapFunctor(const CPath2008& path, DistanceUnit dWidth);
	bool GetSnapPoint(const CPoint2008& pt,const CPoint2008& vdir,  CPoint2008& reslt);
	DistanceUnit mindist;
protected:
	

	const CPath2008& thePath;
	DistanceUnit dWidth;
};
