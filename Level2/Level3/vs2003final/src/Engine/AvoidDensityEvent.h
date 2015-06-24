// AvoidDensityEvent.h: interface for the AvoidDensityEvent class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVOIDDENSITYEVENT_H__FA986B27_45DA_45C4_A4EA_3D5DFEC51FDD__INCLUDED_)
#define AFX_AVOIDDENSITYEVENT_H__FA986B27_45DA_45C4_A4EA_3D5DFEC51FDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "MOVEVENT.H"
#include "common\point.h"
#include "..\inputs\areaintersectpoint.h"

class InputTerminal;
// avoid density event
class AvoidDensityEvent : public MobileElementMovementEvent  
{
private:
	int					m_iRealState;
	Point				m_ptRealDestination;
	INTERSECT_INFO_LIST m_vIntersectInfo;
	
public:
	AvoidDensityEvent();
	virtual ~AvoidDensityEvent();

public:
	int getRealState( void ) const { return m_iRealState; }
	const Point& getRealDestination( void ) const { return m_ptRealDestination;	}
	const INTERSECT_INFO_LIST& getIntersecrInfoList( void ) const { return m_vIntersectInfo;}
	
	void InitEx(  MobileElement *aMover, ElapsedTime eventTime, int _iRealState, 
				   const Point& _realPt, const INTERSECT_INFO_LIST& _infoList,bool bNoLog  );
	
	// processor event
	virtual int process (CARCportEngine *_pEngine );

	// generate next event
	void generateNextEvent( );

	// avoid the density area
	void avoidDensityArea( const INTERSECT_INFO& _info, InputTerminal* _pInTerm );

	//Returns event's name
    virtual const char *getTypeName (void) const { return "MobileElement Avoid Density Event"; };
	
    //Returns event type
    int getEventType (void) const { return MobileElementAvoidDensityEvent; };

private:
	void getPersonRealtimeDest( void );
};

#endif // !defined(AFX_AVOIDDENSITYEVENT_H__FA986B27_45DA_45C4_A4EA_3D5DFEC51FDD__INCLUDED_)
