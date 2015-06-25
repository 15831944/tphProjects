#include "StdAfx.h"
#include ".\rayintersectpath.h"
#include <common/ARCVector.h>

bool CRayIntersectPath2D::Get()
{
	//convert path
	m_path.DoOffset(-m_pt);
	m_path.Rotate(-m_dir.AngleFromCoorndinateX());

	for( int i=0;i<(int)m_path.size()-1;i++)
	{
		const ARCVector2& p1 = m_path[i];
		const ARCVector2& p2 = m_path[i+1];
		double miny = MIN(p1.y,p2.y);
		double maxy = MAX(p1.y,p2.y);
		double dpy = maxy-miny;
		if( miny<0 && 0<=maxy )
		{
			double dr = (0-p1.y)/(p2.y-p1.y);			
			{
				double rx = p1.x+ (p2.x-p1.x)*dr;
				if(rx>0)
				{
					OutRet ret;
					ret.m_dist = rx;
					ret.m_indexInpath = i+dr;
					m_Out.push_back(ret);
				}
			}
		}
	}
	
	m_path.Rotate(m_dir.AngleFromCoorndinateX());
	m_path.DoOffset(m_pt);
	std::sort(m_Out.begin(),m_Out.end());

	return !m_Out.empty();
}

CRayIntersectPath2D::CRayIntersectPath2D( const CPoint2008& pt, const CPoint2008& dir, const CPath2008& path ) 
{
	m_pt = ARCVector2(pt.x,pt.y);
	m_dir = ARCVector2(dir.x,dir.y);
	m_path.reserve(path.getCount());
	for(int i=0;i<path.getCount();i++)
		m_path.push_back( ARCVector2(path[i].x, path[i].y) );
}