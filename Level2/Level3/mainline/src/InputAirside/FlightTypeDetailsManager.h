#pragma once
#include "FlightTypeDetailsItem.h"
#include <vector>
#include "InputAirsideAPI.h"

class CAirportDatabase;
class CAirsideExportFile;
class CAirsideImportFile;
class INPUTAIRSIDE_API FlightTypeDetailsManager
{
public:
	typedef std::vector< FlightTypeDetailsItem* > FlightTypeDetailsItemVector;
	typedef FlightTypeDetailsItemVector::iterator FlightTypeDetailsItemIter;
public:
	FlightTypeDetailsManager(CAirportDatabase* pAirportDB, int nProjID);
	virtual ~FlightTypeDetailsManager(void);

public:
	void ReadData();
	void SaveData();
	void AddItem(FlightTypeDetailsItem* pItem);
	void DeleteItem(FlightTypeDetailsItem* pItem);
	void RemoveAll();
	int  GetCount(){	return (int)m_vectFltTypeDetailsItem.size(); }
	FlightTypeDetailsItem* GetItem(int nIndex);

	BOOL FlightConstraintHasDefine(FlightConstraint& cons);


	FlightTypeDetailsItem* GetFlightDetailItem(const AirsideFlightDescStruct& fltDesc );


	static void ExportFlightTypeDetailsManager(CAirsideExportFile& exportFile, CAirportDatabase* pAirportDB);
	static void ImportFlightTypeDetailsManager(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);


protected:
	CAirportDatabase					*m_pAirportDB;
	int									m_nProjID;

	FlightTypeDetailsItemVector			m_vectFltTypeDetailsItem;
	FlightTypeDetailsItemVector			m_vectNeedDelete;
};
