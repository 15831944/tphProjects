#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "ALTObject.h"
#include "../Common/elaptime.h"
#include "..\common\ProbabilityDistribution.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API ItinerantFlight : public DBElement
{
public:
	ItinerantFlight();
	virtual ~ItinerantFlight();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:
	CString		m_strCode;
	BOOL		m_bEnRoute;
//	FlightConstraint  m_fltType;
	int			m_nEntryID;
	int         m_nEntryFlag;
	int			m_nExitID;
	int			m_nExitFlag;
	int			m_nStandID;
	ElapsedTime m_startArrTime;
	ElapsedTime m_endArrTime;
	ElapsedTime m_startDepTime;
	ElapsedTime m_endDepTime;
	int			m_nFlightCount;
	CString		m_strArrDistName;
	CString		m_strArrDistScreenPrint;
	ProbTypes	m_enumArrProbType;  // distribution(for Engin)

	CString		m_strDepDistName;
	CString		m_strDepDistScreenPrint;
	ProbTypes	m_enumDepProbType;
	CString		m_strDepprintDist;
		
	CString		m_strArrprintDist;
	CString		m_strACType;
	CString		m_strOrig;
	CString		m_strDest;

	ProbabilityDistribution* m_pDistribution;
	CAirportDatabase* m_pAirportDB;

public:
	void SetCode(CString strCode){m_strCode = strCode;}
	CString GetName(){return m_strCode;}

	void SetEnRoute(BOOL bEnRoute) { m_bEnRoute = bEnRoute;}
	BOOL GetEnRoute(){return m_bEnRoute;}

	void SetOrig(CString strOrig){m_strOrig = strOrig;}
	CString GetOrig(){return m_strOrig;}

	void SetDest(CString strDest) { m_strDest = strDest;}
	CString GetDest() { return m_strDest;}

	void SetACType(CString strACType) { m_strACType = strACType;}
	CString GetACType() { return m_strACType;}

// 	void SetStandID(int nStandID) { m_nStandID = nStandID;}
// 	int GetStandID() {return m_nStandID;}
//	FlightConstraint& GetFltType() { return m_fltType; }
//	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	void SetEntryID(int nEntryID){m_nEntryID = nEntryID;}
	int GetEntryID(){return m_nEntryID;}
	//the entry flag is ALTObjectType 
	void SetEntryFlag(int nEntryFlag){m_nEntryFlag = nEntryFlag;}
	int GetEntryFlag(){return m_nEntryFlag;}
	void SetExitID(int nExitID){m_nExitID = nExitID;}
	int GetExitID(){return m_nExitID;}
	//the exit flag is ALTObjectType
	void SetExitFlag(int nExitFlag){m_nExitFlag = nExitFlag;}
	int GetExitFlag(){return m_nExitFlag;}
	void SetArrStartTime(ElapsedTime& startTime){ m_startArrTime = startTime;}
	void SetArrEndTime(ElapsedTime& endTime){ m_endArrTime = endTime;}
	ElapsedTime* GetArrStartTime(){return &m_startArrTime;}
	ElapsedTime* GetArrEndTime(){return &m_endArrTime;}
	void SetDepStartTime(ElapsedTime& startTime) { m_startDepTime = startTime;}
	void SetDepEndTime(ElapsedTime& endTime) { m_endDepTime = endTime;}
	ElapsedTime* GetDepStartTime() { return &m_startDepTime;}
	ElapsedTime* GetDepEndTime() { return &m_endDepTime;}
	void SetFltCount(int nFlightCount){m_nFlightCount = nFlightCount;}
	int GetFltCount(){return m_nFlightCount;}
	void SetArrDistName(CString strDistName){m_strArrDistName = strDistName;}
	CString GetArrDistName(){return m_strArrDistName;}
	void SetArrDistScreenPrint(CString strScreenPrint){m_strArrDistScreenPrint = strScreenPrint;}
	CString GetArrDistScreenPrint(){return m_strArrDistScreenPrint;}

	void SetDepDistName(CString strDistName){ m_strDepDistName = strDistName;}
	CString GetDepDistName() { return m_strDepDistName;}

	void SetDepDistScreenPrint(CString strScreenPrint) { m_strDepDistScreenPrint = strScreenPrint;}
	CString GetDepDistScreenPrint() {return m_strDepDistScreenPrint;}


	void SetArrProbTypes(ProbTypes enumProbType){m_enumArrProbType = enumProbType;}
	void SetDepProbTypes(ProbTypes enumProbType){m_enumDepProbType = enumProbType;}
	ProbabilityDistribution* GetArrProbDistribution();
	ProbabilityDistribution* GetDepProbDistribution();
	void SetArrPrintDist(CString strprintDist){m_strArrprintDist = strprintDist;}
	void SetDepPrintDist(CString strprintDist){m_strDepprintDist = strprintDist;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nParentID,CString strFltType);
};

class INPUTAIRSIDE_API ItinerantFlightList : public DBElementCollection<ItinerantFlight>
{
public:
	typedef DBElementCollection<ItinerantFlight> BaseClass;
	ItinerantFlightList();
	virtual ~ItinerantFlightList();
protected:
//	virtual void InitFromDBRecordset(CADORecordset& recordset) {}
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
		
public:
	void ReadData(int nParentID);
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}
	static void ExportItinerantFlights(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportItinerantFlights(CAirsideImportFile& importFile);
private:
	CAirportDatabase* m_pAirportDB;
private:
	//export dll symbols
	void DoNotCall();
};