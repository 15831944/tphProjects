// BaggageProcUtil.h: interface for the BaggageProcUtil class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BAGGAGEPROCUTIL_H__A42EBC7A_1D1C_4129_80A7_1B00558DE722__INCLUDED_)
#define AFX_BAGGAGEPROCUTIL_H__A42EBC7A_1D1C_4129_80A7_1B00558DE722__INCLUDED_

#include "PointAndBagsInBagProcessor.h"
class Path;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class BaggageProcUtil  
{
public:
	BaggageProcUtil();
	virtual ~BaggageProcUtil();
private:
	BAGPROCUTIL m_vPointAndBags;
	// needed time which one baggage moving a circle on baggage device
	ElapsedTime m_costTimeOfCircle;
public:

	// divide baggage moving path as several point
	void Init( Path* _ptList, DistanceUnit& _dSpeed  );

	// get next point on baggage moving path according to current index
	Point GetNextPos( int _iCurrentIdx ) const;

	PointAndBagsInBagProcessor& GetItemAt( int _iIdx ){ return m_vPointAndBags[_iIdx];}

	// add a _pPerson into baggage device's _iCurrentIdx 
	void AddPerson( int _iCurrentIdx , long _newPerson );

	// Erase a _pPerson into baggage device's _iCurrentIdx 
	void ErasePerson( int _iCurrentIdx , long _delPerson );

	// get the nearest point's index in baggae moving path 
	int GetNearestPosIdx ( const Point& _ptTestPoint  ) const;

	// get a point from baggage moving path which is the nearest point from _ptTestPoint
	void GetNearestPoint ( const Point& _ptTestPoint , Point& _ptResult ) const;

	int GetPosCount() const { return m_vPointAndBags.size() ;	};

	// Passenger pick up bags on current pos
	void PickUpBags( Passenger* _pOwner , int _iCurrentPos, BAGS& _vBags );

	ElapsedTime GetCircleTime()const { return m_costTimeOfCircle;	};
	

};

#endif // !defined(AFX_BAGGAGEPROCUTIL_H__A42EBC7A_1D1C_4129_80A7_1B00558DE722__INCLUDED_)
