// AutoRoster.h: interface for the CAutoRoster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUTOROSTER_H__E828F287_4AEE_480E_AA8B_10660C68C47D__INCLUDED_)
#define AFX_AUTOROSTER_H__E828F287_4AEE_480E_AA8B_10660C68C47D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <map>
#include "common\elaptime.h"
#include "inputs\assign.h"
#include <CommonData/procid.h>

class Processor;
class ProcessorID;
class CMobileElemConstraint;
class InputTerminal;
class CSimLevelOfService;

typedef std::vector<ProcessorRoster> ProcAssList;
//////////////////////////////////////////////////////////////////////////
class CAutoRosterItem
{
public:
	CAutoRosterItem();
	~CAutoRosterItem();

private:
	std::map< ProcessorID, ProcAssList> m_mapAutoProcAss;
	
public:
	void clearData( void );

	void addProcAss( const ProcessorID& _id,const ProcessorRoster& _procAss );
	std::map< ProcessorID, ProcAssList>& getProcAssMap( void );
	void addAutoRostorToDateSet( InputTerminal* _inTerm);
	
private:
	void SortVector(ProcAssList& pa_list);
	void addProcAssToDataSet( const ProcessorID& id,InputTerminal* _inTerm, std::vector<ProcessorRoster*>& _newAssList);
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CAutoRoster  
{
public:
	CAutoRoster();
	virtual ~CAutoRoster();

private:
	CAutoRosterItem* m_pAutoRoster;		// store CAutoRosterItem of each sim_run
	int m_iRosterItemCount;				// the num of sim_run
	int m_iCurRosterIndex;				// current sim_run index
		
public:
	void clearUserRoster( InputTerminal* _inTerm, const CSimLevelOfService*  _pTLOSSim = NULL );
	void init( int _numOfRun );
	void setCurRosterIndex( int _index );
	int  getCurRosterIndex( void );

	bool openProcessor( const ElapsedTime&  _openTime, Processor* _proc, const CMobileElemConstraint& _mob );
	bool closeProcssor( const ElapsedTime& _closeTime, Processor* _proc, const CMobileElemConstraint& _mob );
	
	Processor* openProcessor( const ElapsedTime&  _openTime, const ProcessorID& _procID, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );
	Processor* closeProcssor( const ElapsedTime& _closeTime, const ProcessorID& _procID, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );
	
	Processor* openProcessor( const ElapsedTime&  _openTime,  std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );
	Processor* closeProcssor( const ElapsedTime& _closeTime,  std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );
	Processor* closeProcssor( const ElapsedTime& _closeTime,  std::vector<int>& _procIndexList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );
	
	void closeAllProcssor( const ElapsedTime& _closeTime,  const std::vector<Processor*>& _procList, const CMobileElemConstraint& _mob , InputTerminal* _inTerm );


	void addAutoRostorToDateSet( InputTerminal* _inTerm);
	void addAutoRostorToDateSet( int _index ,InputTerminal* _inTerm);

	void clearTLOSProcRoster( const CSimLevelOfService*  _pTLOSSim, InputTerminal* _inTerm, bool _bAddCloseEvent = true);

private:
	void clear();

	void closeAllTLOSProc( const CSimLevelOfService*  _pTLOSSim, InputTerminal* _inTerm );
};

#endif // !defined(AFX_AUTOROSTER_H__E828F287_4AEE_480E_AA8B_10660C68C47D__INCLUDED_)
