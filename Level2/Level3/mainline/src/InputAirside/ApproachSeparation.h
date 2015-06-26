#pragma once
#include "ApproachSeparationItem.h"
#include "ApproachSeparationType.h"


#include "LandingBehindLandingSameRunway.h"
#include "LandingBehindLandingIntersectionRuway.h"
#include "LandingBehindTakeoff.h"
#include "../common/UnitsManager.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API ApproachSeparation
{
public:
//	typedef std::vector<ApproachSeparationItem*> ApproachSeparationItemVector;
//	typedef std::vector<ApproachSeparationItemVector*>::iterator ApproachSeparationItemIter;

public:
	ApproachSeparation();
	ApproachSeparation(int nProjID);
	~ApproachSeparation(void);

	void ReadData();
	void SaveData();
	ApproachSeparationItem* GetApproachSeparationItem(ApproachSeparationType emType);

	void SetProjectID(int nProjID);

	void ConvertToUseUnit(CUnitsManager * pGlobalUnits);
	void ConvertToDBUnit(CUnitsManager * pGlobalUnits);

private:
	int m_nProjID;
	//ApproachSeparationItemVector m_vectApproachSeparation;
	ApproachSeparationItem* m_pLandingBehindLandingSameRunway;
	ApproachSeparationItem* m_pLandingBehindLandingIntersectionRuway_LAHSO;
	ApproachSeparationItem* m_pLandingBehindLandingIntersectionRuway_NON_LAHSO;
	ApproachSeparationItem* m_pLandingBehindTakeoffSameRunway;
	ApproachSeparationItem* m_pLandingBehindTakeoffIntersectionRunway;
public:
	void ImportData(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ImportApproachSeparation(CAirsideImportFile& importFile);
	static void ExportApproachSeparation(CAirsideExportFile& exportFile);
};
