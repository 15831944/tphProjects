#ifndef ITFLIGHT_H
#define ITFLIGHT_H
#pragma once
#include "../Inputs/BaseFlight.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "..\Common\Flt_cnst.h"
#include "../Common/elaptime.h"
#include "../Common/ARCFlight.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API ItinerantFlightScheduleItem :  public ARCFlight,public DBElement
{
public:
	ItinerantFlightScheduleItem();
	~ItinerantFlightScheduleItem();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

public:

// 	void SetName(CString strName){m_strName = strName;}
// 	CString GetName(){return m_strName;}

	void SetEntryID(int nEntryID){m_nEntryID = nEntryID;}
	int GetEntryID(){return m_nEntryID;}
	void SetExitID(int nExitID){m_nExitID = nExitID;}
	int GetExitID(){return m_nExitID;}
// 	void SetArrTime(ElapsedTime arrTime){ m_arrTime = arrTime;}
// 	ElapsedTime* GetArrTime(){return &m_arrTime;}

// 	void SetDepTime(ElapsedTime depTime) { m_depTime = depTime;}
// 	ElapsedTime* GetDepTime() { return &m_depTime;}

// 	void SetFlightID(int nFltID){ m_nFltID = nFltID; }
// 	int  GetFlightID(){return m_nFltID; }

	void SetEntryFlag(int nEntryFlag){m_nEntryFlag = nEntryFlag;}
	int GetEntryFlag(){return m_nEntryFlag;}

	void SetExitFlag(int nExitFlag){m_nExitFlag = nExitFlag;}
	int GetExitFlag(){return m_nExitFlag;}

	void SetEnRoute(BOOL bEnRoute) {m_bEnRoute = bEnRoute;}
	BOOL GetEnRoute(){return m_bEnRoute;}

// 	void SetACType(CString strACType) { m_strACType = strACType;}
// 	CString GetACType() { return m_strACType;}

	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

	void ImportDataSave(int nNewProjID, CString& strFltType);
	virtual enumFlightType getFlightType() { return ItinerantFlightType;}

private:
//	CString				m_strName;
//	FlightConstraint	m_fltType;
	int					m_nEntryID;
	int					m_nExitID;
//	ElapsedTime			m_arrTime;
//	ElapsedTime			m_depTime;
//	int					m_nFltID;
	int					m_nEntryFlag;
	int					m_nExitFlag;
//	CString				m_strACType;
	BOOL				m_bEnRoute;
	
	CAirportDatabase	*m_pAirportDB;
};

class INPUTAIRSIDE_API ItinerantFlightSchedule : public DBElementCollection < ItinerantFlightScheduleItem >
{
public:
	ItinerantFlightSchedule(void);
	virtual ~ItinerantFlightSchedule(void);

protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

public:
	void ReadData(int nParentID);
	static void ExportItinerantFlightSchedule(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportItinerantFlightSchedule(CAirsideImportFile& importFile);
	
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
private:
	CAirportDatabase* m_pAirportDB;
private:
	//export dll symbols
	void DoNotCall();

};

#endif










