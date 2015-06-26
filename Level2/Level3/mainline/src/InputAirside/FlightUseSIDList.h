#pragma once
#include "..\Database\DBElementCollection_Impl.h"
#include "FlightUseSID.h"
#include "common/elaptime.h"
#include "InputAirsideAPI.h"

class CAirsideExportFile;
class CAirportDatabase;

class INPUTAIRSIDE_API CFlightUseSIDList : public DBElementCollection<CFlightUseSID>
{
public:
	typedef DBElementCollection<CFlightUseSID> BaseClass;
	CFlightUseSIDList();
	virtual ~CFlightUseSIDList(void);

public:
	void SetFromTime(ElapsedTime estFromTime);
	ElapsedTime GetFromTime(void);
	void SetToTime(ElapsedTime estToTime);
	ElapsedTime GetToTime(void);
	void GetItemByFlightTypeAndSidID(CString strFlightType, int nSidID, CFlightUseSID **ppFlightUseSID);

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
