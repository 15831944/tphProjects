

#ifndef AIRSIDE_INPUT_H
#define AIRSIDE_INPUT_H

#include "common\template.h"
#include "engine\proclist.h"
#include "main\floors.h"
#include "AreasPortals.h"
#include "main\DefaultDisplayProperties.h"
#include "main\Cameras.h"
#include "main\DisplayPropOverrides.h"
#include "main\Floor2.h"
#include "Common\strdict.h"
#include "../Engine/ContourTree.h"
//2005-9-2 Ben
#include "Structurelist.h"
#include "WallShapeList.h"
//
//#include "AirsideAircraft.h"

#include "Engine/TaxiwayTrafficGraph.h"

#include"AirsideNodeList.h"
class CAirsideNodeList;

class CAirportInfo;
class CAirportDatabase;
class CPlacement;

class CAirsideInput
{
public:
	CAirsideInput(void);
	CAirsideInput(StringDictionary* pStrDict, CAirportDatabase* pAirportDB, FlightSchedule* pFlightSchedule);
	~CAirsideInput(void);

public:
	ProcessorList* GetProcessorList()const { return m_pProcessorList; };
	
	
	//airport name array
	std::vector<CString> m_csAirportNameList;
	void GetAirprotInfo(const CString& _pProjPath);
	void RenameAirportName(const CString& _pProjPath,CString strOldName,CString strNewName,bool _bUndo);
	void SetLL(const CString& _pProjPath,CString strLong,CString strLat,bool _bUndo);
	void GetLL(const CString& _pProjPath,CString &strLong,CString &strLat);
	CAirportInfo *m_pAirportInfo;
	CContourTree *pContourTree;


	CPlacement* GetPlacementPtr() const { return m_pPlacement;	}
	void InitAirsideInputData(InputTerminal* pTerm, const CString& _pProjPath);
	void SaveAirsideInputData(const CString& _pProjPath, bool _bUndo);
	void CleanInputData();
//	CAirsideAircraftCat* GetAACatManager() { return &m_aaCatManager; }
//	CAirsideAircraftCat& GetAirsideAircraftCat() { return m_aaCatManager; }
	FlightSchedule* GetFlightSchedule() const{ return m_pFlightSchedule; }
//	CAirportInfo* GetAirportInfo(){return m_pAirportInfo;}

	//Taxiway Traffic Graph,generate intersections and directions
	//when auto snaped ,need rebuild graph to update intersections and directions
	//void RebuildTaxiwayTrafficGraph(){	return m_TaxiwayTraffic.RebuildGraph(); }
	bool AddTaxiwayProc(TaxiwayProc* pProc){	return m_TaxiwayTraffic.AddTaxiwayProc(pProc); }
	bool DeleteTaxiwayProc(TaxiwayProc* pProc){	return m_TaxiwayTraffic.DeleteTaxiwayProc(pProc); }
	bool UpdateTaxiwayProc(TaxiwayProc* pProc){	return m_TaxiwayTraffic.UpdateTaxiwayProc(pProc); }

	void AutoSnapTraffic();
protected:
	CFloors *m_pfloors;
	CPortals m_portals;
	CDefaultDisplayProperties m_defDispProp;
	CPlacement* m_pPlacement;
	ProcessorList* m_pProcessorList;
	CCameras *m_pcameras;
	CDisplayPropOverrides m_displayoverrides;
	//2005-9-2 Ben
	CStructureList *m_pStructureList; 
	WallShapeList * m_pWallList;
	//2005-09-15 by frank 
	CAirportDatabase* m_pAirportDB;
//	CAirsideAircraftCat m_aaCatManager;
	FlightSchedule *m_pFlightSchedule;
	
	CAirsideNodeList *m_pAirsideNodeList;

//	ASLayout * m_pasLayout;
	TaxiwayTrafficGraph m_TaxiwayTraffic;

public:
	CStructureList * getpStructurelist(){return m_pStructureList;}
	WallShapeList *getpWallShapeList(){return m_pWallList;}
	CAirsideNodeList *getAirsideNodeList(){return m_pAirsideNodeList;}

	CFloors& GetFloors() { return *m_pfloors; }
	CCameras& GetCameras() { return *m_pcameras; }
	CPortals& GetPortals() { return m_portals;}
	CDefaultDisplayProperties& GetDispProp() { return m_defDispProp;}
	CDisplayPropOverrides& GetDispPropOverides(){return m_displayoverrides;}
	
//	ASLayout* GetAirsideLayout(){return m_pasLayout;}
public:
	//bool For_OnNewDocument( PROJECTINFO& ProjInfo );
	bool For_InitializeDefault(PROJECTINFO& ProjInfo);
	bool For_InitializeProject(PROJECTINFO& ProjInfo);


};    

#endif
