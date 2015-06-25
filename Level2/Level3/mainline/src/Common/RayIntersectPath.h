#pragma once
#include <common/Point2008.h>
#include <common/Path2008.h>
#include <common/ARCVector.h>
#include <common/ARCPath.h>

class CRayIntersectPath2D
{
public:
	CRayIntersectPath2D(const CPoint2008& pt, const CPoint2008& dir, const CPath2008& path);

	bool Get();

	//out 
	struct OutRet
	{
		double m_dist;
		double m_indexInpath;
		bool operator<(const OutRet& other)const{ return m_dist< other.m_dist; }
	};

	std::vector<OutRet> m_Out;
	
	
	ARCVector2 m_pt;
	ARCVector2 m_dir;
	ARCPath m_path;
};
