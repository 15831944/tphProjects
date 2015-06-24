// Car.h: interface for the CCar class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAR_H__4C29FC76_F5D0_40F3_8BD8_6208B413EE06__INCLUDED_)
#define AFX_CAR_H__4C29FC76_F5D0_40F3_8BD8_6208B413EE06__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "process.h"
//#include "hold.h"
//#include "terminal.h"
typedef std::vector< Processor* > DOOR;  
#define PLATFORM_CAR_DISTANCE 10.0
class CCar  
{
public:

	void removePerson( const Person* _pPerson );
	
	int clearQueue (ElapsedTime _curTime);
	void CleanData();
	int GetPaxCount();
	void AddPax( int _nGroupCount );
	void DeletePax(int _nGroupCount);
	void LeaveStation();
	CCar();
	virtual ~CCar();
	CCar(const CCar & _car);
	BOOL IsAValidCar();
	//serilize CCar 's info
	int WriteAllProcessorInfo(ArctermFile& procFile) const;
	//read all cars data from files
	int ReadAllProcessorInfo( ArctermFile& procFile );

	//if station is not on first floor, then we should update all its compoment's z 
	void CorrectZ( DistanceUnit _dZ );
	Processor* GetEntryDoor( int _nIdx );
	int GetEntryDoorCount() const;

	void DeleteEntryDoor( int _nIdx );
	void DeleteExitDoor( int _nIdx );
	Processor* GetExitDoor( int _nIdx );
	int GetExitDoorCount() const;

	void DoOffest( DistanceUnit _xOffset, DistanceUnit _yOffset );

	HoldingArea* GetCarArea();


	DOOR m_entryDoors;	
	DOOR m_exitDoors;	
	HoldingArea* m_carArea;
	//HoldingArea* m_preBoardingArea;
	
	Terminal* m_pTerm;

	void Rotate (DistanceUnit degrees);
	void Mirror(const Path* _p);

	void SetCarCapacity( int _iCarCapacity){ m_iCapacity = _iCarCapacity;};
	int GetCarCapacity() const { return m_iCapacity;};
	
	bool ComputeDoorInfo(int _nEntryDoorNum, int _nExitDoorNum, int _nExitDoorType,double _dCarLength, double _dPlatFormWidth );

protected:
	int m_nPaxOnBoard;
	int m_iCapacity;
};

#endif // !defined(AFX_CAR_H__4C29FC76_F5D0_40F3_8BD8_6208B413EE06__INCLUDED_)
