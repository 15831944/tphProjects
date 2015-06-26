#pragma once
#include "../Database/dbelement.h"
#include "common/elaptime.h"
#include "Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CFlightUseSID :
	public DBElement
{
public:
	CFlightUseSID();
	virtual ~CFlightUseSID(void);

public:
	void SetFlightType(FlightConstraint& flightType);
	FlightConstraint GetFlightType();
	CString GetFlightTypeString(){return m_strFlightType;}
	void SetSidID(int nSidID);
	int GetSidID(void);

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase;}

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const;
	virtual void GetUpdateSQL(ElapsedTime estFromTime, ElapsedTime estToTime, CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

public:
	void ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID, int nFromTime, int nToTime);
	void ImportData(CAirsideImportFile& importFile);

public:
	void Save(int nParentID, int nRunwayID, int nMarking, ElapsedTime estFromTime, ElapsedTime estToTime);

private:
	CString m_strFlightType;
	FlightConstraint m_FlightType;
	CAirportDatabase* m_pAirportDatabase; 
	int     m_SidID;
};