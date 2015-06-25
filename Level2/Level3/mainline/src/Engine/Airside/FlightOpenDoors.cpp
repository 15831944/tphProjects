#include "stdafx.h"
#include "FlightOpenDoors.h"



int CFlightOpenDoors::getDoorIndex( ACTypeDoor::DoorDir doorDir, int sideIdx ) const
{
	for(int i=0;i<getCount();i++)
	{
		const COpenDoorInfo& dinfo = getDoor(i);
		if(dinfo.m_sideIndex == sideIdx && dinfo.m_doorSide==doorDir)	
			return i;
	}
	return -1;
}

int CFlightOpenDoors::getCount() const
{
	return (int)m_doorlist.size();
}
