#pragma once
#include "..\Database\ADODatabase.h"
#include "..\Database\DBElementCollection_Impl.h"
#include "../Common/FLT_CNST.H"
#include "../common/TimeRange.h"
#include "common/elaptime.h"
#include "../common/ProbabilityDistribution.h"
#include "InputAirsideAPI.h"

class CAirportDatabase;
class CADORecordset;
class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API DepartureSlotItem : public DBElement
{
public:
	DepartureSlotItem(void);
	virtual ~DepartureSlotItem(void);
public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);

	FlightConstraint GetFlightConstraint();
	void SetFltType(FlightConstraint fltCons);
    void setAirportDB(CAirportDatabase *pAirportDB){m_pAirportDB = pAirportDB;}

	void SetProjID(int nProjID){m_nProjID = nProjID;}
	int	 getProjID(){ return m_nProjID; }

	int GetSID(void);
	void SetSID(int nSID);
	CString GetSIDName(void);

	ProbabilityDistribution* GetFirstSlotDelayProDis(void);
	CString GetFirstSlotDelayProDisName(void);
	void SetFirstSlotDelayProDisName(CString strFirstSlotDelayProDisName);
	void SetFirstSlotDelayProDisType(ProbTypes  emFirstSlotDisType);
	void SetFirstSlotDelayPrintDis(CString strFirstSlotPrintDis);

	ProbabilityDistribution* GetInterSlotDelayProDis(void);
	CString GetInterSlotDelayProDisName(void);
	void SetInterSlotDelayProDisName(CString strInterSlotDelayProDisName);
	void SetInterSlotDelayProDisType(ProbTypes emInterSlotDisType);
	void SetInterSlotDelayPrintDis(CString strInterSlotPrintDis);
public:
	  void ExportData(CAirsideExportFile& exportFile);
	  void ImportData(CAirsideImportFile& importFile);
protected:
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
protected:
	int					m_nProjID;
	FlightConstraint	m_fltType;

	//SID
	int m_nSID;

	//Time Range
	ElapsedTime			m_FromTime;  
	ElapsedTime			m_ToTime; 

	//First Slot Delay
	CString		m_strFirstSlotDistName;
	ProbTypes m_enumFirstSlotProbType; 
	CString m_strFirstSlotPrintDist;
	ProbabilityDistribution* m_pFirstSlotDistribution;
	
	//Inter Slot Delay
	CString		m_strInterSlotDistName;
	ProbTypes m_enumInterSlotProbType; 
	CString m_strInterSlotPrintDist;
	ProbabilityDistribution* m_pInterSlotDistribution;

	bool m_bImport;//true,import;false,not;
	CString m_strImportFltType;//import,so save FlightConstraint data in this string;

public:
	void SetFromTime(ElapsedTime estFromTime);
	ElapsedTime GetFromTime(void);
	void SetToTime(ElapsedTime estToTime);
	ElapsedTime GetToTime(void);
	//CString m_strFlightType;
	CAirportDatabase	*m_pAirportDB;	
}; 

class INPUTAIRSIDE_API DepartureSlotSepcifications : public DBElementCollection<DepartureSlotItem>
{
public:
	typedef DBElementCollection<DepartureSlotItem> BaseClass;
	DepartureSlotSepcifications(CAirportDatabase* pAirportDB);
	virtual ~DepartureSlotSepcifications(void);

public:
	virtual void ReadData(int nParentID);
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;
	static void ExportDepartureSlotSepc(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportDepartureSlotSepc(CAirsideImportFile& importFile);
	virtual	void ImportData(CAirsideImportFile& importFile);
	virtual void ImportData(int nCount,CAirsideImportFile& importFile);

	CAirportDatabase	*m_pAirportDB;
	void SetAirportDB(CAirportDatabase * pAirportDB){ m_pAirportDB = pAirportDB; }
	//protected:	
private:
	//export dll symbols
	void DoNotCall();
};