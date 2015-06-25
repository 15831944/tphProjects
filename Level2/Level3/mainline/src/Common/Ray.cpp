#include "StdAfx.h"
#include ".\ray.h"
#include "ARCVector.h"
#include "line.h"
#include <limits>

ARCRay::ARCRay( const Point& orig, const Point& vdir ):Origin(orig),Direction(vdir)
{

}


Point ARCRay::GetDistPoint( DistanceUnit dist ) const
{
	return Origin + Direction * dist;
}


DistanceRay3Line3::DistanceRay3Line3( const ARCRay& ray,const Line& line ) : m_ray(ray),m_line(line)
{

}

bool DistanceRay3Line3::intersects()
{
	ARCVector3 SegDir = m_line.GetPoint2() - m_line.GetPoint1();
	SegDir.Normalize();

	DistanceUnit fA01 = - ARCVector3(m_ray.Direction).dot(SegDir);
	DistanceUnit fDet = abs(1.0 - fA01*fA01);

	if (fDet >= 0)
	{
		return true;
	}

	return false;
}

DistanceUnit DistanceRay3Line3::GetSquared(DistanceUnit dtolerance)
{
	
	ARCVector3 SegOrin = m_line.GetPoint1();
	ARCVector3 SegDir = m_line.GetPoint2() - m_line.GetPoint1();
	DistanceUnit SegExt = SegDir.Length();
	SegDir.Normalize();


	ARCVector3 kDiff = m_ray.Origin- SegOrin;
	DistanceUnit fA01 = - ARCVector3(m_ray.Direction).dot(SegDir);
	DistanceUnit fB0 = kDiff.dot(m_ray.Direction);
	DistanceUnit fB1 = -kDiff.dot(SegDir);
	DistanceUnit fC = kDiff.Magnitude2();
	DistanceUnit fDet = abs(1.0 - fA01*fA01);
	DistanceUnit fS0, fS1, fSqrDist, fExtDet;

	if (fDet >= dtolerance)
	{
		// The ray and segment are not parallel.
		fS0 = fA01*fB1-fB0;
		fS1 = fA01*fB0-fB1;
		fExtDet = SegExt*fDet;

		if (fS0 >= (DistanceUnit)0.0)
		{
			if (fS1 >= -fExtDet)
			{
				if (fS1 <= fExtDet)  // region 0
				{
					// minimum at interior points of ray and segment
					DistanceUnit fInvDet = ((DistanceUnit)1.0)/fDet;
					fS0 *= fInvDet;
					fS1 *= fInvDet;
					fSqrDist = fS0*(fS0+fA01*fS1+((DistanceUnit)2.0)*fB0) +
						fS1*(fA01*fS0+fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
				else  // region 1
				{
					fS1 = SegExt;
					fS0 = -(fA01*fS1+fB0);
					if (fS0 > (DistanceUnit)0.0)
					{
						fSqrDist = -fS0*fS0+fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
					}
					else
					{
						fS0 = (DistanceUnit)0.0;
						fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
					}
				}
			}
			else  // region 5
			{
				fS1 = -SegExt;
				fS0 = -(fA01*fS1+fB0);
				if (fS0 > (DistanceUnit)0.0)
				{
					fSqrDist = -fS0*fS0+fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
				else
				{
					fS0 = (DistanceUnit)0.0;
					fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
			}
		}
		else
		{
			if (fS1 <= -fExtDet)  // region 4
			{
				fS0 = -(-fA01*SegExt+fB0);
				if (fS0 > (DistanceUnit)0.0)
				{
					fS1 = -SegExt;
					fSqrDist = -fS0*fS0+fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
				else
				{
					fS0 = (DistanceUnit)0.0;
					fS1 = -fB1;
					if (fS1 < -SegExt)
					{
						fS1 = -SegExt;
					}
					else if (fS1 > SegExt)
					{
						fS1 = SegExt;
					}
					fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
			}
			else if (fS1 <= fExtDet)  // region 3
			{
				fS0 = (DistanceUnit)0.0;
				fS1 = -fB1;
				if (fS1 < -SegExt)
				{
					fS1 = -SegExt;
				}
				else if (fS1 > SegExt)
				{
					fS1 = SegExt;
				}
				fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
			}
			else  // region 2
			{
				fS0 = -(fA01*SegExt+fB0);
				if (fS0 > (DistanceUnit)0.0)
				{
					fS1 = SegExt;
					fSqrDist = -fS0*fS0+fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
				else
				{
					fS0 = (DistanceUnit)0.0;
					fS1 = -fB1;
					if (fS1 < -SegExt)
					{
						fS1 = -SegExt;
					}
					else if (fS1 > SegExt)
					{
						fS1 = SegExt;
					}
					fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
				}
			}
		}
	}
	else
	{
		// ray and segment are parallel
		if (fA01 > (DistanceUnit)0.0)
		{
			// opposite direction vectors
			fS1 = -SegExt;
		}
		else
		{
			// same direction vectors
			fS1 = SegExt;
		}

		fS0 = -(fA01*fS1+fB0);
		if (fS0 > (DistanceUnit)0.0)
		{
			fSqrDist = -fS0*fS0+fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
		}
		else
		{
			fS0 = (DistanceUnit)0.0;
			fSqrDist = fS1*(fS1+((DistanceUnit)2.0)*fB1)+fC;
		}
	}

	m_fRayParameter = fS0;
	m_fLineParameter = fS1/SegExt;
	return abs(fSqrDist);
}

DistanceRay3Path3::DistanceRay3Path3( const ARCRay& ray, const Path& path ):m_ray(ray),m_path(path)
{
	
}

bool DistanceRay3Path3::intersects()
{
	ASSERT(m_path.getCount()>1);

	for(int  i=0;i< m_path.getCount()-1;i++)
	{
		Line seg(m_path.getPoint(i),m_path.getPoint(i+1));
		DistanceRay3Line3 distrayline(m_ray,seg);
		if(distrayline.intersects())
			return true;
	}
	return false;
}

DistanceUnit DistanceRay3Path3::GetSquared( DistanceUnit dTolerance )
{
	ASSERT(m_path.getCount()>1);

	DistanceUnit minSquaredDist = (std::numeric_limits<DistanceUnit>::max)();

	for(int  i=0;i< m_path.getCount()-1;i++)
	{
		Line seg(m_path.getPoint(i),m_path.getPoint(i+1));
		DistanceRay3Line3 distrayline(m_ray,seg);
		DistanceUnit thisDist = distrayline.GetSquared(dTolerance);
		if(thisDist < minSquaredDist)
		{
			minSquaredDist = thisDist;
			m_fRayParameter = distrayline.m_fRayParameter;
			m_fPathParameter = i + distrayline.m_fLineParameter;
		}
	}
	return minSquaredDist;
}