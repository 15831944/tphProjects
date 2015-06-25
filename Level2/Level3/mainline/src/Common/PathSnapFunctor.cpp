#include "StdAfx.h"
#include ".\pathsnapfunctor.h"
#include "ARCMathCommon.h"
#include "Line2008.h"
#include "ARCVector.h"
bool PathSnapFunctor::GetSnapPoint( const CPoint2008& pt,const CPoint2008& v_dir, CPoint2008& reslt )
{
	mindist  = ARCMath::DISTANCE_INFINITE;
	for(int i=0;i< thePath.getCount()-1;i++)
	{
		CPoint2008 p1 = thePath.getPoint(i);
		CPoint2008 p2 = thePath.getPoint(i+1);
	
		ARCVector3 vl=pt-p1;
		ARCVector3 vprj=p2-p1;

		DistanceUnit len=  vl.dot(vprj) / vprj.Length();
		if(len >= 0 && len<=vprj.Length() )
		{
			vprj.SetLength(len);
			CPoint2008 prjPt = p1 + vprj;
			DistanceUnit ptDistToLine = prjPt.distance(pt);

			if( ptDistToLine <= mindist && ptDistToLine <= dWidth )
			{
				mindist = ptDistToLine;
				reslt = prjPt;
			}
		}
		//CLine2008 linep1p2(p1,p2);
		//DistanceUnit ptDistToLine =  linep1p2.PointDistance(pt);
		//if( ptDistToLine > dWidth)
		//	continue;
		//
		//CPoint2008 vdir = v_dir;
		////vdir.Normalize();

		//CPoint2008 p2p1 = p1 - p2;
		//CPoint2008 ptp1 = p1 - pt;
		//double D = vdir.getX() *  p2p1.getY() - p2p1.getX() * vdir.getY();
		//if(abs(D) < ARCMath::EPSILON)
		//	continue;

		//double t = (ptp1.getX() *  p2p1.getY() - ptp1.getY() * p2p1.getX() ) / D;
		//double v = (-ptp1.getX() * vdir.getY() + ptp1.getY() * vdir.getX() ) / D;
		//
		//if( v< 0 || v >1)
		// continue;
		//if( ptDistToLine <= mindist )
		//{
		//	reslt = pt + vdir * t;
		//	mindist = ptDistToLine;
		//}
	}
	if(mindist <= dWidth )
		return true;

	return false;
}

PathSnapFunctor::PathSnapFunctor( const CPath2008& path, DistanceUnit width ) : thePath(path), dWidth(width)
{
	
}