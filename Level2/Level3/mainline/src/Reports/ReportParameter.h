// ReportParameter.h: interface for the CReportParameter class.
//
#pragma once
#include "ReportType.h"

class ElapsedTime;
class CMobileElemConstraint;
class ProcessorID;
class ArctermFile;
class InputTerminal;

enum ReportParaClass
{
	PaxType_Para,
	ProcGroup_Para,
	ReportType_Para,
	Threshold_Para,
	Time_Para,
	Areas_Para,
	Portals_Para,
	FromTo_Para,
	Runs_Para,
};

class CReportParameter  
{
public:
	std::vector<CMobileElemConstraint> Thrognput_pax; 
protected:
	CReportParameter( CReportParameter* _pDecorator );
	
protected:
	CReportParameter* m_pDecorator;
	CString m_strName;
	ENUM_REPORT_TYPE m_enReportCategory;
private:
	CReportParameter(const CReportParameter&); //disallow copy constructor
	CReportParameter& operator=(const CReportParameter&); //disallow assignment
public:
	struct FROM_TO_PROCS
	{
		std::vector<ProcessorID> m_vFromProcs;
		std::vector<ProcessorID> m_vToProcs;
	};
		
public:
	virtual ~CReportParameter();
	void SetName( const CString& _strName ){ m_strName = _strName;	};
	CString GetName( )const{ return m_strName;	};
	void SetDecorator( CReportParameter* _pDecorator ){ m_pDecorator = _pDecorator;	}
	CReportParameter* GetDecorator() { return m_pDecorator;	};
	void SetReportCategory( ENUM_REPORT_TYPE _enCategory ){ m_enReportCategory = _enCategory;}
	ENUM_REPORT_TYPE GetReportCategory()const{ return m_enReportCategory;}

	//********** 
	//startime , end time, interval
	//**********
	virtual bool GetStartTime( ElapsedTime& _startTime )const=0;
	virtual bool GetEndTime( ElapsedTime& _endTime )const=0;
	virtual bool GetInterval ( long& _intervalSecond )const=0;
	virtual bool SetStartTime( const ElapsedTime& _startTime )=0;
	virtual bool SetEndTime( const ElapsedTime& _endTime )=0;
	virtual bool SetInterval ( const long _intervalSecond )=0;
	
	//**********
	// retport type
	//**********
	virtual bool GetReportType ( int& _iReportType  )const=0;
	virtual bool SetReportType ( int _iReportType  )=0;

	//**********
	//Threshold
	//**********
	virtual bool GetThreshold ( long& _lThreshold )const = 0;
	virtual bool SetThreshold ( long _lThreshold )= 0;

	//**********
	//pax type, processor group
	//**********
	virtual bool GetPaxType( std::vector<CMobileElemConstraint>& _vPaxType )const=0;
	virtual bool GetProcessorGroup( std::vector<ProcessorID>& _vProcGroup )const=0;

	virtual bool SetPaxType( const std::vector<CMobileElemConstraint>& _vPaxType )=0;
	virtual bool SetProcessorGroup( const std::vector<ProcessorID>& _vProcGroup )=0;

	//areas
	virtual bool GetAreas(  std::vector<CString>& _vAreas )const=0;
	virtual bool SetAreas( const std::vector<CString>& _vAreas )=0;

	//ports
	virtual bool GetPortals(  std::vector<CString>& _vPortals )const=0;
	virtual bool SetPortals ( const std::vector<CString>& _vPortals )=0;

	//from---to processors	
	virtual bool GetFromToProcs(  FROM_TO_PROCS& _fromToProcs )const=0;
	virtual bool SetFromToProcs ( const FROM_TO_PROCS& _fromToProcs )=0;

	//runs
	virtual bool GetReportRuns ( std::vector<int>& vReportRuns )const;
	virtual bool SetReportRuns ( const std::vector<int>& vReportRuns );
	virtual bool IsMultiRunReport();

	//read ,write
	virtual void WriteClassType( ArctermFile& _file )=0;
	virtual void WriteClassData( ArctermFile& _file )=0;
	virtual void ReadClassData( ArctermFile& _file , InputTerminal* _pTerm)=0;
	void SaveReportParaToFile( const CString& _strPathName );
	static CReportParameter* LoadReoprtParaFromFile(  const CString& _strPathName,InputTerminal* _pInTerm );
	//clone
	virtual CReportParameter* Clone()=0;
	
	bool operator == ( const CReportParameter& _anotherInstance );
	virtual bool IsEqual( const CReportParameter& _anotherInstance )=0;	
};

//******************** How To Use***************
// Generally to say, it use a design pattern named " Decorate pattern "
/**** sample
//***************
#include"ReportParaOfTime.h"
#include"ReportParaWithArea.h"
#include"ReportParaOfThreshold.h"
#include<iostream>
main()
{
	CReportParameter* p = new CReportParaOfTime( new CReportParaWithArea( new CReportParaOfThreshold( NULL) ) );
	p->SetInterval(200);
	p->SetThreshold( 45 );
	long temp;
	if( !p->GetInterval( temp ) )
	{
		AfxMessageBox("error");
	}
	std::cout<<" interval = "<<temp<<std::endl;
	if( !p->GetThreshold( temp ) )
	{
		AfxMessageBox("error");
	}
	std::cout<<" threshold = "<<temp<<std::endl;
	
}

****/
//////////////////