#pragma once
#include "reportparameter.h"

class CReportParaOfRuns :
	public CReportParameter
{
private:
	//int m_iReportType;

	std::vector<int > m_vReportRuns;

public:
	CReportParaOfRuns(CReportParameter* _pDecorator);
	~CReportParaOfRuns(void);
private:
	CReportParaOfRuns(const CReportParaOfRuns&); //disallow copy constructor
	CReportParaOfRuns& operator=(const CReportParaOfRuns&); //disallow assignment



public:

	//********** 
	//startime , end time, interval
	//**********
	virtual bool GetStartTime( ElapsedTime& _startTime )const;
	virtual bool GetEndTime( ElapsedTime& _endTime )const;
	virtual bool GetInterval ( long& _intervalTime )const;
	virtual bool SetStartTime( const ElapsedTime& _startTime );
	virtual bool SetEndTime( const ElapsedTime& _endTime );
	virtual bool SetInterval ( const long _intervalSecond );

	//**********
	// Runs
	//**********
	virtual bool GetReportRuns ( std::vector<int>& vReportRuns )const;
	virtual bool SetReportRuns ( const std::vector<int>& vReportRuns );
	virtual bool IsMultiRunReport();
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
	virtual bool GetAreas( std::vector<CString>& _vAreas )const;
	virtual bool SetAreas( const std::vector<CString>& _vAreas );
	//ports
	virtual bool GetPortals(  std::vector<CString>& _vPortals )const;
	virtual bool SetPortals ( const std::vector<CString>& _vPortals );

	//from---to processors
	virtual bool GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const;
	virtual bool SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs );

	//read ,write
	virtual void WriteClassType( ArctermFile& _file );
	virtual void WriteClassData( ArctermFile& _file );
	virtual void ReadClassData( ArctermFile& _file, InputTerminal* _pTerm );

	//clone
	virtual CReportParameter* Clone();
	virtual bool IsEqual( const CReportParameter& _anotherInstance );












};
