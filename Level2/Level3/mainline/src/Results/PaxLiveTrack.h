// PaxLiveTrack.h: interface for the CPaxLiveTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXLIVETRACK_H__ABBE7F47_667F_4037_9422_0A824749D19C__INCLUDED_)
#define AFX_PAXLIVETRACK_H__ABBE7F47_667F_4037_9422_0A824749D19C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "common\term_bin.h"
#include <vector>

// declare
class Terminal;
typedef std::vector< MobEventStruct > event_list;

// get a pax's all event track by pax id
class CPaxLiveTrack  
{
public:
	CPaxLiveTrack();
	virtual ~CPaxLiveTrack();

private:
	long		m_lPaxID;
	long		m_lLogIndex;
	event_list	m_vTrack;

public:
	// get & set 
	void setPaxID( long _lID );
	void setLogIndex( long _lIndex );
	long getPaxID( void ) const;
	long getLogIndex( void ) const;
	long getEventCount( void ) const;
	
	const MobEventStruct& getEvent( long _lIdx ) const;

	// init
	bool initByID( long _lID, Terminal* _term, const CString& _strPrjPath );
	bool initByIndex( long _lIndex, Terminal* _term, const CString& _strPrjPath );
	void clear( void );
};

bool MobEventStructIsEqual( const MobEventStruct& _stuct1, const MobEventStruct& struct2 );
#endif // !defined(AFX_PAXLIVETRACK_H__ABBE7F47_667F_4037_9422_0A824749D19C__INCLUDED_)
