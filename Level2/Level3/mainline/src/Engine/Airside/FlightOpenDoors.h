#pragma once
#include <vector>
#include "Common\Point2008.h"
#include "Common\elaptime.h"
#include "Common\ACTypeDoor.h"

class COpenDoorInfo
{
public:
	COpenDoorInfo():m_bConnected(FALSE),m_doorSide(ACTypeDoor::BothSide){}
	CPoint2008 mDoorPos; //door position
	CPoint2008 mGroundPos; //stair ground pos at dir 
	ElapsedTime mOpenTime; //open time
	BOOL m_bConnected;
	ACTypeDoor::DoorDir m_doorSide;    //left side or right
	int m_sideIndex;  //index in left or right
};


class CFlightOpenDoors
{
public:
	int getCount()const;
	COpenDoorInfo& getDoor(int idx){ return m_doorlist[idx]; }
	const COpenDoorInfo& getDoor(int idx)const{ return m_doorlist[idx]; }

	void add(const COpenDoorInfo& info){ m_doorlist.push_back(info); }

	int getDoorIndex(ACTypeDoor::DoorDir doorDir, int sideIdx)const;
protected:
	typedef std::vector<COpenDoorInfo> COpenDoorInfos;
	COpenDoorInfos m_doorlist;
};