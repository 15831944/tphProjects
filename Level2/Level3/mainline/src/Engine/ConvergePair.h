// ConvergePair.h: interface for the ConvergePair class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONVERGEPAIR_H__A2D61440_7F98_4F5B_8716_15F0ED8ABDB6__INCLUDED_)
#define AFX_ONVERGEPAIR_H__A2D61440_7F98_4F5B_8716_15F0ED8ABDB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "common\template.h"
//#include "engine\Terminal.h"
#include <CommonData/procid.h>
#include "engine\process.h"
//#include "engine\hold.h"
#include "engine\proclist.h"
//#include "engine\person.h"
#include "results\paxentry.h"
#include <vector>


class ConvergePair  
{
public:
	ConvergePair(int _iLeadIndex,int _iFollowIndex);//,int _iCreaterIndex);
	virtual ~ConvergePair();

protected:
	int m_iLeadIndex; // leader id
	int m_iFollowIndex; // follower id
	Person * m_pLeader;
	Person * m_pFollow;
	bool m_bLeadArrival;
	bool m_bFollowArrival;
	bool m_bFollowInHolding;
	bool m_bLeaderWaiting;
	bool m_bFollowWaiting;

	bool m_bDelete;
//	int  m_iCreatePairId;         // who create the pair ,if lead,it store the leadid,else store the followid;
    Terminal* m_pTerm;
	int m_iVisitorType;
	Processor * m_BeforeLinkWaitProc;
public:
	//CSinglePaxTypeFlow *m_beforeProcessFlow; 
	//ProcessorID m_beforeProcessFlowCurID;
public:
	bool AmNowConvergeProcessor(int _index,Processor **_iProc,Person *_pPerson,bool _bStationOnly, ElapsedTime _curTime);
	Processor * GetNextConvergeProcessor(int _iIndex,ProcessorArray&_nextProcGroup); //if return the HoldingArea;set the m_FollowInHolding==1;
	void ReportArrival(int _index,ElapsedTime p_time);//set the ArrivalTag;
	bool IsMyPair(int _iFirIndex,int _iSecIndex); //check if index pair is the leadindex and followindex ,if not return false,else set two person*;
    bool IsMyPair(int _iIndex);//check one index is have in pair;
	bool SetPoint(int _id,Person * _pPerson);//set Point By id, 
	void exchangeLeadFollow(void);
    void ClearTag();
	void SetTerminal(Terminal *_pTerm) {m_pTerm = _pTerm;}
	bool IsLeader(int _index){return(_index==m_iLeadIndex);}
	bool IsLeadArrival(){ return m_bLeadArrival;}
	bool IsFollowArrival(){ return m_bFollowArrival;}
	bool IsFollowHolding(){ return m_bFollowInHolding;}
	bool getWaitTag(int _index);
	void setWaitTag(int _index,bool _value);
	Person* GetLeaderPoint(){return m_pLeader;}
	Person* GetFollowerPoint(){return m_pFollow;}
	void ClearLeader(){m_pLeader = NULL;}
	void ClearFollower(){m_pFollow = NULL;}
	int GetLeaderIndex(){return m_iLeadIndex;}
	int GetFollowerIndex(){return m_iFollowIndex;}
	void setVisitorType(int _iVisitorType){m_iVisitorType = _iVisitorType;}

	void setDeleteTag(){m_bDelete = true;}
	bool getDeleteTag(){return m_bDelete;}
};

#endif // !defined(AFX_ONVERGEPAIR_H__A2D61440_7F98_4F5B_8716_15F0ED8ABDB6__INCLUDED_)





















