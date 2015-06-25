#include "StdAfx.h"
#include ".\floorchangemap.h"
#include "PATH.H"



double FloorChangeMap::getNewFloor( double oldFloor ) const
{
	int nOldFloor = (int)oldFloor;
	double dOffset = oldFloor - nOldFloor; 

	int nNewFloor = getNewFloor(nOldFloor);
	int nNewFloorPlus = getNewFloor(nOldFloor+1);

	return nNewFloor*(1.0-dOffset) + nNewFloorPlus*dOffset;
}

int FloorChangeMap::getNewFloor( int iOldFloor ) const
{
	if(iOldFloor<(int)nNewFloorIndex.size() && iOldFloor>=0)
	{
		return nNewFloorIndex[iOldFloor];
	}
	return iOldFloor;
}

void FloorChangeMap::ChangePathFloor( Path& path ) const
{
	for(int i=0;i<path.getCount();i++)
	{
		Point pt = path.getPoint(i);
		pt.setZ( getNewFloor(pt.getZ()/SCALE_FACTOR)*SCALE_FACTOR);
		path.setPoint(pt, i);
	}
}

void FloorChangeMap::ChangePointFloor( Point& pt ) const
{
	pt.setZ( getNewFloor(pt.getZ()/SCALE_FACTOR)*SCALE_FACTOR);
}
