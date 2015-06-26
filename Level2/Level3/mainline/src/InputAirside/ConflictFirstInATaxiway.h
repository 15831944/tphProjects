#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "ALTObject.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class INPUTAIRSIDE_API CFirstInATaxiway : public DBElement
{
public:
	CFirstInATaxiway();
	virtual ~CFirstInATaxiway();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:

	//CString m_strFltTypeA;
	//CString m_strFltTypeB;
	FlightConstraint m_fltTypeA;
	FlightConstraint m_fltTypeB;
	int m_nConflictResolutionID;
	CAirportDatabase* m_pAirportDB;
	//	int			m_nProjectID;

public:
	//void SetFltTypeA(CString strFltTypeA){m_strFltTypeA = strFltTypeA;}
	//CString GetFltTypeA(){return m_strFltTypeA;}
	FlightConstraint& GetFltTypeA() { return m_fltTypeA; }
	void SetFltTypeA(const FlightConstraint& fltTypeA) { m_fltTypeA = fltTypeA; }

	//void SetFltTypeB(CString strFltTypeB){m_strFltTypeB = strFltTypeB;}
	//CString GetFltTypeB(){return m_strFltTypeB;}
	FlightConstraint& GetFltTypeB() { return m_fltTypeB; }
	void SetFltTypeB(const FlightConstraint& fltTypeB) { m_fltTypeB = fltTypeB; }

	void SetConflictResolutionID(int nConflictResolutionID){m_nConflictResolutionID = nConflictResolutionID;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nParentID,CString strFltTypeA,CString strFltTypeB);
};

//////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API CFirstInATaxiwayList : public DBElementCollection<CFirstInATaxiway>
{
public:
	typedef DBElementCollection<CFirstInATaxiway> BaseClass;
	CFirstInATaxiwayList();
	virtual ~CFirstInATaxiwayList();
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

public:
	void ReadData(int nParentID);
	void SetConflictResolutionID(int nConflictResolutionID){m_nConflictResolutionID = nConflictResolutionID;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportCFirstInATaxiways(CAirsideExportFile& exportFile);
	void ImportCFirstInATaxiways(CAirsideImportFile& importFile);
private:
	int m_nConflictResolutionID;
	CAirportDatabase* m_pAirportDB;

	//export dll symbols
	void DoNotCall();

};