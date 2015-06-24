// Monitor.h: interface for the CMonitor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONITOR_H__5A16AF70_D2A6_4C40_B8AA_E39B1DDDD67C__INCLUDED_)
#define AFX_MONITOR_H__5A16AF70_D2A6_4C40_B8AA_E39B1DDDD67C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class InputTerminal;
class CSimParameter;
class Processor;
class CMobileElemConstraint;
class ProcessorID;
class ElapsedTime;
//class GroupIndex;
class CSimLevelOfServiceItem;
#include<vector>
#include<map>
#include "common\template.h"
class CMonitor  
{
	typedef std::vector<char>HISTORY;
	class CHistoryRecord
	{
	public:
		CHistoryRecord(){  m_iMaxQueueLength = 0 ;};
		CString m_csProc;
		//bool m_bNeedToGenerateTLOSEvent;
		int m_iMaxQueueLength;
		std::map<CString, HISTORY>m_mapQLenHistory;
		std::map<CString, HISTORY>m_mapQTimeHistory;
		std::map<CString, bool>m_mapNeedToGenTLOSEvent;
	};

	// all history record group by processor's name
	std::vector<CHistoryRecord>m_vAllProcHistory;
	InputTerminal* m_pTerm;
	CSimParameter* m_pSimSetting;
public:
	CMonitor( InputTerminal* _pTerm, CSimParameter* _pSimSetting );
	virtual ~CMonitor();
public:

	// set processor's TLOS according to the sim setting parameter
	 void InitTLOSData();

	 void DoTLOSCheck( const ProcessorID* _pWhichProc , const ElapsedTime& _currentTime , const CString& _csPaxType );

	 // when person arrival at processor's queue, call this function to notify the monitor
	 void PersonArrivalAtProcQueue( const Processor* _pWhichProc , const ElapsedTime& _currentTime , const CMobileElemConstraint& _paxType );

	 Processor* OpenProcessor( std::vector<Processor*>& _procs , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const;
	 	// call roster to open one of processors in proc group  for such pax type at such time
	 Processor* OpenProcessor( const ProcessorID& _procGroupID , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const;

	 void OpenProcessor( Processor* _pProcToOpen , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const;


private:
	// test queue length to see if should open or close some processor 

	 int CheckQueueLength( const std::vector<Processor*>& _vProcs ,const CSimLevelOfServiceItem* _pServiceItem, const CMobileElemConstraint& _paxType  ,CHistoryRecord& _history );
	 int GetCurrentMaxQueueLen( const std::vector<Processor*>& _vProcs )const;
	 //int GetCurrentMaxQueueLen( const std::vector<int>& _vProcs )const;

	 // test waiting in queue time to see if should open or close some processor 
	 
	 int CheckQueueTime( const std::vector<Processor*>& _vProcs ,const CSimLevelOfServiceItem* _pServiceItem, const CMobileElemConstraint& _paxType  ,CHistoryRecord& _history ,const ElapsedTime& _currentTime);
	 int GetCurrentMaxQueueTime(const std::vector<Processor*>& _vProcs ,const ElapsedTime& _currentTime) const;

	 // generate a new TLOS event and add it into event list
	 void GenerateNewTLOSEvent( const ProcessorID* _pWhichProc , const ElapsedTime& _currentTime, const CString& _csPaxType );

	 // decide opening processor or closing processor according to history record
	 int OpenOrClose(char _chNewState, HISTORY& _history );



	 // call roster to close one of processors in proc group for such pax type at such time
	 Processor* CloseProcessor( const ProcessorID& _procGroupID , const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const;

	 // call roster to close one of processors in proc group for such pax type at such time
	 Processor* CMonitor::CloseProcessor(  std::vector<Processor*>_vProcs, const ElapsedTime& _currentTime  ,const CMobileElemConstraint& _paxType )const;	 

};

#endif // !defined(AFX_MONITOR_H__5A16AF70_D2A6_4C40_B8AA_E39B1DDDD67C__INCLUDED_)
