#include "StdAfx.h"

#include "point.h"
#include ".\getintersection.h"
#include "line.h"
#include "ARCVector.h"
#include "DistanceLineLine.h"
#include "Path2008.h"
#include "ARCPath.h"
#include "ARCPipe.h"


#include <map>


struct SideIntersectInfo
{
	DistanceUnit sideIntersect;
	DistanceUnit sideOtherIntersect;
	int sidei;

	SideIntersectInfo(const DistanceUnit& side, const DistanceUnit& otherside, int iside){ sideIntersect = side, sideOtherIntersect = otherside;sidei = iside; }
	bool operator < (const SideIntersectInfo& otherInfo)const{ return sideIntersect < otherInfo.sideIntersect; } 
};

void GetHoldPositionsInfo(const CPath2008& selfPath,DistanceUnit selfWith, const CPath2008& otherPath, DistanceUnit dwidth, HoldPositionInfoList& intersectList,bool bWithOffset)
{
	//ASSERT(selfPath.getCount()>1); ASSERT(otherPath.getCount()>1);
	if(selfPath.getCount()<2) return;
	if(otherPath.getCount()<2) return;


	ARCPipe pipepath (otherPath,dwidth);
	int npippathcnt  = (int)pipepath.m_centerPath.size();

	ARCVector3 fdir = pipepath.m_centerPath.at(0) - pipepath.m_centerPath.at(1); fdir.Normalize();
	ARCVector3 edir = pipepath.m_centerPath.at(npippathcnt -1) - pipepath.m_centerPath.at(npippathcnt-2);edir.Normalize();

	ARCVector3 fPoint =  fdir*selfWith*0.5+pipepath.m_centerPath.front();
	ARCVector3 epoint =  edir*selfWith*0.5+pipepath.m_centerPath.back();
	pipepath.m_centerPath.insert(pipepath.m_centerPath.begin(),fPoint);
	pipepath.m_centerPath.insert(pipepath.m_centerPath.end(),epoint);

	pipepath.m_sidePath1.insert(pipepath.m_sidePath1.begin(),fPoint);
	pipepath.m_sidePath1.insert(pipepath.m_sidePath1.end(), epoint);

	pipepath.m_sidePath2.insert(pipepath.m_sidePath2.begin(),fPoint);
	pipepath.m_sidePath2.insert(pipepath.m_sidePath2.end(), epoint);


	CPath2008 sidePath1 = pipepath.m_sidePath1.getPath2008();
	CPath2008 sidePath2 = pipepath.m_sidePath2.getPath2008();
	CPath2008 centerPath = pipepath.m_centerPath.getPath2008();


	//std::vector<DistanceUnit> sideIntersections;
	std::vector<DistanceUnit> centerIntersections;	
	std::vector<SideIntersectInfo> sideIntersectionInfos;

	IntersectPathPath2D intersectPP1;
	if( intersectPP1.Intersects(selfPath,sidePath1) )
	{
		//sideIntersections.insert(sideIntersections.end(),intersectPP1.m_vpath1Parameters.begin(),intersectPP1.m_vpath1Parameters.end());
		for(int i=0;i< (int)intersectPP1.m_vIntersectPtIndexInPath1.size();i++)
		{
			sideIntersectionInfos.push_back( SideIntersectInfo(intersectPP1.m_vIntersectPtIndexInPath1.at(i), intersectPP1.m_vIntersectPtIndexInPath2.at(i),0) ); 
		}
	}
	IntersectPathPath2D intersectPP2;
	if( intersectPP2.Intersects(selfPath,sidePath2) )
	{
		//sideIntersections.insert(sideIntersections.end(),intersectPP2.m_vpath1Parameters.begin(),intersectPP2.m_vpath1Parameters.end() );
		for(int i=0;i< (int)intersectPP2.m_vIntersectPtIndexInPath1.size();i++)
		{
			sideIntersectionInfos.push_back( SideIntersectInfo(intersectPP2.m_vIntersectPtIndexInPath1.at(i), intersectPP2.m_vIntersectPtIndexInPath2.at(i), 1) ); 
		}
	}

	IntersectPathPath2D intersectCenter;
	if(intersectCenter.Intersects(selfPath,centerPath,5.0))
	{
		centerIntersections = intersectCenter.m_vIntersectPtIndexInPath1;
	}
	//sideIntersections.push_back(-1.0);
	sideIntersectionInfos.push_back( SideIntersectInfo(-1.0,-1.0,0) );
	sideIntersectionInfos.push_back( SideIntersectInfo((double)selfPath.getCount(),(double)selfPath.getCount(),1) );

	

	std::sort(sideIntersectionInfos.begin(),sideIntersectionInfos.end());
	std::sort(centerIntersections.begin(),centerIntersections.end());

	// put the center intersections to the side intersection pair;
	typedef std::pair<SideIntersectInfo,SideIntersectInfo> SideIntersectPair;

	typedef std::map<SideIntersectPair, std::vector<DistanceUnit> > SidePairMapNode;
	SidePairMapNode centerIntersectMap;

	for(int i =0 ; i< (int)centerIntersections.size(); i++)
	{
		DistanceUnit center = centerIntersections.at(i);
		for(int j=0; j<(int)sideIntersectionInfos.size()-1; j++)
		{
			SideIntersectPair sidePair(sideIntersectionInfos.at(j),sideIntersectionInfos.at(j+1));
			if(center<sidePair.second.sideIntersect && center > sidePair.first.sideIntersect){
				centerIntersectMap[sidePair].push_back(center);
				break;
			}
		}
	}

	//now check the pair;
	int idx = 0;
	for(SidePairMapNode::const_iterator itr = centerIntersectMap.begin(); itr!=centerIntersectMap.end();itr++)
	{
		const std::vector<DistanceUnit>& centerlist = itr->second;
		if(centerlist.size()>0)// means this is a pair of intersect
		{
			double relatPos = centerlist.at((centerlist.size()-1)/2);
			const SideIntersectPair& sidePair =  itr->first;
			//
			HoldPositionInfo holdinfo;
			holdinfo.intersectidx = idx;
			holdinfo.first = selfPath.GetIndexDist((float)relatPos);
			

			const SideIntersectInfo& intectInfo1= sidePair.first;
			const SideIntersectInfo& intectInfo2 = sidePair.second;

			if(intectInfo1.sideIntersect >0 ){				
				
				DistanceUnit realDistself = selfPath.GetIndexDist((float)intectInfo1.sideIntersect);				

				ARCVector3 otherDir;
				if(intectInfo1.sidei == 0)
				{
					DistanceUnit realDist = sidePath1.GetIndexDist((float)intectInfo1.sideOtherIntersect);
					otherDir = sidePath1.GetDistPoint(realDist+1) - sidePath1.GetDistPoint(realDist);
				}else{
					DistanceUnit realDist = sidePath2.GetIndexDist((float)intectInfo1.sideOtherIntersect);
					otherDir = sidePath2.GetDistPoint(realDist+1) - sidePath2.GetDistPoint(realDist);
				}
				//calculate the offset of intersect
				DistanceUnit offset = 0;
				if(bWithOffset)
				{
					ARCVector3 selfDir = selfPath.GetDistPoint(realDistself+1) - selfPath.GetDistPoint(realDistself);

					double cosAngle = selfDir.GetCosOfTwoVector(otherDir);
					double sinAngle;
					if( abs(cosAngle) >1 ) 
						sinAngle = 0;
					else
						sinAngle = sqrt(1-cosAngle*cosAngle);

					if(sinAngle < ARCMath::EPSILON * 0.001)
						sinAngle = ARCMath::EPSILON * 0.001;

					double ctanAngle = abs(cosAngle)/sinAngle;

					offset = selfWith * 0.5 * ctanAngle;
					if(holdinfo.first > realDistself) 
						offset = -offset;
				}
				
				//
				holdinfo.second =  realDistself + offset;
				intersectList.push_back(holdinfo);
			}
			if(intectInfo2.sideIntersect<selfPath.getCount())
			{	
				DistanceUnit realDistself = selfPath.GetIndexDist((float)intectInfo2.sideIntersect);
				
				ARCVector3 otherDir;
				if(intectInfo1.sidei == 0)
				{
					DistanceUnit realDist = sidePath1.GetIndexDist((float)intectInfo2.sideOtherIntersect);
					otherDir = sidePath1.GetDistPoint(realDist+1) - sidePath1.GetDistPoint(realDist);
				}else{
					DistanceUnit realDist = sidePath2.GetIndexDist((float)intectInfo2.sideOtherIntersect);
					otherDir = sidePath2.GetDistPoint(realDist+1) - sidePath2.GetDistPoint(realDist);
				}
				//calculate the offset of intersect
				DistanceUnit offset = 0;
				if(bWithOffset)
				{
					ARCVector3 selfDir = selfPath.GetDistPoint(realDistself+1) - selfPath.GetDistPoint(realDistself);

					double cosAngle = selfDir.GetCosOfTwoVector(otherDir);
					double sinAngle;

					if( abs(cosAngle) >1 ) 
						sinAngle = 0;
					else
						sinAngle = sqrt(1-cosAngle*cosAngle);

					if(sinAngle < ARCMath::EPSILON * 0.001)
						sinAngle = ARCMath::EPSILON * 0.001;

					double ctanAngle = abs(cosAngle)/sinAngle;
					offset = selfWith * 0.5 * ctanAngle;
					if(holdinfo.first > realDistself) 
						offset = -offset;
				}		
				
				//
				holdinfo.second =  realDistself + offset;			
				intersectList.push_back(holdinfo);
			}
			//
			idx++;
		}
	}
	
}

