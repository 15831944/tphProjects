#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "common/elaptime.h"
#include "InputAirsideAPI.h"

class CFlightUseSIDList;
class CAirsideExportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CLandingRunwaySIDAssignment : public DBElementCollection<CFlightUseSIDList>
{
public:
	typedef DBElementCollection<CFlightUseSIDList> BaseClass;
	CLandingRunwaySIDAssignment();
	virtual ~CLandingRunwaySIDAssignment(void);

public:
	void SetLandingRunwayID(int nLandingRunwayID);
	int GetLandingRunwayID(void);
	void SetRunwayMarkingIndex(int nRunwayMarkingIndex);
	int GetRunwayMarkingIndex(void);
	void GetItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime, CFlightUseSIDList **ppFlightUseSIDList);
	void DeleteItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime);
	BOOL IsConflictTime(CFlightUseSIDList *pFlightUseSIDList);

	//DBElementCollection
	virtual void SaveData(int nParentID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	void SetAirportDatabase(CAirportDatabase* pAirportDB){ m_pAirportDatabase = pAirportDB; }

public:
	void ExportData(CAirsideExportFile& exportFile);

private: 
	int m_LandingRunwayID;
	int m_RunwayMarkingIndex;  //0:Marking1 1:Marking2
	CAirportDatabase* m_pAirportDatabase;
};