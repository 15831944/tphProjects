#include "StdAfx.h"
#include ".\basefloor.h"
#include "Common\ARCGUID.h"
static double INSANE_MAX=2000000.0;
static double INSANE_MIN=1000.0;

CVisibleRegion::CVisibleRegion(const CString& _sName) : sName(_sName),m_ID(-1)
{
}

BOOL CBaseFloor::IsExtentsSane() const
{
	ARCVector2 v((m_vMapExtentsMax-m_vMapExtentsMin)*m_CadFile.dScale);
	if(v[VX] < INSANE_MIN || v[VX] > INSANE_MAX || v[VY] < INSANE_MIN || v[VY] > INSANE_MAX)
		return FALSE;
	return TRUE;
}

void CBaseFloor::DeleteAllVisibleRegions()
{
	for(size_t i=0; i < m_vVisibleRegions.size(); i++) {
		delete m_vVisibleRegions[i];
	}	
	m_vVisibleRegions.clear();	
}
void CBaseFloor::DeleteAllInVisibleRegions()
{
	for(size_t i=0; i < m_vInVisibleRegions.size(); i++) {
		delete m_vInVisibleRegions[i];
	}
	m_vInVisibleRegions.clear();	
}


void CVisibleRegion::ReadDataFromDB(int _paeentID,std::vector<CVisibleRegion*>* RegionContainer,BOOL Isvisible)
{
	if(RegionContainer == NULL)
		return ;
	CString SQL ;
	long count = 0 ;
	SQL.Format(_T("SELECT * FROM TB_DECK_REGION WHERE DECKID = %d AND ISVISIBLE = %d"),_paeentID,Isvisible) ;
	CADORecordset dataset ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataset,DATABASESOURCE_TYPE_ACCESS) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}
	GetDataSet(dataset,RegionContainer) ;
}
void CVisibleRegion::WriteDataToDB(int _paeentID,std::vector<CVisibleRegion*>* RegionContainer,BOOL Isvisible)
{

	if(RegionContainer == NULL)
		return ;
	int count = (int)RegionContainer->size() ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM TB_DECK_REGION WHERE DECKID = %d AND ISVISIBLE = %d"),_paeentID,Isvisible) ;
	try
	{
		CADODatabase::BeginTransaction() ;
		CADODatabase::ExecuteSQLStatement(SQL,DATABASESOURCE_TYPE_ACCESS) ;	
		for (int i = 0 ; i < count ;i++)
		{
			CVisibleRegion::WriteData(RegionContainer->at(i),_paeentID,Isvisible) ;
		}
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		CADODatabase::RollBackTransation() ;
	}

}
void CVisibleRegion::GetDataSet(CADORecordset& _set , std::vector<CVisibleRegion*>* RegionContainer)
{
	CString sName ;
	int id = -1;
	CVisibleRegion*  Pregion = NULL  ;
	while(!_set.IsEOF())
	{
		_set.GetFieldValue(_T("ID"),id) ;
		_set.GetFieldValue(_T("REGIONNAME"),sName) ;
		Pregion = new CVisibleRegion(CString()) ;
		Pregion->m_ID = id ;
		Pregion->sName = sName ;
		Pregion->ReadPathFromDB() ;
		RegionContainer->push_back(Pregion);
		_set.MoveNextData() ;
	}
}
void CVisibleRegion::WriteData(CVisibleRegion* _region,int _parentID,BOOL Isvisible)
{
	CString SQL ;
	SQL.Format(_T("INSERT INTO TB_DECK_REGION\
				  ( REGIONNAME, ISVISIBLE, DECKID)\
				  VALUES ('%s',%d,%d)"),_region->sName,Isvisible,_parentID) ;
	_region->m_ID = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
	_region->WritePathToDB() ;
}
void CVisibleRegion::ReadPathFromDB()
{
	Pollygon::ReadDataFromDB(&polygon,m_ID) ;
}
void CVisibleRegion::WritePathToDB()
{
	Pollygon::WriteDataToDB(&polygon,m_ID) ;
}


void CBaseFloor::MapFileName(const CString& sFileName) 
{
	if(sFileName.CompareNoCase(m_CadFile.sFileName) != 0) {
		m_CadFile.sFileName = sFileName;
		m_bIsMapValid = TRUE;		
		m_bIsCADLoaded = FALSE;
		m_bIsCADLayersLoaded = FALSE;
		m_bIsCADProcessed = FALSE;
		m_bUseLayerInfoFiles = FALSE;
	}
}

void CBaseFloor::MapPathName(const CString& sPathName) 
{
	if(sPathName.CompareNoCase(m_CadFile.sPathName) != 0) {
		m_CadFile.sPathName = sPathName;
		m_bIsMapValid = TRUE;		
		m_bIsCADLoaded = FALSE;
		m_bIsCADLayersLoaded = FALSE;
		m_bIsCADProcessed = FALSE;
	}
}

CBaseFloor::CBaseFloor()
{
	m_dVisualAltitude = 0;
	m_dRealAltitude = 0;
	m_dThickness = 0;	//floor thickness in arcterm units (cm)
	m_bIsActive = FALSE;		//floor is the active floor

	m_bIsVisible = TRUE;		//floor is visible
	m_bIsOpaque = FALSE;
	m_bUseMarker = FALSE;
	m_vMarkerLoc = ARCVector2(0.0,0.0);
	use_AlignLine = FALSE;


	m_bIsShowMap = FALSE;
	m_bVRDirtyFlag = FALSE;
	m_bIsTexValid = FALSE;	
	m_CadFile.sFileName = _T("No map selected");
	m_CadFile.sPathName = _T("");
	m_CadFile.dScale = 100.0;
	m_CadFile.vOffset = ARCVector2(0.0,0.0);
	m_CadFile.dRotation = 0.0;	

	m_vOffset= ARCVector2(0,0);
	m_bUseVisibleRegions = FALSE;

	m_sUUID = ARCGUID::Generate(_T("Floor"));
}




BOOL CBaseFloor::LoadCAD()
{
	return CCADFileContent::LoadCAD(m_CadFile.sFileName,m_CadFile.sPathName);
}