CPath2008 GetSubPath( const CPath2008& pathsrc , DistanceUnit dfrom ,DistanceUnit dto )
{
	bool bswaped = false;
	if(dfrom > dto){
		std::swap(dfrom, dto);
		bswaped = true;
	}

	std::vector<CPoint2008> vSubPath;
	double relatPos1 = pathsrc.GetDistIndex(dfrom);
	double relatPos2 = pathsrc.GetDistIndex(dto);
	//if(relatPos1<0) relatPos1 = 0;
	//if(relatPos2>pathsrc.getCount()-1) relatPos2 =(float) pathsrc.getCount()-1;
	ASSERT(relatPos1>-1 && relatPos1 <= pathsrc.getCount());
	ASSERT(relatPos2>-1 && relatPos2 <= pathsrc.getCount());

	int piFrom =(int) relatPos1; int piTo =(int)relatPos2;

	ASSERT(piFrom <= piTo);

	vSubPath.reserve(piTo -piFrom + 2);
	CPoint2008 DistPt;
	DistPt = pathsrc.GetDistPoint(dfrom);
	vSubPath.push_back(DistPt);
	for(int j = (int)relatPos1+1; j<relatPos2; j++)
	{
		vSubPath.push_back( pathsrc.getPoint(j) );			
	}
	DistPt = pathsrc.GetDistPoint(dto);
	vSubPath.push_back(DistPt);
	
	if(bswaped) std::reverse(vSubPath.begin(),vSubPath.end());
	
	CPath2008 restl;
	restl.init(vSubPath.size(), &vSubPath[0]);
	return restl;
}

//double GetPointDistInPath( const CPath2008& path,const CPoint2008& pt )
//{
//	double accDist =0;
//	int i=0;
//	for(i=0;i<path.getCount()-1;i++)
//	{
//		ARCVector3 v1 = pt - path.getPoint(i);
//		ARCVector3 v2 = pt - path.getPoint(i+1);
//		if( v1.GetCosOfTwoVector(v2) <-0.999 )
//		{
//			return accDist + v1.length();
//		}
//		else
//		{
//			accDist += (path.getPoint(i+1)-path.getPoint(i)).length();
//		}
//	}
//	return accDist;
//}
