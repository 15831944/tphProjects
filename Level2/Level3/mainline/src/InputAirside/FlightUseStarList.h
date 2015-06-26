#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "FlightUseStar.h"
#include "common/elaptime.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CFlightUseStarList : public DBElementCollection<CFlightUseStar>
{
public:
	typedef DBElementCollection<CFlightUseStar> BaseClass;
	CFlightUseStarList();
	virtual ~CFlightUseStarList(void);

public:
	void SetFromTime(ElapsedTime estFromTime);
	ElapsedTime GetFromTime(void);
	void SetToTime(ElapsedTime estToTime);
	ElapsedTime GetToTime(void);
	void GetItemByFlightTypeAndStarID(CString strFlightType, int nStarID, CFlightUseStar **ppFlightUseStar);

	void Save(int nParentID, int nRunwayID, int nMarking);
	//DBElement
	virtual void InitFromDBRecordset(CADORecordset& recordset);
	
	void SetAirportDatabase(CAirportDatabase* pAirportDatabase){ m_pAirportDatabase = pAirportDatabase;}

	void SortFlightType();

public:
	void ExportDataEx(CAirsideExportFile& exportFile, int nRunwayID, int nMarkingID);

private:
	ElapsedTime m_FromTime;  
	ElapsedTime m_ToTime;
	CAirportDatabase* m_pAirportDatabase;
};
