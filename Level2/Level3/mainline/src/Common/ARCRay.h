#pragma once
#include "ARCVector.h"

class ARCLine;
class ARCPath;
class ARCPolygon;
class ARCPath3;

class ARCRay2
{
public:
	ARCRay2(void){}
	ARCRay2(const ARCVector2& orign,const ARCVector2& dir)
		:m_orign(orign),m_dir(dir){}	

	ARCVector2 m_orign;
	ARCVector2 m_dir;
};


class ARCRay3
{
public:
	ARCRay3(void){}
	ARCRay3(const ARCVector3& orign,const ARCVector3& dir)
		:m_orign(orign),m_dir(dir){}	

		ARCVector3 m_orign;
		ARCVector3 m_dir;
};


class ARCRayIntersect
{
public:

	int Get(const ARCRay2& ray, const ARCLine& path);

	int Get(const ARCRay2& ray, const ARCPath& path);

	int Get(const ARCRay2& ray, const ARCPolygon& path);

	int Get(const ARCRay3& ray, const ARCPoint3& pt, double drad);

	std::vector<double> rayDistList;

};


class ARCPathIntersect
{
public:
	int Get(const ARCPath& path, const ARCPolygon& poly, bool bOnlyFirst = false);

	int Get(const ARCPath3& path, const ARCPolygon& poly, double polyZmin,double polyZmax,  bool bOnlyFirst = false);

	int Get(const ARCPath3& path, const ARCPoint3& pt, double drad, double dFromDist=0, bool bOnlyFirst = false);

	std::vector<double> pathDistList;
	std::vector<double> pathIndexList;
};

