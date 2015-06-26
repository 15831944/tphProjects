// TrainLogEntry.cpp: implementation of the CTrainLogEntry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <string.h>
#include "TrainLogEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTrainLogEntry::CTrainLogEntry()
	:TerminalLogItem<TrainDescStruct,TrainEventStruct> ()
{

}

CTrainLogEntry::~CTrainLogEntry()
{

}

void CTrainLogEntry::echo(ofsstream& p_stream, const CString& _csProjPath) const
{
	assert( m_pOutTerm );
    p_stream	<< item.id << ',' 
				<< GetStartTime() << ',' 
				<< GetEndTime() << ','
				<< item.speed << ','
				<< item.accelerateSpeed << ','
				<< item.decelerateSpeed << ','
				<< GetTurnAroundTime() << ','
				<< GetHeadWayTime() << '\n';
}

void CTrainLogEntry::SetTrainID(int nID)
{
	item.ID = nID;
}

int CTrainLogEntry::GetTrainID()
{
	return item.ID;
}

void CTrainLogEntry::SetId( CString _csId )
{
	strcpy( item.id, _csId );
}

long CTrainLogEntry::GetID()
{
	CString strID(item.id);
	int nIndex = strID.Find('-');
	strID = strID.Right(strID.GetLength() - nIndex - 1);
	int id = atoi(strID) - 1;
	return long(id);
}

void CTrainLogEntry::SetStartTime( ElapsedTime _time )
{
	item.startTime = (long)_time;
}

void CTrainLogEntry::SetEndTime( ElapsedTime _time )
{
	item.endTime = (long)_time;
}


void CTrainLogEntry::SetCarNumber( int _nCars )
{
	item.numCars = _nCars;
}

void CTrainLogEntry::SetCarLength( DistanceUnit _dLength )
{
	item.carLength = (float)_dLength;
}

void CTrainLogEntry::SetCarWidth( DistanceUnit _dWidth )
{
	item.carWidth = (float)_dWidth;
}

void CTrainLogEntry::SetSpeed( DistanceUnit _Speed )
{
	item.speed = (float)_Speed;
}

void CTrainLogEntry::SetAccelerateSpeed( DistanceUnit _accelerateSpeed )
{
	item.accelerateSpeed = (float)_accelerateSpeed;
}

void CTrainLogEntry::SetDecelerateSpeed( DistanceUnit _decelerateSpeed )
{
	item.decelerateSpeed = (float)_decelerateSpeed;
}


void CTrainLogEntry::SetCircleTime( ElapsedTime _time )
{
	item.circleTime = _time;
}

void CTrainLogEntry::SetTurnAroundTime( ElapsedTime _turnAroundTime )
{
	item.turnArroundTime = (long)_turnAroundTime;
}

void CTrainLogEntry::SetHeadWayTime( ElapsedTime _headWayTime)
{
	item.headWayTime = (long)_headWayTime;
}

void CTrainLogEntry::SetIndexNum( int _nIdx )
{
	item.indexNum = _nIdx;
}

int CTrainLogEntry::GetIndexNum()
{
	return (int)item.indexNum;
}

ElapsedTime CTrainLogEntry::GetStartTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.startTime );
    return t;	
}

ElapsedTime CTrainLogEntry::GetEndTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.endTime );
    return t;	
}

ElapsedTime CTrainLogEntry::GetTurnAroundTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.turnArroundTime );
    return t;	
}


ElapsedTime CTrainLogEntry::GetHeadWayTime() const
{
    ElapsedTime t;
    t.setPrecisely( item.headWayTime );
    return t;	
}
