#pragma once
#include "parameters.h"
#include "AirsideReportAPI.h"

class AIRSIDEREPORT_API CRunwayExitItem
{
public:
	enum Operation_type {ALL,Takeoff,Landing};
protected:
	int m_RunwayID ;
	int m_ExitID ;
	CString m_RunwayName ;
	CString m_ExitName ;
	Operation_type m_Operation ;
public:
	static CString GetOperationName(Operation_type _oper)
	{
		switch(_oper)
		{
		case CRunwayExitItem::ALL:
			return _T("All") ;
			break;
		case CRunwayExitItem::Takeoff:
			return _T("T/O") ;
		case CRunwayExitItem::Landing:
			return _T("Landing") ;
		default:
			return _T("Unknown") ;
		}
	}
public:
	void SetRunwayID(int _runway) { m_RunwayID = _runway ;} ;
	int GetRunwayID() { return m_RunwayID ;} ;

	void SetExitID(int _exitid) { m_ExitID = _exitid ;} ;
	int GetExitID() { return m_ExitID ;} ;

	void SetRunwayName(const CString& _val) { m_RunwayName = _val ;} ;
	CString GetRunwayName() { return m_RunwayName ;} ;

	void SetExitName(const CString& _val) { m_ExitName = _val ;} ;
	CString GetExitName() { return m_ExitName ;} ;

	void SetOperationType(Operation_type _oper) { m_Operation = _oper ;} ;
	Operation_type GetOperationType() { return m_Operation ;} ;

};
class RunwayExitRepDefineData_Exit
{
public:
	CString m_ExitName ;
	CString m_runwayName ;
	int m_ExitID ;
	int m_RunwayID ;
	RunwayExitRepDefineData_Exit():m_ExitID(-1),m_RunwayID(-1)
	{

	}
};
class RunwayExitRepDefineData_Runway
{
public:
	CString m_RunwayName ;
	int m_Operater ;
	int m_RunwayID ;
	RunwayExitRepDefineData_Runway():m_Operater(0) ,m_RunwayID(-1)
	{

	}
	std::vector<RunwayExitRepDefineData_Exit> m_data ;

	void AddExitName( CString _name,CString _runwayName ,int _ExitID ,int _RunwayID )
	{
		for (int i = 0 ; i< (int)m_data.size() ;i++)
		{
			if(m_data[i].m_ExitName.CompareNoCase(_name) == 0)
				return ;
		}
		RunwayExitRepDefineData_Exit exit ;
		exit.m_ExitName = _name ;
		exit.m_runwayName = _runwayName ;
		exit.m_RunwayID = _RunwayID ;
		exit.m_ExitID  = _ExitID ;
		m_data.push_back(exit) ;
	}

	void ExportFile(ArctermFile& _file)
	{
		_file.writeField(m_RunwayName) ;
		_file.writeInt(m_Operater) ;
		_file.writeInt(m_RunwayID) ;
		_file.writeLine() ;
		_file.writeInt((int)m_data.size()) ;
		_file.writeLine() ;

		for (int i = 0 ; i < (int)m_data.size() ;i++)
		{
			_file.writeField(m_data[i].m_ExitName) ;
			_file.writeField(m_data[i].m_runwayName) ;
			_file.writeInt(m_data[i].m_ExitID) ;
			_file.writeInt(m_data[i].m_RunwayID) ;
			_file.writeLine() ;
		}
	}
	void ImportFile(ArctermFile& _file)
	{
		TCHAR th[1024] = {0} ;
		_file.getField(th,1024) ;
		m_RunwayName.Format(_T("%s"),th) ;
		_file.getInteger(m_Operater) ;
		_file.getInteger(m_RunwayID) ;
		_file.getLine() ;

		int size = 0 ;
		if(!_file.getInteger(size) )
			return ;
		for (int i = 0 ; i< size ;i++)
		{
			_file.getLine() ;
			RunwayExitRepDefineData_Exit exit ;
			_file.getField(th,1024) ;
			exit.m_ExitName.Format(_T("%s"),th) ;

			_file.getField(th,1024) ;
			exit.m_runwayName.Format(_T("%s"),th) ;

			_file.getInteger(exit.m_ExitID) ;
			_file.getInteger(exit.m_RunwayID) ;
			m_data.push_back(exit) ;
			
		}
	}
};
class RunwayExitRepDefineData
{
public:
	RunwayExitRepDefineData() {};
	~RunwayExitRepDefineData() 
	{
		ClearData() ;
	}
	std::vector<RunwayExitRepDefineData_Runway*> m_data ;

	RunwayExitRepDefineData_Runway* AddRunway(CString _runway , int oper) 
	{
		for (int i = 0 ; i < (int)m_data.size() ;i++)
		{
			if(_runway.CompareNoCase( m_data[i]->m_RunwayName)  == 0)
				return m_data[i];
		}
		RunwayExitRepDefineData_Runway* Pdata = new RunwayExitRepDefineData_Runway ;
		Pdata->m_RunwayName = _runway ;
		Pdata->m_Operater = oper ;
		m_data.push_back(Pdata) ;
		return Pdata ;
	}
	void ClearData()
	{
		for (int i = 0 ; i < (int)m_data.size() ;i++)
		{
				delete m_data[i];
		}
		m_data.clear() ;
	}

	void ExportFile(ArctermFile& _file)
	{
		_file.writeInt((int)m_data.size()) ;
		
		_file.writeLine() ;
		for (int i = 0 ; i < (int)m_data.size() ;i++)
		{
			m_data[i]->ExportFile(_file) ;
		}
	}

	void ImportFile(ArctermFile& _file)
	{
		int size = 0 ;
		if(!_file.getInteger(size))
			return ;
		
		for (int i = 0 ; i < size ;i++)
		{
			_file.getLine() ;
			RunwayExitRepDefineData_Runway* Pdata = new RunwayExitRepDefineData_Runway ;
			Pdata->ImportFile(_file) ;
			m_data.push_back(Pdata) ;
		}
	}
};
class AIRSIDEREPORT_API CRunwayExitParameter :
	public CParameters
{
public:
	CRunwayExitParameter(void);
	~CRunwayExitParameter(void);
public:
	std::vector<CRunwayExitItem*> m_RunwayData ;

	RunwayExitRepDefineData m_ShowData ;
public:
	void AddRunwayExit(int _runwayID ,int _exitID ,const CString& _runwayName ,const CString& _exitname,CRunwayExitItem::Operation_type _oper) ;
public:
	CRunwayExitItem* FindItemByExitID(int _exitID) ;
	CRunwayExitItem* GetItem(int nIdx);
	int GetCount()const;

public:
	virtual BOOL ExportFile(ArctermFile& _file) ;
	virtual BOOL ImportFile(ArctermFile& _file) ;

	virtual void LoadParameter() {} ;
	virtual void UpdateParameter() {} ;

	//write and load parameter from file
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
	virtual CString GetReportParamName();

	void ClearData() ;

	RunwayExitRepDefineData* GetShowData() { return &m_ShowData ;};

	void GetDataFromShowData() ;
	CRunwayExitItem::Operation_type m_AllOper ;
}; 
