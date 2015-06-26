#include "StdAfx.h"
#include ".\airsidegroundpropdlg.h"
#include "3DGridOptionsDlg.h"

CAirsideGroundPropDlg::CAirsideGroundPropDlg(int nAirportID,CAirsideGround* pFloor,CTermPlanDoc *pDoc, CWnd* pParent)
:CFloorPropertiesDlg(pFloor,pDoc,pParent)
{
	if(pFloor->LoadCAD())
		pFloor->LoadCADLayers();
	m_nAirportID = nAirportID;
}

CAirsideGroundPropDlg::~CAirsideGroundPropDlg(void)
{
}
void CAirsideGroundPropDlg::OnGridOptions()
{
	int nLevelID = ((CAirsideGround *)m_pFloor)->getID();
	((CAirsideGround *)m_pFloor)->ReadGridInfo(nLevelID);
	C3DGridOptionsDlg dlg(*(m_pFloor->GetGrid()),m_pDoc);
	if(dlg.DoModal() == IDOK) 
	{
		dlg.UpdateGridData(m_pFloor->GetGrid());
		((CAirsideGround *)m_pFloor)->UpdateGridInfo(nLevelID);
		((CAirsideGround *)m_pFloor)->UpdateCADInfo(nLevelID);
		
	}

}
void CAirsideGroundPropDlg::OnOK()
{
	if(!SaveData())
        return;
	CADODatabase::BeginTransaction();
	try
	{	
		((CAirsideGround *)m_pFloor)->SaveData(m_nAirportID);	
		int nLevelID = ((CAirsideGround *)m_pFloor)->getID();
		((CAirsideGround *)m_pFloor)->UpdateCADInfo(nLevelID);
		((CAirsideGround *)m_pFloor)->DeleteFloorLayerFromDB(nLevelID) ;
		((CAirsideGround *)m_pFloor)->SaveFloorLayerToDB(nLevelID) ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	CADODatabase::CommitTransaction() ;
	CDialog::OnOK();
}

void CAirsideGroundPropDlg::OnCancel()
{
	

	CDialog::OnCancel();
}
