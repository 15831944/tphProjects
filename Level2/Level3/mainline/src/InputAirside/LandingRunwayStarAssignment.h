#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "common/elaptime.h"
#include "InputAirsideAPI.h"

class CFlightUseStarList;
class CAirsideExportFile;
class CAirsideImportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CLandingRunwayStarAssignment : public DBElementCollection<CFlightUseStarList>
{
public:
	typedef DBElementCollection<CFlightUseStarList> BaseClass;
	CLandingRunwayStarAssignment();
	virtual ~CLandingRunwayStarAssignment(void);

public:
	void SetLandingRunwayID(int nLandingRunwayID);
	int GetLandingRunwayID(void);
	void SetRunwayMarkingIndex(int nRunwayMarkingIndex);
	int GetRunwayMarkingIndex(void);
	void GetItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime, CFlightUseStarList **ppFlightUseStarList);
	void DeleteItemByTime(ElapsedTime estFromTime, ElapsedTime estToTime);
	BOOL IsConflictTime(CFlightUseStarList *pFlightUseStarList);

	void SetAirportDatabase(CAirportDatabase* pAirportDB){ m_pAirportDatabase = pAirportDB;}
	//DBElementCollection
	virtual void SaveData(int nParentID);
	virtual void DeleteItemFromIndex(size_t nIndex);
	virtual void DeleteData();
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);

public:
	void ExportData(CAirsideExportFile& exportFile);

private: 
	int m_LandingRunwayID;
	int m_RunwayMarkingIndex;  //1:Marking1 2:Marking2
	CAirportDatabase* m_pAirportDatabase;
};