// ReportParaOfTwoGroupProcs.h: interface for the CReportParaOfTwoGroupProcs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTPARAOFTWOGROUPPROCS_H__8CAB27A7_1F8A_415A_A0E4_1B2A597CBA7B__INCLUDED_)
#define AFX_REPORTPARAOFTWOGROUPPROCS_H__8CAB27A7_1F8A_415A_A0E4_1B2A597CBA7B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ReportParameter.h"

class CReportParaOfTwoGroupProcs : public CReportParameter  
{
	CReportParaOfTwoGroupProcs::FROM_TO_PROCS m_FromToProcs;
private:
	CReportParaOfTwoGroupProcs(const CReportParaOfTwoGroupProcs&); //disallow copy constructor
	CReportParaOfTwoGroupProcs& operator=(const CReportParaOfTwoGroupProcs&); //disallow assignment
	
public:
	CReportParaOfTwoGroupProcs( CReportParameter* _pDecorator );
	virtual ~CReportParaOfTwoGroupProcs();
public:
	//startime , end time, interval
	//**********
	virtual bool GetStartTime( ElapsedTime& _startTime )const;
	virtual bool GetEndTime( ElapsedTime& _endTime )const;
	virtual bool GetInterval ( long& _intervalTime )const;
	virtual bool SetStartTime( const ElapsedTime& _startTime );
	virtual bool SetEndTime( const ElapsedTime& _endTime );
	virtual bool SetInterval ( const long _intervalSecond );

	//**********
	// retport type
	//**********
	virtual bool GetReportType ( int& _iReportType  )const;
	virtual bool SetReportType ( int _iReportType  );

	//**********
	//Threshold
	//**********
	virtual bool GetThreshold ( long& _lThreshold )const;
	virtual bool SetThreshold ( long _lThreshold );

	//**********
	//pax type, processor group
	//**********
	virtual bool GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const;
	virtual bool GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const;

	virtual bool SetPaxType( const std::vector<CMobileElemConstraint>& _vPaxType );
	virtual bool SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup );

	//areas
	virtual bool GetAreas(  std::vector<CString>& _vAreas )const;
	virtual bool SetAreas( const std::vector<CString>& _vAreas );

	//ports
	virtual bool GetPortals(  std::vector<CString>& _vPortals )const;
	virtual bool SetPortals ( const std::vector<CString>& _vPortals );

	//from---to processors
	virtual bool GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const;
	virtual bool SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs );

	virtual void WriteClassType( ArctermFile& _file );
	virtual void WriteClassData( ArctermFile& _file );
	virtual void ReadClassData( ArctermFile& _file, InputTerminal* _pTerm );
	
	//clone
	virtual CReportParameter* Clone();
	
	virtual bool IsEqual( const CReportParameter& _anotherInstance );

};

#endif // !defined(AFX_REPORTPARAOFTWOGROUPPROCS_H__8CAB27A7_1F8A_415A_A0E4_1B2A597CBA7B__INCLUDED_)
