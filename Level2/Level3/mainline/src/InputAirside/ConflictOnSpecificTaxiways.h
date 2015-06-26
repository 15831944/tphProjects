#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "ALTObject.h"
#include "..\Common\Flt_cnst.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;
class INPUTAIRSIDE_API COnSpecificTaxiways : public DBElement
{
public:
	COnSpecificTaxiways();
	virtual ~COnSpecificTaxiways();

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

private:

	//CString m_strFltType;
	FlightConstraint  m_fltType;
	int m_nTaxiwayID;
	int m_nConflictResolutionID;
	CAirportDatabase* m_pAirportDB;
	//	int			m_nProjectID;

public:
	//void SetFltType(CString strFltType){m_strFltType = strFltType;}
	//CString GetFltType(){return m_strFltType;}
	FlightConstraint& GetFltType() { return m_fltType; }
	void SetFltType(const FlightConstraint& fltType) { m_fltType = fltType; }

	void SetTaxiwayID(int nTaxiwayID){m_nTaxiwayID = nTaxiwayID;}
	int GetTaxiwayID(){return m_nTaxiwayID;}

	void SetConflictResolutionID(int nConflictResolutionID){m_nConflictResolutionID = nConflictResolutionID;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);
	void ImportSaveData(int nParentID,CString strFltType);
};

class INPUTAIRSIDE_API COnSpecificTaxiwaysList : public DBElementCollection<COnSpecificTaxiways>
{
public:
	typedef DBElementCollection<COnSpecificTaxiways> BaseClass;
	COnSpecificTaxiwaysList();
	virtual ~COnSpecificTaxiwaysList();
protected:
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL) const;

public:
	void ReadData(int nParentID);
	void SetConflictResolutionID(int nConflictResolutionID){m_nConflictResolutionID = nConflictResolutionID;}
	void SetAirportDB(CAirportDatabase* pAirportDB){m_pAirportDB = pAirportDB;}

	void ExportCOnSpecificTaxiwayss(CAirsideExportFile& exportFile);
	void ImportCOnSpecificTaxiwayss(CAirsideImportFile& importFile);
private:
	int m_nConflictResolutionID;
	CAirportDatabase* m_pAirportDB;

	void DoNotCall();

};