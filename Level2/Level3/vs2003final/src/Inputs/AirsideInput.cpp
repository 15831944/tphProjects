#include "stdafx.h"
#include "main\Placement.h"
#include "airsideinput.h"
#include "airportInfo.h"
#include "In_term.h"
#include "../Engine/RunwayProc.h"
#include "../Engine/TaxiwayProc.h"
  

CAirsideInput::CAirsideInput(void)
{
	m_pProcessorList = NULL;
	m_pAirportInfo=new CAirportInfo();
	m_pPlacement = new CPlacement(AirsidePlacementsFile);
	m_pfloors = new CFloors(AirsideFloorsFile);

	pContourTree = new CContourTree() ;

	m_pStructureList=new CStructureList(AirsideStructureFile);
	m_pWallList= new WallShapeList (AirsideWallShapeFile);

	m_pAirportDB = NULL;
	m_pFlightSchedule = NULL;

	m_pcameras = new CCameras(AirsideCamerasFile);

	m_pAirsideNodeList = new CAirsideNodeList();
//	m_pasLayout = new ASLayout();
}

CAirsideInput::CAirsideInput(StringDictionary* pStrDict, CAirportDatabase* pAirportDB, FlightSchedule* pFlightSchedule)
{
	m_pProcessorList = new ProcessorList(pStrDict, AirsideProcessorLayoutFile);
	m_pAirportInfo=new CAirportInfo();
	m_pPlacement = new CPlacement(AirsidePlacementsFile);
	m_pfloors = new CFloors(AirsideFloorsFile);

	pContourTree = new CContourTree();	

	m_pStructureList=new CStructureList(AirsideStructureFile);
	m_pWallList= new WallShapeList (AirsideWallShapeFile);

	m_pAirportDB = pAirportDB;
	m_pFlightSchedule = pFlightSchedule;

	m_pcameras = new CCameras(AirsideCamerasFile);

	m_pAirsideNodeList = new CAirsideNodeList();
//	m_pasLayout = new ASLayout();
}

CAirsideInput::~CAirsideInput(void)
{	
	if(m_pPlacement)
	{
		delete m_pPlacement;
		m_pPlacement = NULL;
	}
	if(m_pProcessorList)
	{
		delete m_pProcessorList;
		m_pProcessorList = NULL;
	}
	if(m_pfloors)
	{
		delete m_pfloors;
		m_pfloors = NULL;
	}
	if( m_pcameras )
	{
		delete m_pcameras;
		m_pcameras = NULL;
	}
	if(	m_pAirsideNodeList )
	{
		delete m_pAirsideNodeList;
		m_pAirsideNodeList = NULL;
	}

	//if(m_pasLayout)
	//{
	//	delete m_pasLayout;
	//	m_pasLayout =NULL;
	//}
}

void CAirsideInput::InitAirsideInputData(InputTerminal* pTerm, const CString& _pProjPath)
{
	m_pProcessorList->SetInputTerminal( pTerm );
	m_pProcessorList->loadDataSet( _pProjPath );
	m_pProcessorList->RefreshAllStationLocatonData();
	if( m_pProcessorList->DidOffset() )
	{
		m_pProcessorList->saveDataSet( _pProjPath, false );
	}
	m_pProcessorList->addStartAndEndProcessors();
	m_pProcessorList->addFromAndToGateProcessors();
	m_pProcessorList->setMiscData( *(pTerm->miscData) );

	pContourTree->loadDataSet(_pProjPath);

//	m_pasLayout->SetProcList(m_pProcessorList);
//	m_aaCatManager.loadDataSet(_pProjPath);

	m_TaxiwayTraffic.Initialize((Terminal*)pTerm, _pProjPath);
	AutoSnapTraffic();
}

void CAirsideInput::SaveAirsideInputData(const CString& _pProjPath, bool _bUndo)
{
	pContourTree->saveDataSet(_pProjPath,_bUndo);
	m_pProcessorList->saveDataSet(_pProjPath, _bUndo);
//	m_aaCatManager.saveDataSet(_pProjPath, _bUndo);
}

void CAirsideInput::CleanInputData()
{
	m_pProcessorList->CleanData();
}

