#include "stdafx.h"

#include "landsidePath.h"
#include <algorithm>
#include "..\..\Common\Path2008.h"
#include "..\..\Common\ARCVector.h"


double LandsidePath::getDistIndex( DistanceUnit dist ) const
{
	_DistConstIter itr  = std::upper_bound(m_distlist.begin(),m_distlist.end(),dist);
	if(itr==m_distlist.end())
		return m_distlist.size();
	else if(itr==m_distlist.begin())
	{
		return -1;
	}
	else
	{
		_DistConstIter itrNext = itr;
		--itr;
		DistanceUnit distSpan = (*itrNext)-(*itr);
		DistanceUnit distOffset = dist - *itr;
		return distOffset/distSpan + (itr-m_distlist.begin());
	}
}

DistanceUnit LandsidePath::getIndexDist( double dIndex ) const
{
	int iIndex = (int)dIndex;
	double dOffset = dIndex - iIndex;
	if(iIndex<0)
		return 0;
	if(iIndex >= (int)m_distlist.size()-1 )
		return getAllLength();


	return  m_distlist[iIndex]*(1-dOffset) + m_distlist[iIndex+1]*dOffset;


}

DistanceUnit LandsidePath::getIndexDist( int iIndex ) const
{
	if(iIndex<0)
		return 0;
	if(iIndex >= (int)m_distlist.size()-1 )
		return getAllLength();
	return m_distlist[iIndex];
}

LandsidePosition LandsidePath::getIndexPosition( double dIndex ) const
{
	if(m_distlist.empty())
		return LandsidePosition();

	int iIndex = (int)dIndex;
	double dOffset = dIndex - iIndex;
	if(iIndex<0)
		return m_poslist.front();
	if(iIndex >= (int)m_distlist.size()-1 )
		return m_poslist.back() ;

	if(dOffset==0)
	{
		return PositionAt(iIndex);
	}

	const LandsidePosition& prePos = m_poslist[iIndex];
	const LandsidePosition& nextPos = m_poslist[iIndex+1];

	LandsidePosition ret = prePos;
	ret.pos = prePos.pos * (1-dOffset) + nextPos.pos* dOffset;
	ret.distInRes = prePos.distInRes * (1-dOffset) + nextPos.distInRes * dOffset;
	return ret;
}

void LandsidePath::addPath( LandsideResourceInSim* pRes, const CPath2008* path,DistanceUnit distFrom )
{
	if(!path)return;
	if(path->getCount()==0)
		return;

	LandsidePosition pos;
	pos.pos = path->getPoint(0);
	pos.pRes = pRes;
	pos.distInRes = distFrom;
	addPostion(pos);

	for(int i=1;i<path->getCount();i++)
	{
		CPoint2008 pt = path->getPoint(i);
		DistanceUnit dist = pt.distance3D(pos.pos);
		distFrom+= dist;

		pos.pos = pt;
		pos.pRes = pRes;
		pos.distInRes = distFrom;
		addPostion(pos);
	}
}

LandsidePosition& LandsidePath::addPostion( const LandsidePosition& pos )
{
	if(m_poslist.empty())
	{
		ASSERT(m_distlist.empty());
		m_poslist.push_back(pos);
		m_distlist.push_back(0);
	}
	else
	{
		LandsidePosition& lastPos = m_poslist.back();
		DistanceUnit dist = lastPos.pos.distance3D(pos.pos);
		m_poslist.push_back(pos);
		m_distlist.push_back(m_distlist.back()+dist);
	}
	return m_poslist.back();
}

ARCVector3 LandsidePath::getDistDir( DistanceUnit dist ) const
{	
	double dIndex = getDistIndex(dist);
	return getIndexDir(dIndex);
}

ARCVector3 LandsidePath::getIndexDir( double dIndex ) const
{
	int nPtCount = getPtCount();
	if(nPtCount <2)
		return ARCVector3(0,0,0);
	

	if(dIndex < 0)
	{
		return PositionAt(0).pos - PositionAt(1).pos;
	}

	if(dIndex > getPtCount() -2)
	{
		return PositionAt(nPtCount -1).pos - PositionAt( nPtCount -2 ).pos;
	}

	int iIndex = (int)dIndex;
	return PositionAt(iIndex+1).pos - PositionAt(iIndex).pos;

}

void LandsidePath::getPath( DistanceUnit distF, DistanceUnit distT, CPath2008& path )
{
	double dIndexF = getDistIndex(distF);
	double dIndexT = getDistIndex(distT);
	path.clear();
	
	path.push_back(getIndexPosition(dIndexF).pos);
	for(int i= (int)dIndexF+1;i<dIndexT;i++)
	{
		path.push_back(PositionAt(i).pos);
	}
	path.push_back(getIndexPosition(dIndexT).pos);
}

bool LandsidePath::ExistResourcePoint( int iIdx,LandsideResourceInSim* pRes ) const
{
	int nPtCount = getPtCount();
	if (iIdx < 0 && iIdx >= nPtCount)
		return false;

	for (int i = iIdx + 1; i < nPtCount; i++)
	{
		const LandsidePosition& nextPos = PositionAt(i);
		if (nextPos.pRes == pRes)
		{
			return true;
		}
	}
	
	return false;
}

DistanceUnit LandsidePath::getAllLength() const
{
	return m_distlist.empty()?0:m_distlist.back();
}
