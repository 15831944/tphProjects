#include "StdAfx.h"
#include ".\arcray.h"
#include "ARCPath.h"
#include "ARCPolygon.h"

int ARCRayIntersect::Get( const ARCRay2& ray, const ARCPath& path )
{
	rayDistList.resize(0);
	//rotate to ray as x coordinate
	ARCPath rPath = path;
	rPath.TransfromToRayCoordX(ray);

	for( int i=0;i<(int)rPath.size()-1;i++)
	{
		const ARCVector2& p1 = rPath[i];
		const ARCVector2& p2 = rPath[i+1];
		double miny = MIN(p1.y,p2.y);
		double maxy = MAX(p1.y,p2.y);
		//double dpy = maxy-miny;
		if( miny<0 && 0<=maxy )
		{
			double dr = (0-p1.y)/(p2.y-p1.y);			
			{
				double rx = p1.x+ (p2.x-p1.x)*dr;
				if(rx>0)
				{
					rayDistList.push_back(rx);					
				}
			}
		}
	}
	std::sort(rayDistList.begin(),rayDistList.end());
	return (int)rayDistList.size();
}

int ARCRayIntersect::Get( const ARCRay2& ray, const ARCPolygon& poly )
{
	rayDistList.resize(0);
	ARCPolygon rPath = poly;
	rPath.TransfromToRayCoordX(ray);

	int N = (int)rPath.size();

	for( int i=0;i<N;i++)
	{
		const ARCVector2& p1 = rPath[i];
		const ARCVector2& p2 = rPath[ (i+1)%N ];
		double miny = MIN(p1.y,p2.y);
		double maxy = MAX(p1.y,p2.y);
		//double dpy = maxy-miny;
		if( miny<0 && 0<=maxy )
		{
			double dr = (0-p1.y)/(p2.y-p1.y);			
			{
				double rx = p1.x+ (p2.x-p1.x)*dr;
				if(rx>0)
				{
					rayDistList.push_back(rx);					
				}
			}
		}
	}
	std::sort(rayDistList.begin(),rayDistList.end());
	return (int)rayDistList.size();
}

int ARCRayIntersect::Get( const ARCRay3& ray, const ARCPoint3& pt, double dRad )
{
	ARCVector3 vOffset = pt - ray.m_orign;
	double r0 = vOffset.Length();
	ARCVector3 vNorm = vOffset;
	vNorm.Normalize();		

	double b = r0 * ray.m_dir.dot(vNorm);
	double c = r0*r0 - dRad*dRad;
	double dt= b*b-c;

	if(dt>=0 && b >= 0)
	{
		double dDistToPax = b - sqrt(dt);	
		rayDistList.push_back(dDistToPax);
		return 1;
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
int ARCPathIntersect::Get( const ARCPath& path, const ARCPolygon& poly, bool bOnlyFirst )
{
	pathDistList.resize(0);

	int N = (int)path.size();
	double dAcc = 0;
	for(int i=0;i<N-1;i++)
	{
		const ARCVector2& p1 = path[i];
		const ARCVector2& p2 = path[i+1];
		ARCVector2 vdir = p2 - p1;
		double d = vdir.Length();
		vdir.Normalize();

		ARCRay2 ray(p1,vdir);
		ARCRayIntersect rayInstersect;
		rayInstersect.Get(ray,poly);

		for(size_t j=0;j<rayInstersect.rayDistList.size();++j)
		{
			double dist = rayInstersect.rayDistList[j];
			if(dist <= d)
			{
				pathDistList.push_back(dist + dAcc);
				pathIndexList.push_back(dist/d + i);				
			}
		}
		if(bOnlyFirst && !pathDistList.empty())
			break;

		dAcc += d;
	}

	return (int)pathDistList.size();
}



int ARCPathIntersect::Get( const ARCPath3& path, const ARCPolygon& poly, double polyZmin,double polyZmax, bool bOnlyFirst /*= false*/ )
{
	pathDistList.resize(0);

	int N = (int)path.size();
	double dAcc = 0;
	for(int i=0;i<N-1;i++)
	{
		ARCVector2 p1 = path[i].xy();
		ARCVector2 p2 = path[i+1].xy();
		ARCVector2 vdir = p2 - p1;
		double d = vdir.Length();
		vdir.Normalize();

		ARCRay2 ray(p1,vdir);
		ARCRayIntersect rayInstersect;
		rayInstersect.Get(ray,poly);

		for(size_t j=0;j<rayInstersect.rayDistList.size();++j)
		{
			double dist = rayInstersect.rayDistList[j];
			if(dist <= d)
			{	
				double drat = dist/d;
				double dZ = path[i].z * (1-drat) + path[i+1].z*drat;
				if(dZ > polyZmax || dZ < polyZmin )
				{
					pathDistList.push_back(dist + dAcc);
					pathIndexList.push_back(drat + i);
				}
			}
		}
		if(bOnlyFirst && !pathDistList.empty())
			break;

		dAcc += d;
	}

	return (int)pathDistList.size();
}

int ARCPathIntersect::Get( const ARCPath3& path, const ARCPoint3& pt, double drad, double dFromDist,bool bOnlyFirst /*= false*/ )
{
	pathDistList.resize(0);

	int N = (int)path.size();
	double dAcc = 0;
	for(int i=0;i<N-1;i++)
	{
		const ARCVector3& p1 = path[i];
		const ARCVector3& p2 = path[i+1];
		ARCVector3 vdir = p2 - p1;
		double d = vdir.Length();
		vdir.Normalize();
		if(dAcc + d<dFromDist)
		{
			dAcc+=d;
			continue;
		}
		ARCPoint3 startPt = p1;		
		double dOffsetp1= 0;
		if(dAcc < dFromDist)
		{
			dOffsetp1 = dFromDist - dAcc;
			double drat = (dFromDist-dAcc)/d;
			startPt = p1*(1-drat)+p2*drat;

		}
		

		ARCRayIntersect rayInstersect;
		rayInstersect.Get(ARCRay3(startPt,vdir),pt,drad);

		for(size_t j=0;j<rayInstersect.rayDistList.size();++j)
		{
			double dist = rayInstersect.rayDistList[j] + dOffsetp1;
			if(dist <= d )
			{	
				double drat = dist/d;
				pathDistList.push_back(dAcc+dist);	
				pathIndexList.push_back(i+drat);
			}
		}
		if(bOnlyFirst && !pathDistList.empty())
			break;

		dAcc += d;
	}

	return (int)pathDistList.size();
}