#pragma once

#include "../Database/\DBElement.h"
#include "../Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirsideImportFile;

class INPUTAIRSIDE_API CFlightTypeRestriction: public DBElement
{
public:
	CFlightTypeRestriction(void);
	~CFlightTypeRestriction(void);

private:
	int m_nProjID;

	FlightConstraint m_fltType;
	int m_nTaxiwayID;
	int	m_nFromPos;
	int	m_nToPos;

	CAirportDatabase* m_pAirportDatabase;
	//CString m_strFltType;

public:

	void setProjID(int nProjID);
	int  GetProjID() const;

	void SetFltType(const FlightConstraint& FltType);
	FlightConstraint& GetFltType() { return m_fltType; }

	void SetTaxiwayID(int nTaxiwayID);	
	int  GetTaxiwayID() const;

	void SetFromPos(int nFromPos);
	int  GetFromPos() const;

	void SetToPos(int nToPos);
	int  GetToPos() const;

	void SetAirportDatabase(CAirportDatabase* pAirportDB);
	void setFlightType(const CString& strFltType);

public:
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nParentID,CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;
	virtual void GetSelectSQL(int nID,CString& strSQL)const;
	virtual void ExportData(CAirsideExportFile& exportFile);
	virtual void ImportData(CAirsideImportFile& importFile);
};