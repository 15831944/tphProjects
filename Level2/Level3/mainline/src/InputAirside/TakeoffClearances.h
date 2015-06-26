#pragma once

#include "TakeoffClearanceItem.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API TakeoffClearances
{
public:
	TakeoffClearances(void);
	TakeoffClearances(int nProjID);
	virtual ~TakeoffClearances(void);

	void ReadData();
	void SaveData();
	TakeoffClearanceItem* GetTakeoffClearanceItem(TakeoffClearanceType emType);
	
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ImportTakeoffClearances(CAirsideImportFile& importFile);
	static void ExportTakeoffClearances(CAirsideExportFile& exportFile);
private:
	int m_nProjID;

	TakeoffBehindTakeoffSameRunwayOrCloseParalled * m_pTBTSameRunway;
	TakeoffBehindTakeoffIntersectingConvergngRunway * m_pTBTIntersect;
	TakeoffBehindLandingSameRunway* m_pTBLSameRunway;
	TakeoffBehindLandingIntersectingConvergingRunway * m_pTBLIntersect;

};
