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
#include "common\states.h"
#include "results\resourceelementlogentry.h"
#include "inputs\resourcepool.h"
// declare
class Terminal;
typedef std::vector<int> PIPES;
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
	//Final destination of pipe
	Point m_ptDestOfPipe;
	//Point list of pipe
	std::vector<PipePointInformation> m_vPipePointList;
	//Status before enter pipe.
	EntityEvents m_statusBeforeEnterPipe;
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

	// process walking on pipe
	virtual void handleWalkOnPipe(const ElapsedTime& _time );

	//Generates the next movement event for the receiver based on its current state and adds it to the event list.
    virtual void generateEvent (ElapsedTime eventTime,bool bNoLog);

	// sets Element's destination
	virtual void setDestination (Point p){m_ptDestination = p;}

	virtual void setLocation( const Point& _ptLocation ){ location = _ptLocation;	}
	
	// process pipe, 
	// in:
	// status: the status before enter pipe.
	// _destPoint: the destination's coordinate.
	void processPipe(EntityEvents _status, Point _destPoint, const ElapsedTime _curTime);

	//Calculate the moving time.
	ElapsedTime moveTime (void) const;
private:

	CString ClacTimeString(const ElapsedTime& _curTime);

	// no pipe
	void walkStraight(Point _destPoint, const ElapsedTime _curTime);

	// automatic pipe
	void walkAlongShortestPath(Point _destPoint, const ElapsedTime _curTime);

	// user defined pipe
	void walkAlongUserDefinePath(Point _destPoint, PIPES userPipe, const ElapsedTime _curTime);
};

#endif // !defined(AFX_RESOURCEELEMENT_H__F9F00B47_F1F5_4155_A378_183EC0EB0932__INCLUDED_)
