#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "Common/FLT_CNST.H"
#include "DistanceAndTimeFromARP.h"
#include <vector>
#include "AirsideImportExportManager.h"
#include "InputAirsideAPI.h"

using namespace std;

class CAirsideImportFile;
class CAirsideExportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CArrivalETAOffset :
	public DBElementCollection<CDistanceAndTimeFromARP>
{
public:
	CArrivalETAOffset(void);
	virtual ~CArrivalETAOffset(void);

public:
	void SetFlightType(FlightConstraint& flightType);
	void SetFlightType(CString& strFlightType);
	FlightConstraint GetFlightType()const;

	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase;}
	CAirportDatabase * GetAirportDatabase(){ return  m_pAirportDatabase; }

	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	virtual void GetInsertSQL(int nProjID, CString& strSQL) const;
	virtual void GetUpdateSQL(CString& strSQL) const;
	virtual void GetDeleteSQL(CString& strSQL) const;

	//dbcollection
	virtual void GetSelectElementSQL(int nParentID,CString& strSQL);
	virtual void SaveData(int nParentID);

	void DeleteDataFromDB();

	CDistanceAndTimeFromARP* GetLargestDistanceItem()const;
	CDistanceAndTimeFromARP* GetNearestDistanceItem(const double& dist)const;

public:
	void ExportData(CAirsideExportFile& exportFile);
	void ImportData(CAirsideImportFile& importFile);

private:
	CString m_strFlightType;
	FlightConstraint m_FlightType;
	CAirportDatabase* m_pAirportDatabase;
};