//get the project airport name list
void CAirsideInput::GetAirprotInfo(const CString& _pProjPath)
{
	int i=0;
	while (true)
	{
		if(m_pAirportInfo->loadDataSet(_pProjPath,2,i )==true)
		{
			m_csAirportNameList.push_back(m_pAirportInfo->m_csAirportName);
		}
		else
		{
			if (m_csAirportNameList.size()<1)
			{
				m_pAirportInfo->saveDataSet(_pProjPath,0,"Airport1",TRUE);
				m_csAirportNameList.push_back("Airport1");
			}
			break;
		}
		i++;
	}
}
void CAirsideInput::RenameAirportName(const CString &_pProjPath,CString strOldName,CString strNewName,bool _bUndo)
{
	for (int i=0;i<(int)m_csAirportNameList.size();i++)
	{
		if(strcmpi(m_csAirportNameList[i],strOldName)==0)
		{
			m_pAirportInfo->m_csAirportName=strNewName;
			m_pAirportInfo->saveDataSet(_pProjPath,i,strNewName,false);
			m_csAirportNameList[i]=strNewName;
		}
	}
}

void CAirsideInput::SetLL(const CString& _pProjPath,CString strLong,CString strLat,bool _bUndo)
{
	m_pAirportInfo->m_latitude->SetValue(strLat);
	m_pAirportInfo->m_longtitude->SetValue(strLong);
	m_pAirportInfo->saveDataSet(_pProjPath,0,"",false);

}
void CAirsideInput::GetLL(const CString& _pProjPath,CString &strLong,CString &strLat)
{
	m_pAirportInfo->loadDataSet(_pProjPath,0,0);
	m_pAirportInfo->m_longtitude->GetValue(strLong);
	m_pAirportInfo->m_latitude->GetValue(strLat);


}
/*
bool CAirsideInput::For_OnNewDocument( PROJECTINFO& ProjInfo )
{
	try
	{
		m_portals.loadDataSet( ProjInfo.path );
		m_pfloors->loadDataSet( ProjInfo.path );
		m_defDispProp.loadDataSet( ProjInfo.path );
		m_pcameras->loadDataSet(ProjInfo.path);
		m_displayoverrides.loadDataSet(ProjInfo.path);
		
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;
		return FALSE;
	}

	return true;
}
*/

bool CAirsideInput::For_InitializeDefault( PROJECTINFO& ProjInfo)
{
	CFloor2* pDefaultFloor = new CFloor2(0,  "AirSide Ground");
	pDefaultFloor->GetGrid()->bVisibility = FALSE;
	pDefaultFloor->GetGrid()->dSizeX = 5000.0 * 100.0;
	pDefaultFloor->GetGrid()->dSizeY = 5000.0 * 100.0;
	pDefaultFloor->GetGrid()->dLinesEvery = 100.0 * 100.0;
	m_pfloors->AddFloor(pDefaultFloor);
//	m_pfloors->saveDataSet(ProjInfo.path, false);
	pDefaultFloor->IsActive(TRUE);
	return true;
}

bool CAirsideInput::For_InitializeProject(PROJECTINFO& ProjInfo)
{
	if(m_pfloors->GetCount() == 0)
	{
			CFloor2* pFloor = new CFloor2( 0, "AirSide Ground" );
			
			m_pfloors->AddFloor(pFloor);
			
	}	
//	m_pfloors->saveDataSet(ProjInfo.path, false);
	
	CFloor2* pFloor = m_pfloors->GetFloor2(0);
	pFloor->IsActive(TRUE);
	if(pFloor->LoadCAD())
		pFloor->LoadCADLayers();

	return true;

}

void CAirsideInput::AutoSnapTraffic()
{
	if(!m_pProcessorList)return ;
	for(int i=0;i<m_pProcessorList->getProcessorCount();i++){
		Processor * pProc = m_pProcessorList->getProcessor(i);
		if(pProc->getProcessorType() == RunwayProcessor ){
			((RunwayProc*)pProc)->autoSnap(m_pProcessorList);
		}
		if(pProc->getProcessorType() == TaxiwayProcessor){
			((TaxiwayProc*)pProc)->autoSnap(m_pProcessorList);
		}
	}
}


