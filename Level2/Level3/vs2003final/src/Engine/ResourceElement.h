// ResourceElement.h: interface for the ResourceElement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEELEMENT_H__F9F00B47_F1F5_4155_A378_183EC0EB0932__INCLUDED_)
#define AFX_RESOURCEELEMENT_H__F9F00B47_F1F5_4155_A378_183EC0EB0932__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "MOBILE.H"
#include "results\resourceelementlogentry.h"
#include "inputs\resourcepool.h"
#include "Inputs\TimePointOnSideWalk.h"
// declare
class Terminal;

/************************************************************************/
/*                       ResourceElement                                */
/************************************************************************/
class ResourceElement : public MobileElement  
{
public:
	ResourceElement( Terminal* _pTerm, CResourcePool* _pool, int _poolIdx, long _id, double _speed, int _index_in_pool );
	virtual ~ResourceElement();
	struct PipePointInformation 
	{
		Point pt;
		int m_nPrePipe;
		int m_nCurPipe;
	};
// data member
private:
	Terminal*		m_pTerm;
	CResourcePool*	m_pOwnPool;
	CString			m_strName;
	ResourceElementLogEntry m_logEntry;
	//MobileElement*	m_pCurrentServicePax;
	ResourceRequestItem m_CurrentRequestItem;

	//Current coordinates
	Point location;       

	//Current destination, used for
	Point m_ptDestination;      
	std::vector<PipePointInformation> m_vPipePointList;
// interface 
public:
	// get current service pax
	const ResourceRequestItem& getCurrentRequestItem( void );

	// set current service pax
	void setCurrentRequestItem ( const ResourceRequestItem& _item );

	//Event handling routine for all movements.
    virtual int move (ElapsedTime currentTime,bool bNoLog);
	
	//Writes the element's current location, state to the log file.
	// in:
	// _bBackup: if the log is backwards.
    virtual void writeLogEntry (ElapsedTime time, bool _bBackup, bool bOffset = true );

	//Performs all required functions before element's destruction
    virtual void kill (ElapsedTime killTime);

	// Dumps all tracks to the log and updates ElementDescription record.
    virtual void flushLog (ElapsedTime currentTime);

	// process birth
	virtual void handleBirth( const ElapsedTime& _time );

	// process death
	virtual void handleDeath( const ElapsedTime& _time );

	// processor stay in bas
	virtual void handleStayInBase( const ElapsedTime& _time );

	// processor arrival processor
	virtual void handleArrivalProcessor( const ElapsedTime& _time );

	// processor leave processor
	virtual void handleLeaveProcessor(  const ElapsedTime& _time ); 

	// processor back to bas
	virtual void handleBackToBase( const ElapsedTime& _time );

	//Sets Element's destination
	virtual void handleWalkOnPipe(const ElapsedTime& _time );

	//Generates the next movement event for the receiver based on its current state and adds it to the event list.
    virtual void generateEvent (ElapsedTime eventTime,bool bNoLog);

	//Sets Element's destination
	virtual void setDestination (Point p){m_ptDestination = p;}

	virtual void setLocation( const Point& _ptLocation ){ location = _ptLocation;	}

	void WalkAlongShortestPath(Point entryPoint, const ElapsedTime _curTime);

	ElapsedTime moveTime (void) const;
private:
	void WritePipeLogs( PTONSIDEWALK& _vPointList, const ElapsedTime _eventTime,  bool _bNeedCheckEvacuation = false );
};

#endif // !defined(AFX_RESOURCEELEMENT_H__F9F00B47_F1F5_4155_A378_183EC0EB0932__INCLUDED_)
