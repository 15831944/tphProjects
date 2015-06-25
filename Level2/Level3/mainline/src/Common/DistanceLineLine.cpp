#include "StdAfx.h"
#include ".\distancelineline.h"
#include <algorithm>

#include "ARCVector.h"
#include "ARCStringConvert.h"
//
//DistanceLineLine2D::DistanceLineLine2D()
//{
//	m_fLine0Parameter = 0.0;
//	m_fLine1Parameter = 0.0;
//}

DistanceUnit DistanceLineLine3D::GetSquared(const CLine2008& m_rkLine0, const CLine2008& m_rkLine1)
{
	typedef DistanceUnit Real;
	
	ARCVector3 l0dir = m_rkLine0.GetPoint2() - m_rkLine0.GetPoint1();

	ARCVector3 l1dir = m_rkLine1.GetPoint2() - m_rkLine1.GetPoint1();

	CPoint2008 p1,p2;
	m_rkLine0.GetCenterPoint(p1); m_rkLine1.GetCenterPoint(p2);

	DistanceUnit extent0 = 0.5 * l0dir.Length();
	DistanceUnit extent1 = 0.5 * l1dir.Length();

	l0dir[VZ]= l1dir[VZ] = 0;
	l0dir.Normalize();
	l1dir.Normalize();



	ARCVector3 kDiff = p1 - p2;
	DistanceUnit fA01 = -l0dir.dot( l1dir);
	DistanceUnit fB0 = kDiff.dot(l0dir);
	DistanceUnit fB1 = - kDiff.dot(l1dir);
	DistanceUnit fC = kDiff.Magnitude2();
	DistanceUnit fDet = abs((DistanceUnit)1.0 - fA01*fA01);
	DistanceUnit fS0, fS1, fSqrDist, fExtDet0, fExtDet1, fTmpS0, fTmpS1;

	if (fDet >= 0)
	{
		// segments are not parallel
		fS0 = fA01*fB1-fB0;
		fS1 = fA01*fB0-fB1;
		fExtDet0 = extent0*fDet;
		fExtDet1 = extent1*fDet;

		if (fS0 >= -fExtDet0)
		{
			if (fS0 <= fExtDet0)
			{
				if (fS1 >= -fExtDet1)
				{
					if (fS1 <= fExtDet1)  // region 0 (interior)
					{
						// minimum at two interior points of 3D lines
						Real fInvDet = ((Real)1.0)/fDet;
						fS0 *= fInvDet;
						fS1 *= fInvDet;
						fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
							fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
					}
					else  // region 3 (side)
					{
						fS1 = extent0;
						fTmpS0 = -(fA01*fS1+fB0);
						if (fTmpS0 < -extent0)
						{
							fS0 = -extent0;
							fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
								fS1*(fS1+((Real)2.0)*fB1)+fC;
						}
						else if (fTmpS0 <= extent0)
						{
							fS0 = fTmpS0;
							fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
						}
						else
						{
							fS0 = extent0;
							fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
								fS1*(fS1+((Real)2.0)*fB1)+fC;
						}
					}
				}
				else  // region 7 (side)
				{
					fS1 = -extent1;
					fTmpS0 = -(fA01*fS1+fB0);
					if (fTmpS0 < -extent0)
					{
						fS0 = -extent0;
						fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
							fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else if (fTmpS0 <= extent0)
					{
						fS0 = fTmpS0;
						fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else
					{
						fS0 = extent0;
						fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
							fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
				}
			}
			else
			{
				if (fS1 >= -fExtDet1)
				{
					if (fS1 <= fExtDet1)  // region 1 (side)
					{
						fS0 = extent0;
						fTmpS1 = -(fA01*fS0+fB1);
						if (fTmpS1 < -extent1)
						{
							fS1 = -extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
						else if (fTmpS1 <=extent1)
						{
							fS1 = fTmpS1;
							fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
						else
						{
							fS1 = extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
					}
					else  // region 2 (corner)
					{
						fS1 = extent1;
						fTmpS0 = -(fA01*fS1+fB0);
						if (fTmpS0 < -extent0)
						{
							fS0 = -extent0;
							fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
								fS1*(fS1+((Real)2.0)*fB1)+fC;
						}
						else if (fTmpS0 <= extent0)
						{
							fS0 = fTmpS0;
							fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
						}
						else
						{
							fS0 =extent0;
							fTmpS1 = -(fA01*fS0+fB1);
							if (fTmpS1 < -extent1)
							{
								fS1 = -extent1;
								fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
									fS0*(fS0+((Real)2.0)*fB0)+fC;
							}
							else if (fTmpS1 <= extent1)
							{
								fS1 = fTmpS1;
								fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)
									+ fC;
							}
							else
							{
								fS1 = extent1;
								fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
									fS0*(fS0+((Real)2.0)*fB0)+fC;
							}
						}
					}
				}
				else  // region 8 (corner)
				{
					fS1 = -extent1;
					fTmpS0 = -(fA01*fS1+fB0);
					if (fTmpS0 < -extent0)
					{
						fS0 = -extent0;
						fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
							fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else if (fTmpS0 <= extent0)
					{
						fS0 = fTmpS0;
						fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else
					{
						fS0 = extent0;
						fTmpS1 = -(fA01*fS0+fB1);
						if (fTmpS1 > extent1)
						{
							fS1 = extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
						else if (fTmpS1 >= -extent1)
						{
							fS1 = fTmpS1;
							fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)
								+ fC;
						}
						else
						{
							fS1 = -extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
					}
				}
			}
		}
		else 
		{
			if (fS1 >= -fExtDet1)
			{
				if (fS1 <= fExtDet1)  // region 5 (side)
				{
					fS0 = -extent0;
					fTmpS1 = -(fA01*fS0+fB1);
					if (fTmpS1 < -extent1)
					{
						fS1 = -extent1;
						fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
							fS0*(fS0+((Real)2.0)*fB0)+fC;
					}
					else if (fTmpS1 <= extent1)
					{
						fS1 = fTmpS1;
						fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)+fC;
					}
					else
					{
						fS1 = extent1;
						fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
							fS0*(fS0+((Real)2.0)*fB0)+fC;
					}
				}
				else  // region 4 (corner)
				{
					fS1 = extent1;
					fTmpS0 = -(fA01*fS1+fB0);
					if (fTmpS0 > extent0)
					{
						fS0 = extent0;
						fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
							fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else if (fTmpS0 >= -extent0)
					{
						fS0 = fTmpS0;
						fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
					}
					else
					{
						fS0 = -extent0;
						fTmpS1 = -(fA01*fS0+fB1);
						if (fTmpS1 < -extent1)
						{
							fS1 = -extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
						else if (fTmpS1 <= extent1)
						{
							fS1 = fTmpS1;
							fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)
								+ fC;
						}
						else
						{
							fS1 = extent1;
							fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
								fS0*(fS0+((Real)2.0)*fB0)+fC;
						}
					}
				}
			}
			else   // region 6 (corner)
			{
				fS1 = -extent1;
				fTmpS0 = -(fA01*fS1+fB0);
				if (fTmpS0 > extent0)
				{
					fS0 = extent0;
					fSqrDist = fS0*(fS0-((Real)2.0)*fTmpS0) +
						fS1*(fS1+((Real)2.0)*fB1)+fC;
				}
				else if (fTmpS0 >= -extent0)
				{
					fS0 = fTmpS0;
					fSqrDist = -fS0*fS0+fS1*(fS1+((Real)2.0)*fB1)+fC;
				}
				else
				{
					fS0 = -extent0;
					fTmpS1 = -(fA01*fS0+fB1);
					if (fTmpS1 < -extent1)
					{
						fS1 = -extent1;
						fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
							fS0*(fS0+((Real)2.0)*fB0)+fC;
					}
					else if (fTmpS1 <= extent1)
					{
						fS1 = fTmpS1;
						fSqrDist = -fS1*fS1+fS0*(fS0+((Real)2.0)*fB0)
							+ fC;
					}
					else
					{
						fS1 = extent1;
						fSqrDist = fS1*(fS1-((Real)2.0)*fTmpS1) +
							fS0*(fS0+((Real)2.0)*fB0)+fC;
					}
				}
			}
		}
	}
	else
	{
		// The segments are parallel.  The average b0 term is designed to
		// ensure symmetry of the function.  That is, dist(seg0,seg1) and
		// dist(seg1,seg0) should produce the same number.
		Real fE0pE1 = extent0 + extent1;
		Real fSign = (fA01 > (Real)0.0 ? (Real)-1.0 : (Real)1.0);
		Real fB0Avr = ((Real)0.5)*(fB0 - fSign*fB1);
		Real fLambda = -fB0Avr;
		if (fLambda < -fE0pE1)
		{
			fLambda = -fE0pE1;
		}
		else if (fLambda > fE0pE1)
		{
			fLambda = fE0pE1;
		}

		fS1 = -fSign*fLambda*extent1/fE0pE1;
		fS0 = fLambda + fSign*fS1;
		fSqrDist = fLambda*(fLambda + ((Real)2.0)*fB0Avr) + fC;
	}

	//m_kClosestPoint0 = m_rkSegment0.Origin + fS0*m_rkSegment0.Direction;
	//m_kClosestPoint1 = m_rkSegment1.Origin + fS1*m_rkSegment1.Direction;
	if(extent0>0)
		m_fLine0Parameter = fS0/extent0;
	else
		m_fLine0Parameter = 0;
	
	if(extent1>0)
		m_fLine1Parameter = fS1/extent1;
	else 
		m_fLine1Parameter = 0;

	return abs(fSqrDist);
}



bool IntersectLineLine3D::Insects(const CLine2008& l1, const CLine2008& l2, DistanceUnit dTolerance )
{
	DistanceUnit distsquare = GetSquared(l1,l2);
	if(distsquare <= dTolerance * dTolerance ) return true;
	return false;
}



bool IntersectLineLine2D::Intersects(const CLine2008& m_rkLine0,const CLine2008& m_rkLine1, DistanceUnit dTolenrace /*= (std::numeric_limits<DistanceUnit>::min)() */ )
{
	ARCVector2 v1 = ARCVector2(m_rkLine0.GetPoint2() - m_rkLine0.GetPoint1());
	ARCVector2 v2 = ARCVector2(m_rkLine1.GetPoint1() - m_rkLine1.GetPoint2());

	DistanceUnit d1 = v1.Length();
	DistanceUnit d2 = v2.Length();

	v1.Normalize();
	v2.Normalize();

	/*DistanceUnit dTolenrace0 = dTolenrace / v1.Length();
	DistanceUnit dTolenrace1 = dTolenrace / v2.Length();*/

	ARCVector2 vd = ARCVector2(m_rkLine1.GetPoint1() - m_rkLine0.GetPoint1());

	DistanceUnit det = v1[VX] * v2[VY] - v1[VY] * v2[VX];

	m_fLine0Parameter = v2[VY] * vd[VX] - v2[VX] * vd[VY];
	m_fLine1Parameter = -v1[VY] * vd[VX] + v1[VX] * vd[VY];

	m_fLine0Parameter /= det;
	m_fLine1Parameter /= det;
	
	
	if( m_fLine0Parameter < 0 ){   if(m_fLine0Parameter>= - dTolenrace) m_fLine0Parameter = 0;  }
	if( m_fLine0Parameter > d1 ){   if( m_fLine0Parameter<= d1 +  dTolenrace) m_fLine0Parameter = d1; }
	
	if( m_fLine1Parameter < 0 ){   if(m_fLine1Parameter>= - dTolenrace) m_fLine1Parameter = 0;  }
	if( m_fLine1Parameter > d2 ){   if( m_fLine1Parameter<= d2 + dTolenrace) m_fLine1Parameter = d2; }

	m_fLine0Parameter/=d1;
	m_fLine1Parameter/=d2;
	if( m_fLine0Parameter >=0 && m_fLine0Parameter <=1 && m_fLine1Parameter >=0 && m_fLine1Parameter <=1) return true;
	return false;

}

int IntersectLinePath2D::Intersects(const CLine2008& m_line, const CPath2008& m_path, DistanceUnit dTolenrace /*= (std::numeric_limits<DistanceUnit>::min)()*/ )
{
	
	m_vlineParameter.clear();
	m_vpathParameter.clear();
	for(int i =0 ;i<m_path.getCount()-1; ++i)
	{
		CLine2008 pathline(m_path.getPoint(i), m_path.getPoint(i+1));
		IntersectLineLine2D intersectLine;
		if(intersectLine.Intersects(m_line,pathline,dTolenrace))
		{
			m_vlineParameter.push_back(intersectLine.GetLine1Parameter());
			m_vpathParameter.push_back(intersectLine.GetLine2Parameter() + i);
		
		}

	}
	return m_vlineParameter.size();
}

int IntersectPathPath2D::Intersects(const CPath2008& m_path1, const CPath2008& m_path2, DistanceUnit dTolenrace /*= (std::numeric_limits<DistanceUnit>::min)()*/ )
{
	m_vIntersectPtIndexInPath1.clear();
	m_vIntersectPtIndexInPath2.clear();
	for(int i=0;i<m_path1.getCount()-1;i++)
	{
		CLine2008 path1Line(m_path1.getPoint(i),m_path1.getPoint(i+1));
		IntersectLinePath2D intersectLinePath;

		if(intersectLinePath.Intersects(path1Line, m_path2,dTolenrace))
		{
			for(int j=0;j<(int)intersectLinePath.m_vlineParameter.size();j++){
				m_vIntersectPtIndexInPath1.push_back(i+intersectLinePath.m_vlineParameter[j]);
			}
			m_vIntersectPtIndexInPath2.insert(m_vIntersectPtIndexInPath2.end(), intersectLinePath.m_vpathParameter.begin(),intersectLinePath.m_vpathParameter.end());
		}
	}
	//std::sort(m_vpath1Parameters.begin(),m_vpath1Parameters.end());
	//std::sort(m_vpath2Parameters.begin(),m_vpath2Parameters.end());
	return m_vIntersectPtIndexInPath1.size();
}

DistanceUnit DistancePointLine3D::GetSquared(const CPoint2008& m_pt, const CLine2008& m_line)
{
//	std::ofstream echoFile ("c:\\debug_DistancePointPath3D.log", stdios::app);

	ARCVector3 segOrigin = m_line.GetPoint1();
//	echoFile<<"segOrigin : " <<ARCStringConvert::toString( segOrigin)<< "\r\n";
	ARCVector3 segDirection = m_line.GetPoint2() - m_line.GetPoint1();
//	echoFile<<"segDirection : " <<ARCStringConvert::toString( segDirection)<< "\r\n";
	segDirection.Normalize();
//	echoFile<<"segDirection Normalize: " <<ARCStringConvert::toString( segDirection)<< "\r\n";
	DistanceUnit segExtent = m_line.GetLineLength();
//	echoFile<<"segExtent: " <<segExtent<< "\r\n";
	ARCVector3 kDiff = m_pt - segOrigin;
//	echoFile<<"kDiff: " <<ARCStringConvert::toString( kDiff)<< "\r\n";
	m_fLineParameter = segDirection.dot(kDiff);	
//	echoFile<<"m_fLineParameter: " <<m_fLineParameter<< "\r\n";
	if(m_fLineParameter < 0 ) m_fLineParameter = 0;
	if(m_fLineParameter > segExtent) m_fLineParameter = segExtent;

//	echoFile<<"m_fLineParameter A: " <<m_fLineParameter<< "\r\n";

	ARCVector3 closetPoint = segOrigin + m_fLineParameter * segDirection;
	kDiff = m_pt - closetPoint;
	m_clostPoint = CPoint2008(closetPoint[VX],closetPoint[VY],closetPoint[VZ]);
	
	if(segExtent>0)
		m_fLineParameter/= segExtent;
	else 
		m_fLineParameter =0;
//	echoFile<<"m_fLineParameter B: " <<m_fLineParameter<< "\r\n";
//	echoFile<<"kDiff.Magnitude2(): " <<kDiff.Magnitude2()<< "\r\n";

//	echoFile.close();
	return  kDiff.Magnitude2();
}




DistanceUnit DistancePointPath3D::GetSquared(const CPoint2008& m_pt, const CPath2008& m_path)
{
	DistanceUnit minSquareDist =(std::numeric_limits<DistanceUnit>::max)() * 0.5;
	
	//std::ofstream echoFile ("c:\\debug_DistancePointPath3D.log", stdios::app);
	//echoFile<< "__________________"<<"standPt :"<<m_pt.getX()<<","<<m_pt.getY()<<"," <<m_pt.getZ()<<",    "<<m_path.getCount()<< "\r\n";
	//echoFile<< "__________________"<<"Path :"<<m_path.getCount()<<"   :  "<<ARCStringConvert::toDBString(m_path) << "\r\n";
	//echoFile.close();
	for(int i =0 ;i< m_path.getCount()-1;i++)
	{
		CPoint2008 p1= m_path.getPoint(i);
		CPoint2008 p2 = m_path.getPoint(i+1);
		
		//echoFile<<"p1 :" << p1.getX()<<","<<p1.getY()<<"," <<p1.getZ()<< "\r\n";
		//echoFile<<"p2 :" << p2.getX()<<","<<p2.getY()<<"," <<p2.getZ()<< "\r\n";	
		//echoFile.close();
		CLine2008 pathLine(p1,p2);
		DistancePointLine3D distPtLine;
		DistanceUnit distSquare = distPtLine.GetSquared(m_pt,pathLine);
		//std::ofstream echoFile1 ("c:\\debug_DistancePointPath3D.log", stdios::app);
		//echoFile1<< "__________________"<<distSquare<<"       x:"<< minSquareDist <<"\r\n";
		//echoFile1.close();
		if(minSquareDist > distSquare )
		{
			minSquareDist = distSquare;
			m_fPathParameter = i + distPtLine.m_fLineParameter;
			m_clostPoint = distPtLine.m_clostPoint;
		}
	}
	//std::ofstream echoFile2 ("c:\\debug_DistancePointPath3D.log", stdios::app);
//	echoFile2<< "__________________:"<<m_fPathParameter<<"       x:"<< minSquareDist <<"\r\n";
//	echoFile2.close();
	return minSquareDist;
}



int IntersectPathPolygon2D::Intersects( const CPath2008& m_path, const CPath2008& m_polygon,DistanceUnit dTolenrace /*= (std::numeric_limits<DistanceUnit>::min)() */ )
{
	m_vpathParameters.clear();
	m_vpolyParameters.clear();
	if(m_polygon.getCount()==0)
		return 0;

	CPath2008 pathPoly = m_polygon;
	pathPoly.insertPointAfterAt(m_polygon.getPoint(0),m_polygon.getCount()-1);
	IntersectPathPath2D pathPath;
	int nCount = pathPath.Intersects(m_path,pathPoly,dTolenrace);
	m_vpathParameters = pathPath.m_vIntersectPtIndexInPath1;
	m_vpolyParameters = pathPath.m_vIntersectPtIndexInPath2;
	return nCount;
}