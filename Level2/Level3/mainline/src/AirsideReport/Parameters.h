#pragma once
#include "../Database/ARCportDatabaseConnection.h"
#include "../Common/FLT_CNST.H"
#include <vector>
#include "../Common/elaptime.h"
#include "ParameterCaptionDefine.h"
#include "AirsideReportAPI.h"

class CXTPPropertyGrid;
class CXTPPropertyGridItem;
class CUnitsManager;
class CAirportDatabase ;
class AIRSIDEREPORT_API CParameters
{
public:
	CParameters();
	virtual ~CParameters();

//members
public:

	//report type
	char m_modelName[255];

//operation
public:
	virtual void LoadParameter() = 0;
	virtual void UpdateParameter() = 0 ;
	
	
	std::vector<FlightConstraint>& getFlightConstraint(){ return m_vFlightConstraint;}

	bool IsFlightScheduleExist(FlightConstraint& fltCons);
	void AddFlightConstraint(FlightConstraint& fltCons);
	bool DelFlightConstraint(size_t nIndex);
	size_t getFlightConstraintCount();
	FlightConstraint getFlightConstraint(size_t nIndex);
	void ClearFlightConstraint();


	ElapsedTime getStartTime(){ return m_startTime;}
	void setStartTime(ElapsedTime time){ m_startTime = time;}
	ElapsedTime getEndTime(){ return m_endTime;}
	void setEndTime(ElapsedTime time){ m_endTime = time;}
	long getInterval(){ return m_lInterval;}
	void setInterval(long  interval){ m_lInterval = interval;}

	void setReportType(enumASReportType_Detail_Summary rpType){ m_reportType = rpType;}
	enumASReportType_Detail_Summary getReportType(){  return m_reportType;}

	void SetLengthUnit(int unit){ m_unitLength = unit;}
	int GetLengthUnit(){ return m_unitLength;}
	CString GetLengthUnitName();

	CString GetParameterString();
public:
	virtual void SetModelName(char* modelName);
	virtual char* GetModelName();

	void SetProjID(int nProiID) ;
	int GetProjID();

	void SetUnitManager(CUnitsManager *unitManager){ m_pUnitManager = unitManager;}
	CUnitsManager *GetUnitManager(){ return m_pUnitManager;}

	void SetReportFileDir(const CString& strReportPath);
	virtual CString GetReportParamName();

	CString GetReportParamPath();

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}

    bool GetMultiRun() const { return m_multiRun; }
    void SetMultiRun(bool val) { m_multiRun = val; }

    bool GetReportRuns(std::vector<int>& vReportRuns);
    void ReportRuns(std::vector<int> val) { m_vReportRuns = val; }
protected:
	//chart type
	int   m_nSubType;
protected:
	ElapsedTime m_startTime;
	ElapsedTime m_endTime;
	long m_lInterval;

	int m_nProjID;
	std::vector<FlightConstraint> m_vFlightConstraint;

	enumASReportType_Detail_Summary m_reportType;

	int m_unitLength;
    CAirportDatabase* m_AirportDB ;
	CString			m_strReportPath;
    bool m_multiRun;
    std::vector<int> m_vReportRuns;
public:
	void SetAirportDB(CAirportDatabase* _AirportDB) { m_AirportDB = _AirportDB ;} ;
	CAirportDatabase* GetAirportDB(){return m_AirportDB;}
protected:
	CUnitsManager *m_pUnitManager;

public:
	//the 2 function are not used in new structure
	virtual BOOL ExportFile(ArctermFile& _file) ;
	virtual BOOL ImportFile(ArctermFile& _file) ;

public:
	//the following functions are used in new structure
	//read and save data from the report path, in the project path
	void LoadParameterFile();
	void SaveParameterFile();
	
	//export and import parameter
	void LoadParameterFromFile(ArctermFile& _file);
	void SaveParameterToFile(ArctermFile& _file);

	//write the parameter data
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);
};
