// CollisonTimeSegment.h: interface for the CCollisonTimeSegment class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLLISONTIMESEGMENT_H__B5415468_55B2_4A29_9452_DC4EF8DCECBE__INCLUDED_)
#define AFX_COLLISONTIMESEGMENT_H__B5415468_55B2_4A29_9452_DC4EF8DCECBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include<map>
#include<vector>
#include "common\elaptime.h"
class CMobileElemConstraint;
class ArctermFile;
class Pollygon;
///#include "engine\terminal.h"
class Terminal;
typedef struct
{
	long m_lFirstTrackIdx;
	long m_lSecondTrackIdx;
}TRACKPAIR;
typedef std::vector<TRACKPAIR> TRACKPAIRVECTOR;
//typedef std::map<int,int> INDEXMAP;
//typedef std::vector<TRACKPAIRVECTOR> PERSONTRACK;
typedef std::map<int,TRACKPAIRVECTOR*> MAP_PERSON;
class CCollisonTimeSegment  
{
private:
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	//INDEXMAP m_mapIndex;
	//PERSONTRACK	m_vPersonTrackInfo;
	MAP_PERSON m_mapPersonTrackInfo;
	Terminal* m_pTerm;
	
public:
	void Init( ElapsedTime& _startTime, ElapsedTime&  _endTime , Terminal* _pTerm )
	{ m_startTime = _startTime; m_endTime = _endTime; m_pTerm = _pTerm ;};
	void AddPersonTrackInfo( int _iPersonIdx, TRACKPAIR& _trackPair ,ElapsedTime& _startTime,ElapsedTime& _endTime );
	void CaculateAll( const CMobileElemConstraint& _paxType, ArctermFile& _file,float _threshold ,Pollygon& _region );
	void ClearAll();
	CCollisonTimeSegment();
	virtual ~CCollisonTimeSegment();


};

#endif // !defined(AFX_COLLISONTIMESEGMENT_H__B5415468_55B2_4A29_9452_DC4EF8DCECBE__INCLUDED_)
