// SimEngSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "SimEngSettingDlg.h"
#include "..\inputs\simparameter.h"
#include "TermPlanDoc.h"
#include "reports\SimAutoReportPara.h"
#include "PassengerTypeDialog.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "..\Inputs\PipeDataSet.h"
#include "../Inputs/schedule.h"
#include ".\simengsettingdlg.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/altobjectgroup.h"
#include "../InputAirside/runway.h"
#include "../InputAirside/taxiway.h"
#include "../InputAirside/stand.h"
#include "DlgTimeRange.h"
#include "DlgSelectALTObject.h"
#include "../AirsideGUI/DlgStandFamily.h"
#include "../InputOnBoard/OnboardSimSetting.h"
#include "../Database/ADODatabase.h"
#include <Inputs/PROCIDList.h>
#include "../Landside/LandsideSimSetting.h"
#include "../Landside/LandsideLayoutObject.h"
#include "DlgCurbsideSelect.h"
#include "landside\InputLandside.h"
#include "inputs\ProcToResource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString strUpper = "Upper";
const CString strLower = "Lower";
/////////////////////////////////////////////////////////////////////////////
// CSimEngSettingDlg dialog


CSimEngSettingDlg::CSimEngSettingDlg( CSimParameter* _pSimParam, CWnd* pParent)
	: CDialog(CSimEngSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimEngSettingDlg)
	m_stSimName = _T("");
	//}}AFX_DATA_INIT
	m_pParent=pParent;
	m_bShowSimName = false;
	m_bAutoCreateProc = FALSE;
	m_pSimTimeRangeDlg = NULL;
	m_pSimParam = _pSimParam;
	m_pAirsideSimSettingClosure = NULL;
	m_pOnboardSimSetting = NULL;
	m_pLandsideSimSetting = NULL;
	
	m_hTimeStepRoot = NULL;

	m_hItinerantFlight = NULL;
	m_hTrainingFlight = NULL;

}

CSimEngSettingDlg::~CSimEngSettingDlg()
{
	if (m_pSimTimeRangeDlg)
		delete m_pSimTimeRangeDlg;
	if(m_pAirsideSimSettingClosure){
		delete m_pAirsideSimSettingClosure;
		m_pAirsideSimSettingClosure = NULL;
	}
	if (m_pOnboardSimSetting)
	{
		delete m_pOnboardSimSetting;
		m_pOnboardSimSetting = NULL;
	}
	if (m_pLandsideSimSetting)
	{
		delete m_pLandsideSimSetting;
		m_pLandsideSimSetting = NULL;
	}
}

void CSimEngSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimEngSettingDlg)
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_TREE1, m_coolTree);
	DDX_Control(pDX, IDC_EDIT_SIM_NAME, m_controlSimName);
	DDX_Control(pDX, IDC_STATIC_SIM_NAME, m_staticSimName);
	DDX_Text(pDX, IDC_EDIT_SIM_NAME, m_stSimName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimEngSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CSimEngSettingDlg)
	ON_COMMAND(ID_SIMENG_COOLTREE_POPUPMEN_ADDPROC, OnSimengCooltreePopupmenAddproc)
	ON_COMMAND(ID_SIMENG_COOLTREE_POPUPMEN_ADDPAXTYPE, OnSimengCooltreePopupmenAddpaxtype)
	ON_COMMAND(ID_SIMENG_COOLTREE_POPUPMEN_DELPROC, OnSimengCooltreePopupmenDelproc)
	ON_COMMAND(ID_SIMENG_COOLTREE_POPUPMEN_DELPAXTYPE, OnSimengCooltreePopupmenDelpaxtype)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_COMMAND(ID_CLOSUREOPERATION_ADDAIRPORT,OnAddAirport)
	ON_COMMAND(ID_CLOSUREOPERATION_DELETEAIRPORT,OnDeleteAirport)
	ON_COMMAND(ID_CLOSUREOPERATION_ADDRUNWAY,OnAddRunwayClosure)
	ON_COMMAND(ID_CLOSUREOPERATION_DELETERUNWAY,OnDeleteRunwayClosure)
	ON_COMMAND(ID_CLOSUREOPERATION_ADDTAXIWAY,OnAddTaxiwayClosure)
	ON_COMMAND(ID_CLOSUREOPERATION_DELETETAXIWAY,OnDeleteTaxiwayClosure)
	ON_COMMAND(ID_CLOSUREOPERATION_ADDSTANDGROUP,OnAddStandGroupClosure)
	ON_COMMAND(ID_CLOSUREOPERATION_DELETESTANDGROUP,OnDeleteStandGroupClosure)
	ON_COMMAND(ID_ADD_PARKINGLOT,OnAddParkingLot)
	ON_COMMAND(ID_DELETE_PARKINGLOT,OnDeleteParkingLot)
	ON_COMMAND(ID_ADD_CLOSURE,OnAddLandsideClosure)
	ON_COMMAND(ID_DELETE_CLOSURE,OnDeleteLandsideClosure)
	ON_COMMAND(ID_ADD_TIMERANGE,OnAddLandsideTimeRange)
	ON_COMMAND(ID_DELETE_TIMERANGE,OnDeleteLandsideTimeRange)
	ON_COMMAND(ID_LSTIMESTEPRANGE_ADDTIMERANGE,OnAddLandsideTimeStepRange)
	ON_COMMAND(ID_LSTIMESTEPRANGE_DELTIMERANGE,OnDelLandsideTimeStepRange)

	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimEngSettingDlg message handlers

BOOL CSimEngSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//Need Modify
	//m_spinSeed.SetRange( 0, UD_MAXVAL );
	m_pSimTimeRangeDlg = new CSimTimeRangeDlg( m_pSimParam );
	ASSERT(m_pSimTimeRangeDlg);

	if( m_bShowSimName )
	{
		m_staticSimName.ShowWindow( SW_SHOW );
		m_controlSimName.ShowWindow( SW_SHOW );
		
		CTime timeTemp = CTime::GetCurrentTime();
		m_stSimName =timeTemp.Format("%Y-%m-%d %H-%M-%S");	

		m_coolTree.EnableWindow( FALSE );

	}
	LoadLandsideData();
	InitTree();
	LoadData();

	LoadAirsideSimClosures();
	LoadOnboardData();
	
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSimEngSettingDlg::OnAddAirport(void)
{
}

void CSimEngSettingDlg::OnDeleteAirport(void)
{
	HTREEITEM hSel = m_coolTree.GetSelectedItem();
	CClosureAtAirport * pItemData = 0;
	pItemData = (CClosureAtAirport * )m_coolTree.GetItemData(hSel);
	if(pItemData){
		m_coolTree.DeleteItem(hSel);
		switch(pItemData->m_nObjectMark) {
		case 1:
			m_pAirsideSimSettingClosure->m_airsideRunwayClosure.DeleteClosure(pItemData);
			break;
		case 2:
			m_pAirsideSimSettingClosure->m_airsideTaxiwayClosure.DeleteClosure(pItemData);
			break;
		case 3:
			m_pAirsideSimSettingClosure->m_airsideStandGroupClosure.DeleteClosure(pItemData);
			break;
		default:
			break;
		}
	}
}

void CSimEngSettingDlg::OnAddRunwayClosure(void)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(!pDoc)return;	
	CDlgSelectALTObject dlgSelect(pDoc->GetProjectID(),ALT_RUNWAY,this);
	if(dlgSelect.DoModal() == IDOK){
		HTREEITEM hSel = m_coolTree.GetSelectedItem();
		CRunwayClosureAtAirport * pRunwayClosure = 0;
		pRunwayClosure = (CRunwayClosureAtAirport*)m_coolTree.GetItemData(hSel);		

		std::vector<ALTObject> vObject;
		ALTObject::GetObjectList(ALT_RUNWAY, pRunwayClosure->m_nAirportID, vObject);

		int nObjectCount = vObject.size();
		for (int i = 0; i < nObjectCount; ++i)
		{
			ALTObject& altObject = vObject.at(i);
			ALTObjectID objName;
			altObject.getObjName(objName);
			if(dlgSelect.GetALTObject() == objName){
				if(pRunwayClosure){
					CRunwayClosureNode * pNewNode = 0;
					pNewNode = new CRunwayClosureNode;
					if(pNewNode){
						pNewNode->m_nRunwayID = altObject.getID();
						pRunwayClosure->AddNode(pNewNode);
						HTREEITEM hRunway = 0;
						{
							COOLTREE_NODE_INFO cni;
							CCoolTree::InitNodeInfo(this,cni);	

							Runway runway;
							runway.ReadObject(pNewNode->m_nRunwayID);
							IntersectionNodeList listInter = runway.GetIntersectNodes();
							if((int)listInter.size() > 1){
								pNewNode->m_nIntersectNodeIDFrom = listInter.at(0).GetID();
								pNewNode->m_nIntersectNodeIDTo = listInter.at(1).GetID();
							}
							CString strNodeTitle = _T("");
							strNodeTitle.Format(_T("Runway:%s - %s"),runway.GetMarking1().c_str(),runway.GetMarking2().c_str());

							cni.net = NET_NORMAL;
							cni.unMenuID = IDR_MENU_CLOSURE_RUNWAY_DELETE;
							hRunway = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hSel);
							m_coolTree.SetItemData(hRunway,(DWORD)pNewNode); 
							IntersectionNode interNode;
							interNode.ReadData(pNewNode->m_nIntersectNodeIDFrom);

							cni.unMenuID = 0;
							cni.net = NET_COMBOBOX;
							strNodeTitle.Format(_T("From:%s"),interNode.GetName());
							HTREEITEM hInterFrom = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);

							interNode.ReadData(pNewNode->m_nIntersectNodeIDTo);
							strNodeTitle.Format(_T("To:%s"),interNode.GetName());
							HTREEITEM hInterTo = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);

							cni.net = NET_SHOW_DIALOGBOX;
							strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
								pNewNode->m_startTime.GetDay(),\
								pNewNode->m_startTime.GetHour(),\
								pNewNode->m_startTime.GetMinute(),\
								pNewNode->m_startTime.GetSecond(),\
								pNewNode->m_endTime.GetDay(),\
								pNewNode->m_endTime.GetHour(),\
								pNewNode->m_endTime.GetMinute(),\
								pNewNode->m_endTime.GetSecond());
							HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);
						}
						m_coolTree.Expand(hRunway,TVE_EXPAND);
						m_coolTree.Expand(hSel,TVE_EXPAND);
					}
				}
			}
		}
	}
}

void CSimEngSettingDlg::OnDeleteRunwayClosure(void)
{
	HTREEITEM hSel = m_coolTree.GetSelectedItem();
	CRunwayClosureNode* pRunwayNode = 0;
	pRunwayNode = (CRunwayClosureNode*)m_coolTree.GetItemData(hSel);
	CRunwayClosureAtAirport * pRunwayClosure = 0;
	pRunwayClosure = (CRunwayClosureAtAirport*)m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
	if(pRunwayClosure && pRunwayNode){
		pRunwayClosure->DeleteNode(pRunwayNode);
		m_coolTree.DeleteItem(hSel);
	}
}

void CSimEngSettingDlg::OnAddTaxiwayClosure(void)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(!pDoc)return;
	CDlgSelectALTObject dlgSelect(pDoc->GetProjectID(),ALT_TAXIWAY,this);
	if(dlgSelect.DoModal() == IDOK){
		HTREEITEM hSel = m_coolTree.GetSelectedItem();
		CTaxiwayClosureAtAirport * pTaxiwayClosure = 0;
		pTaxiwayClosure = (CTaxiwayClosureAtAirport*)m_coolTree.GetItemData(hSel);		

		std::vector<ALTObject> vObject;
		ALTObject::GetObjectList(ALT_TAXIWAY, pTaxiwayClosure->m_nAirportID, vObject);

		int nObjectCount = vObject.size();
		for (int i = 0; i < nObjectCount; ++i)
		{
			ALTObject& altObject = vObject.at(i);
			ALTObjectID objName;
			altObject.getObjName(objName);
			if(dlgSelect.GetALTObject() == objName)
			{
				if(pTaxiwayClosure)
				{
					CTaxiwayClosureNode * pNewNode = 0;
					pNewNode = new CTaxiwayClosureNode;
					if(pNewNode)
					{
						pNewNode->m_nTaxiwayID = altObject.getID();
						pTaxiwayClosure->AddNode(pNewNode);
						HTREEITEM hTaxiway = 0;
						{
							COOLTREE_NODE_INFO cni;
							CCoolTree::InitNodeInfo(this,cni);	

							Taxiway taxiway;
							taxiway.ReadObject(pNewNode->m_nTaxiwayID);
							IntersectionNodeList listInter = taxiway.GetIntersectNodes();
							if((int)listInter.size() > 1)
							{
								pNewNode->m_nIntersectNodeIDFrom = listInter.at(0).GetID();
								pNewNode->m_nIntersectNodeIDTo = listInter.at(1).GetID();
							}
							CString strNodeTitle = _T("");
							strNodeTitle.Format(_T("Taxiway:%s"),taxiway.GetMarking().c_str());

							cni.net = NET_NORMAL;
							cni.unMenuID = IDR_MENU_CLOSURE_TAXIWAY_DELETE;
							hTaxiway = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hSel);
							m_coolTree.SetItemData(hTaxiway,(DWORD)pNewNode); 

							IntersectionNode interNode;
							interNode.ReadData(pNewNode->m_nIntersectNodeIDFrom);
							cni.unMenuID = 0;
							cni.net = NET_COMBOBOX;
							strNodeTitle.Format(_T("From:%s"),interNode.GetName());
							HTREEITEM hInterFrom = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);
							interNode.ReadData(pNewNode->m_nIntersectNodeIDTo);
							strNodeTitle.Format(_T("To:%s"),interNode.GetName());
							HTREEITEM hInterTo = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);

							cni.net = NET_SHOW_DIALOGBOX;
							strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
								pNewNode->m_startTime.GetDay(),\
								pNewNode->m_startTime.GetHour(),\
								pNewNode->m_startTime.GetMinute(),\
								pNewNode->m_startTime.GetSecond(),\
								pNewNode->m_endTime.GetDay(),\
								pNewNode->m_endTime.GetHour(),\
								pNewNode->m_endTime.GetMinute(),\
								pNewNode->m_endTime.GetSecond());
							HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);
						}
						m_coolTree.Expand(hTaxiway,TVE_EXPAND);
						m_coolTree.Expand(hSel,TVE_EXPAND);
					}
				}
			}
		}
	}
}

void CSimEngSettingDlg::OnDeleteTaxiwayClosure(void)
{
	HTREEITEM hSel = m_coolTree.GetSelectedItem();
	CTaxiwayClosureNode* pTaxiwayNode = 0;
	pTaxiwayNode = (CTaxiwayClosureNode*)m_coolTree.GetItemData(hSel);
	CTaxiwayClosureAtAirport * pTaxiwayClosure = 0;
	pTaxiwayClosure = (CTaxiwayClosureAtAirport*)m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
	if(pTaxiwayClosure && pTaxiwayNode){
		pTaxiwayClosure->DeleteNode(pTaxiwayNode);
		m_coolTree.DeleteItem(hSel);
	}
}

void CSimEngSettingDlg::OnAddStandGroupClosure(void)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(!pDoc)return;
	CDlgStandFamily dlgSelect(pDoc->GetProjectID());
	if(dlgSelect.DoModal() == IDOK){
		HTREEITEM hSel = m_coolTree.GetSelectedItem();
		CStandGroupClosureAtAirport * pStandGroupClosure = 0;
		pStandGroupClosure = (CStandGroupClosureAtAirport*)m_coolTree.GetItemData(hSel);	
	
		if(pStandGroupClosure){
			CStandGroupClosureNode * pNewNode = 0;
			pNewNode = new CStandGroupClosureNode;
			if(pNewNode){
				pNewNode->m_nStandGroupID = dlgSelect.GetSelStandFamilyID();
				pStandGroupClosure->AddNode(pNewNode);
				HTREEITEM hStandGroup = 0;
				{
					COOLTREE_NODE_INFO cni;
					CCoolTree::InitNodeInfo(this,cni);	
					
					CString strNodeTitle = _T("");
					if(pNewNode->m_nStandGroupID == -1)
						strNodeTitle.Format(_T("Stand Group:%s"),_T("All"));
					else
						strNodeTitle.Format(_T("Stand Group:%s"),dlgSelect.GetSelStandFamilyName());

					cni.net = NET_NORMAL;
					cni.unMenuID = IDR_MENU_CLOSURE_STANDGROUP_DELETE;
					hStandGroup = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hSel);
					m_coolTree.SetItemData(hStandGroup,(DWORD)pNewNode);   

					cni.unMenuID = 0;
					cni.net = NET_SHOW_DIALOGBOX;
					strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
						pNewNode->m_startTime.GetDay(),\
						pNewNode->m_startTime.GetHour(),\
						pNewNode->m_startTime.GetMinute(),\
						pNewNode->m_startTime.GetSecond(),\
						pNewNode->m_endTime.GetDay(),\
						pNewNode->m_endTime.GetHour(),\
						pNewNode->m_endTime.GetMinute(),\
						pNewNode->m_endTime.GetSecond());
					HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hStandGroup);
				}
				m_coolTree.Expand(hStandGroup,TVE_EXPAND);
				m_coolTree.Expand(hSel,TVE_EXPAND);
			}
		}		
	}
}

void CSimEngSettingDlg::OnDeleteStandGroupClosure(void)
{
	HTREEITEM hSel = m_coolTree.GetSelectedItem();
	CStandGroupClosureNode * pStandGroupNode = 0;
	pStandGroupNode = (CStandGroupClosureNode * )m_coolTree.GetItemData(hSel);
	CStandGroupClosureAtAirport * pStandGroupClosure = 0;
	pStandGroupClosure = (CStandGroupClosureAtAirport*)m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
	if(pStandGroupClosure && pStandGroupNode){
		pStandGroupClosure->DeleteNode(pStandGroupNode);
		m_coolTree.DeleteItem(hSel);
	}
}

bool CSimEngSettingDlg::LoadAirsideSimClosures(void)
{
	if(m_pAirsideSimSettingClosure){
		delete m_pAirsideSimSettingClosure;
		m_pAirsideSimSettingClosure = NULL;
	}

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(!pDoc)return (false);
	//if(EnvMode_AirSide != pDoc->m_systemMode)return (false);
	m_pAirsideSimSettingClosure = new CAirsideSimSettingClosure(pDoc->GetProjectID());
	if(!m_pAirsideSimSettingClosure)return (false);

	m_pAirsideSimSettingClosure->ReadData(); 

	bool bVehicleService = m_pAirsideSimSettingClosure->IsVehicleService();
	if (bVehicleService)
		m_coolTree.SetCheckStatus(m_hVehicles,TRUE);
	else
		m_coolTree.SetCheckStatus(m_hVehicles,FALSE);

	bool bCyclicGroundRoute = m_pAirsideSimSettingClosure->AllowCyclicGroundRoute();
	if (bCyclicGroundRoute)
	{
		m_coolTree.SetCheckStatus(m_hCyclicGroundRoute,TRUE);
	}
	else
	{
		m_coolTree.SetCheckStatus(m_hCyclicGroundRoute,FALSE);
	}

	if(m_pAirsideSimSettingClosure->IsItinerantFlight())
	{
		m_coolTree.SetCheckStatus(m_hItinerantFlight, TRUE);
	}
	else
	{
		m_coolTree.SetCheckStatus(m_hItinerantFlight, FALSE);
	}

	if(m_pAirsideSimSettingClosure->IsTrainingFlight())
	{
		m_coolTree.SetCheckStatus(m_hTrainingFlight, TRUE);
	}
	else
	{
		m_coolTree.SetCheckStatus(m_hTrainingFlight, FALSE);
	}




	SetRunwayClosureTreeItemContent();
	SetTaxiwayClosureTreeItemContent();
	SetStandGroupClosureTreeItemContent();

	return (true);
}

bool CSimEngSettingDlg::SetTaxiwayClosureTreeItemContent(void)
{
	if(!m_pAirsideSimSettingClosure)return (false);

	int nCount = -1,nIndex = -1;
	//taxiway closure
	nCount = m_pAirsideSimSettingClosure->m_airsideTaxiwayClosure.GetAirportCount();
	for (nIndex = 0;nIndex < nCount;++nIndex) {
		CTaxiwayClosureAtAirport * pTaxiwayClosure = m_pAirsideSimSettingClosure->m_airsideTaxiwayClosure.GetTaxiwayClosureAtAirport(nIndex);
		if(!pTaxiwayClosure) continue;

		ALTAirport airport;
		airport.ReadAirport(pTaxiwayClosure->m_nAirportID);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);	

		cni.net = NET_NORMAL;
		cni.unMenuID = IDR_MENU_CLOSURE_TAXIWAY_ADD;
		HTREEITEM hTaxiwayClosure = m_coolTree.InsertItem(airport.getName(),cni,FALSE,FALSE,m_hTaxiwayClosures);
		m_coolTree.SetItemData(hTaxiwayClosure,(DWORD)pTaxiwayClosure);

		int nTaxiwayClosureNodeCount = -1;
		nTaxiwayClosureNodeCount = pTaxiwayClosure->GetTaxiwayClosureNodeCount();
		for (int i = 0;i < nTaxiwayClosureNodeCount;++i) {
			CTaxiwayClosureNode * pTaxiwayClosureNode = pTaxiwayClosure->GetTaxiwayClosureNode(i);
			if(!pTaxiwayClosureNode)continue;

			CCoolTree::InitNodeInfo(this,cni);	

			Taxiway taxiway;
			taxiway.ReadObject(pTaxiwayClosureNode->m_nTaxiwayID);
			CString strNodeTitle = _T("");
			strNodeTitle.Format(_T("Taxiway:%s"),taxiway.GetMarking().c_str());

			cni.net = NET_NORMAL;
			cni.unMenuID = IDR_MENU_CLOSURE_TAXIWAY_DELETE;
			HTREEITEM hTaxiway = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiwayClosure);
			m_coolTree.SetItemData(hTaxiway,(DWORD)pTaxiwayClosureNode); 

			IntersectionNode interNode;
			interNode.ReadData(pTaxiwayClosureNode->m_nIntersectNodeIDFrom);
			cni.unMenuID = 0;
			cni.net = NET_COMBOBOX;
			strNodeTitle.Format(_T("From:%s"),interNode.GetName());
			HTREEITEM hInterFrom = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);
			interNode.ReadData(pTaxiwayClosureNode->m_nIntersectNodeIDTo);
			strNodeTitle.Format(_T("To:%s"),interNode.GetName());
			HTREEITEM hInterTo = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);

			cni.net = NET_SHOW_DIALOGBOX;
			strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
				pTaxiwayClosureNode->m_startTime.GetDay(),\
				pTaxiwayClosureNode->m_startTime.GetHour(),\
				pTaxiwayClosureNode->m_startTime.GetMinute(),\
				pTaxiwayClosureNode->m_startTime.GetSecond(),\
				pTaxiwayClosureNode->m_endTime.GetDay(),\
				pTaxiwayClosureNode->m_endTime.GetHour(),\
				pTaxiwayClosureNode->m_endTime.GetMinute(),\
				pTaxiwayClosureNode->m_endTime.GetSecond());
			HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hTaxiway);

			m_coolTree.Expand(hTaxiway, TVE_EXPAND);
		} 
		m_coolTree.Expand(hTaxiwayClosure, TVE_EXPAND);
	}

	m_coolTree.Expand(m_hTaxiwayClosures, TVE_EXPAND);
	return (true);
}

bool CSimEngSettingDlg::SetStandGroupClosureTreeItemContent(void)
{
	int nCount = -1,nIndex = -1;
	//runway closure
	nCount = m_pAirsideSimSettingClosure->m_airsideStandGroupClosure.GetAirportCount();
	for (nIndex = 0;nIndex < nCount;++nIndex) {
		CStandGroupClosureAtAirport * pStandGroupClosure = m_pAirsideSimSettingClosure->m_airsideStandGroupClosure.GetStandGroupClosureAtAirport(nIndex);
		if(!pStandGroupClosure) continue;

		ALTAirport airport;
		airport.ReadAirport(pStandGroupClosure->m_nAirportID);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);	

		cni.net = NET_NORMAL;
		cni.unMenuID = IDR_MENU_CLOSURE_STANDGROUP_ADD;
		HTREEITEM hStandGroupClosure = m_coolTree.InsertItem(airport.getName(),cni,FALSE,FALSE,m_hStandGroupClosures);
		m_coolTree.SetItemData(hStandGroupClosure,(DWORD)pStandGroupClosure);

		int nStandGroupClosureNodeCount = -1;
		nStandGroupClosureNodeCount = pStandGroupClosure->GetStandGroupClosureNodeCount();
		for (int i = 0;i < nStandGroupClosureNodeCount;++i) {
			CStandGroupClosureNode * pStandGroupClosureNode = pStandGroupClosure->GetStandGroupClosureNode(i);
			if(!pStandGroupClosureNode)continue;

			CCoolTree::InitNodeInfo(this,cni);	

			ALTObjectGroup altObjGroup;	
			CString strNodeTitle = _T("");
			if(pStandGroupClosureNode->m_nStandGroupID == -1)
				strNodeTitle.Format(_T("Stand Group:%s"),_T("All"));
			else{
				altObjGroup.ReadData(pStandGroupClosureNode->m_nStandGroupID);
				strNodeTitle.Format(_T("Stand Group:%s"),altObjGroup.getName().GetIDString());
			}

			cni.net = NET_NORMAL;
			cni.unMenuID = IDR_MENU_CLOSURE_STANDGROUP_DELETE;
			HTREEITEM hStandGroup = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hStandGroupClosure);
			m_coolTree.SetItemData(hStandGroup,(DWORD)pStandGroupClosureNode);   

			cni.unMenuID = 0;
			cni.net = NET_SHOW_DIALOGBOX;
			strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
				pStandGroupClosureNode->m_startTime.GetDay(),\
				pStandGroupClosureNode->m_startTime.GetHour(),\
				pStandGroupClosureNode->m_startTime.GetMinute(),\
				pStandGroupClosureNode->m_startTime.GetSecond(),\
				pStandGroupClosureNode->m_endTime.GetDay(),\
				pStandGroupClosureNode->m_endTime.GetHour(),\
				pStandGroupClosureNode->m_endTime.GetMinute(),\
				pStandGroupClosureNode->m_endTime.GetSecond());
			HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hStandGroup);

			m_coolTree.Expand(hStandGroup, TVE_EXPAND);
		} 
		m_coolTree.Expand(hStandGroupClosure, TVE_EXPAND);
	}

	m_coolTree.Expand(m_hStandGroupClosures, TVE_EXPAND);
	return (true);
}

bool CSimEngSettingDlg::SetRunwayClosureTreeItemContent(void)
{
	int nCount = -1,nIndex = -1;
	//runway closure
	nCount = m_pAirsideSimSettingClosure->m_airsideRunwayClosure.GetAirportCount();
	for (nIndex = 0;nIndex < nCount;++nIndex) {
		CRunwayClosureAtAirport * pRunwayClosure = m_pAirsideSimSettingClosure->m_airsideRunwayClosure.GetRunwayClosureAtAirport(nIndex);
		if(!pRunwayClosure) continue;

		ALTAirport airport;
		airport.ReadAirport(pRunwayClosure->m_nAirportID);
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);	
		
		cni.net = NET_NORMAL;
		cni.unMenuID = IDR_MENU_CLOSURE_RUNWAY_ADD;
		HTREEITEM hRunwayClosure = m_coolTree.InsertItem(airport.getName(),cni,FALSE,FALSE,m_hRunwayClosures);
		m_coolTree.SetItemData(hRunwayClosure,(DWORD)pRunwayClosure);

		int nRunwayClosureNodeCount = -1;
		nRunwayClosureNodeCount = pRunwayClosure->GetRunwayClosureNodeCount();
		for (int i = 0;i < nRunwayClosureNodeCount;++i) {
			CRunwayClosureNode * pRunwayClosureNode = pRunwayClosure->GetRunwayClosureNode(i);
			if(!pRunwayClosureNode)continue;

			CCoolTree::InitNodeInfo(this,cni);	

			Runway runway;
			runway.ReadObject(pRunwayClosureNode->m_nRunwayID);
			CString strNodeTitle = _T("");
			strNodeTitle.Format(_T("Runway:%s - %s"),runway.GetMarking1().c_str(),runway.GetMarking2().c_str());

			cni.net = NET_NORMAL;
			cni.unMenuID = IDR_MENU_CLOSURE_RUNWAY_DELETE;
			HTREEITEM hRunway = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunwayClosure);
			m_coolTree.SetItemData(hRunway,(DWORD)pRunwayClosureNode); 
			IntersectionNode interNode;
			interNode.ReadData(pRunwayClosureNode->m_nIntersectNodeIDFrom);

			cni.unMenuID = 0;
			cni.net = NET_COMBOBOX;
			strNodeTitle.Format(_T("From:%s"),interNode.GetName());
			HTREEITEM hInterFrom = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);

			interNode.ReadData(pRunwayClosureNode->m_nIntersectNodeIDTo);
			strNodeTitle.Format(_T("To:%s"),interNode.GetName());
			HTREEITEM hInterTo = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);

			cni.net = NET_SHOW_DIALOGBOX;
			strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
				pRunwayClosureNode->m_startTime.GetDay(),\
				pRunwayClosureNode->m_startTime.GetHour(),\
				pRunwayClosureNode->m_startTime.GetMinute(),\
				pRunwayClosureNode->m_startTime.GetSecond(),\
				pRunwayClosureNode->m_endTime.GetDay(),\
				pRunwayClosureNode->m_endTime.GetHour(),\
				pRunwayClosureNode->m_endTime.GetMinute(),\
				pRunwayClosureNode->m_endTime.GetSecond());
			HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hRunway);

			m_coolTree.Expand(hRunway, TVE_EXPAND);
		} 
		m_coolTree.Expand(hRunwayClosure, TVE_EXPAND);
	}

	m_coolTree.Expand(m_hRunwayClosures, TVE_EXPAND);

	return (true);
}

void CSimEngSettingDlg::LoadData()
{
	char szBuf[256];
	
	// if num of run is more than 1, make random seed disable
	if( m_pSimParam->GetSimPara()->GetNumberOfRun() >1 )
		m_coolTree.SetEnableStatus( m_hRandomSeed, FALSE );
	else
		m_coolTree.SetEnableStatus( m_hRandomSeed, TRUE );

	
	int nRandomOptions = m_pSimParam->GetRandomFlag();
	if( nRandomOptions == 1)
	{
		m_coolTree.SetRadioStatus( m_TrueRandom );
		m_coolTree.SetItemText( m_coolTree.GetChildItem( m_TrueRandom ) , "0" );
	}
	else
	{
		m_coolTree.SetRadioStatus( m_hRandomSeed );
		CString strValue;
		strValue.Format("%d",m_pSimParam->GetSeed() );
		m_coolTree.SetItemText( m_hRandomSeed  ,"Random Seed:  " + strValue );
		m_coolTree.SetItemData( m_hRandomSeed, m_pSimParam->GetSeed() );
	}	

	/*
	CString sValue;
	sValue.Format("%d",m_pSimParam->GetStartTime().asHours() );
	m_coolTree.SetItemText( m_hFlightFilterStartTime  ,"Start Hours:  "+sValue );

	sValue.Format("%d",m_pSimParam->GetEndTime().asHours() );
	m_coolTree.SetItemText( m_hFlightFilterEndTime  ,"End Hours:  "+sValue );*/
	HTREEITEM hItemChild = NULL;
	//while (m_coolTree.ItemHasChildren(m_hFlightFilter))
	//{
	//	hItemChild = m_coolTree.GetChildItem(m_hFlightFilter);
	//	m_coolTree.DeleteItem(hItemChild);
	//}

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this, cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	//CString sStart;
	//CString sEnd;

	//COleDateTime outTime;


	//	ElapsedTime eStart = m_pSimParam->GetStartTime();


	//	sStart.Format("Start Day: %d Time: ",eStart.GetDay());
	//	outTime.SetTime(eStart.GetHour(), eStart.GetMinute(), eStart.GetSecond());
	//	sStart += outTime.Format(_T("%X"));

	//	//sEnd = _T("End: ");
	//	ElapsedTime eEnd = m_pSimParam->GetEndTime();
	//	sEnd.Format("End Day: %d Time: ",eEnd.GetDay());
	//	outTime.SetTime(eEnd.GetHour(), eEnd.GetMinute(), eEnd.GetSecond());
	//	sEnd += outTime.Format(_T("%X"));

	//m_coolTree.InsertItem(sStart, cni, TRUE, FALSE, m_hFlightFilter);	
	//m_coolTree.InsertItem(sEnd, cni, TRUE, FALSE, m_hFlightFilter);

	//CString sValue;
	//sValue = _T("Initialization Period: ");
	//sValue += m_pSimParam->GetInitPeriod().printTime(0);
	//m_coolTree.InsertItem(sValue, cni, TRUE, FALSE, m_hFlightFilter);

	//sValue = _T("Follow Up Period: ");
	//sValue += m_pSimParam->GetFollowupPeriod().printTime(0);
	//m_coolTree.InsertItem(sValue, cni, TRUE, FALSE, m_hFlightFilter);
	//m_coolTree.Expand(m_hFlightFilter, TVE_EXPAND);
	

	if( m_pSimParam->GetPaxCount() >= 0 )
	{
		m_coolTree.SetCheckStatus( m_hMPCount, TRUE );
		CString strValue;
		strValue.Format("%d",m_pSimParam->GetPaxCount() );
		m_coolTree.SetItemText( m_hMPCount  , "Maximum Passenger Count:  " +strValue );
		m_coolTree.SetItemData( m_hMPCount,(DWORD)m_pSimParam->GetPaxCount());
	}
	else
	{
		m_coolTree.SetCheckStatus( m_hMPCount, FALSE );
		m_coolTree.SetItemText( m_hMPCount , "Maximum Passenger Count:  30000" );
		m_coolTree.SetItemData( m_hMPCount,(DWORD)30000);
	}	

	CString strValue;
	strValue.Format("%d",m_pSimParam->GetSimPara()->GetNumberOfRun() );
	m_coolTree.SetItemText( m_hNuberOfRun  ,"Number Of Runs:  "+strValue );
	m_coolTree.SetItemData( m_hNuberOfRun, m_pSimParam->GetSimPara()->GetNumberOfRun() );
	
	m_coolTree.SetCheckStatus( m_hNoPax, m_pSimParam->GetVisitorsFlag() );
	m_coolTree.SetCheckStatus( m_hBarrier , m_pSimParam->GetBarrierFlag() );
	m_coolTree.SetCheckStatus( m_hHub , m_pSimParam->GetHubbingFlag() );
	m_coolTree.SetCheckStatus(m_hCongestionImpact,m_pSimParam->GetCongestionImpactFlag());
	
	m_coolTree.SetCheckStatus(m_hAvoidance,m_pSimParam->GetCollisionFlag());
	m_coolTree.SetCheckStatus(m_hArea_avoid,m_pSimParam->GetAreaFlag());
	int iCount  = m_vAllNoPaxDetailItem.size();
	for( int i=0; i<iCount; ++i )
	{
		m_coolTree.SetCheckStatus( m_vAllNoPaxDetailItem[i], m_pSimParam->GetNoPaxDetailFlag().at( i ));
	}

	m_coolTree.SetCheckStatus(m_hPipe_avoid,m_pSimParam->GetPipFlage());
	iCount = m_vAreaItem.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_coolTree.SetCheckStatus(m_vAreaItem[i],m_pSimParam->GetAreaItemFlag(m_coolTree.GetItemText(m_vAreaItem[i])));
	}

	iCount = m_vPipeItem.size();
	for (int i = 0; i < iCount; ++i)
	{
		m_coolTree.SetCheckStatus(m_vPipeItem[i],m_pSimParam->GetPipeItemFlag(m_coolTree.GetItemText(m_vPipeItem[i])));
	}
	CSimGeneralPara* pPara = m_pSimParam->GetSimPara();
	//CSimAutoReportPara* pAutoPara = NULL;
	CString strClassType=pPara->GetClassType();
	if(strClassType == "AUTOREPORT" )
	{
		m_coolTree.SetCheckStatus(m_hOptional,TRUE);
		m_coolTree.SetRadioStatus(m_hAutoReport,TRUE);
		int iCount = ((CSimAutoReportPara*)pPara)->GetReportCount();
		for( int i=0; i<iCount; ++i )
		{
			ENUM_REPORT_TYPE iType = ((CSimAutoReportPara*)pPara)->At(i);
			switch ( iType )
			{
				// passenger
				case ENUM_PAXLOG_REP :
					m_coolTree.SetCheckStatus(m_hActivity,TRUE);
					break;
				case ENUM_DISTANCE_REP:
					m_coolTree.SetCheckStatus(m_hDistance,TRUE);
					break;
				case ENUM_QUEUETIME_REP:
					m_coolTree.SetCheckStatus(m_hTimeInQue,TRUE);
					break;
				case ENUM_DURATION_REP:
					m_coolTree.SetCheckStatus(m_hTimeInTerm,TRUE);
					break;
				case ENUM_SERVICETIME_REP:
					m_coolTree.SetCheckStatus(m_hTimeInSer,TRUE);
					break;
				case ENUM_ACTIVEBKDOWN_REP:
					m_coolTree.SetCheckStatus(m_hActivityBreakdown,TRUE);
					break;
				
				// processor
				case ENUM_UTILIZATION_REP:
					m_coolTree.SetCheckStatus(m_hUiti,TRUE);
					break;
				case ENUM_THROUGHPUT_REP:
					m_coolTree.SetCheckStatus(m_hThroughtput,TRUE);
					break;
				case ENUM_QUEUELENGTH_REP:
					m_coolTree.SetCheckStatus(m_hQueueLen,TRUE);
					break;
				case ENUM_AVGQUEUELENGTH_REP:
					m_coolTree.SetCheckStatus(m_hCriticalQuePara,TRUE);
					break;
				
				// space
				case ENUM_PAXCOUNT_REP:
					m_coolTree.SetCheckStatus(m_hOccupancy,TRUE);
					break;
				case ENUM_PAXDENS_REP:
					m_coolTree.SetCheckStatus(m_hDensity,TRUE);
					break;
				case ENUM_SPACETHROUGHPUT_REP:
						m_coolTree.SetCheckStatus(m_hSpaceThroutput,TRUE);
					break;
				case ENUM_COLLISIONS_REP:
						m_coolTree.SetCheckStatus(m_hCollision,TRUE);
						break;
				// ac 
				case ENUM_ACOPERATION_REP:
					m_coolTree.SetCheckStatus(m_hAO,TRUE);
					break;
			
				// bag
				case ENUM_BAGCOUNT_REP:
						m_coolTree.SetCheckStatus(m_hBagCount,TRUE);
					break;
				case ENUM_BAGWAITTIME_REP:
							m_coolTree.SetCheckStatus(m_hBagWaitTime,TRUE);
						break;
				case ENUM_BAGDISTRIBUTION_REP:
							m_coolTree.SetCheckStatus(m_hDistribution,TRUE);
						break;
				case ENUM_BAGDELIVTIME_REP:
								m_coolTree.SetCheckStatus(m_hDeliveryTime,TRUE);
						break;
			
					// economic
				case ENUM_ECONOMIC_REP :
						m_coolTree.SetCheckStatus(m_hEconomic,TRUE);
					break;
			}
		}
		
	}
	else if(strClassType=="LEVEOFSERVICE")
	{
		m_coolTree.SetCheckStatus(m_hOptional,TRUE);
		m_coolTree.SetRadioStatus(m_hTLOS,TRUE);
		CSimLevelOfService* pSARP=(CSimLevelOfService*)pPara;
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		if( pSARP->getDynamicCreateProcFlag() )
		{
			m_bAutoCreateProc = TRUE;
			m_coolTree.SetCheckStatus(  m_hAutoCreateProc, TRUE );
		}
		else
		{
			m_bAutoCreateProc = FALSE;
			m_coolTree.SetCheckStatus(  m_hAutoCreateProc, FALSE );
		}
		int size = pSARP->getSettingCout();
		for( int i=0; i< size; i++ )
		{
			const CSimLevelOfServiceItem& item = pSARP->getSetting( i );
			CString strProcName = item.GetProcessorName();
			cni.nt = NT_NORMAL;
			cni.net= NET_NORMAL;
			HTREEITEM hProcItem = m_coolTree.InsertItem(strProcName,cni,FALSE,FALSE,m_hTLOSProcessor);

			const std::vector<Mob_ServicTime>& MSI = item.getSpecificPaxTypeService();
		
			CString strPaxType;
			SERVICE_ITEMS SI;
			for(int i=0; i<static_cast<int>(MSI.size()); i++ )
			{
				Mob_ServicTime mob_st =  MSI[i];
				mob_st.first.screenPrint( szBuf );
				strPaxType = szBuf;
				cni.nt = NT_NORMAL;
				cni.net = NET_NORMAL;
				HTREEITEM hPaxtypeItem = m_coolTree.InsertItem(strPaxType,cni,FALSE,FALSE,hProcItem);
				//////////////////////////////////////////////////////////////////////////
				CMobileElemConstraint *p_mob = new CMobileElemConstraint(GetInputTerminal());
				*(p_mob) = mob_st.first;
				p_mob->SetInputTerminal( GetInputTerminal() );
				m_coolTree.SetItemData( hPaxtypeItem, (DWORD)p_mob );
				//////////////////////////////////////////////////////////////////////////

				SI = mob_st.second;
				CString strQLU,strQLL,strQWSU,strQWSL,strAN,strADU,strADL;
				strQLU.Format("%d",SI.m_iUpperQueueLength);
				strQLL.Format("%d",SI.m_iLowerQueueLength);
				strQWSU.Format("%d",SI.m_lUpperQueueWaitingSeconds);
				strQWSL.Format("%d",SI.m_lLowerQueueWaitingSeconds);
				strAN=SI.m_strAreaName;
				strADU.Format("%d",SI.m_iUpperAreaDensity);
				strADL.Format("%d",SI.m_iLowerAreaDensity);
				InsertAllTypeChildItem(hPaxtypeItem,&strQLU,&strQLL,&strQWSU,&strQWSL,&strAN,&strADU,&strADL);
			}
		}			
	}

	UpdateData( false );
}

bool CSimEngSettingDlg::LoadOnboardData()
{
	if (m_pOnboardSimSetting == NULL)
	{
		m_pOnboardSimSetting = new OnboardSimSetting;
	}

	try
	{
		m_pOnboardSimSetting->ReadData();
	}
	catch (CADOException* error)
	{
		CString strError = _T("");
		strError.Format("Database operation error: %s",error->ErrorMessage());
		MessageBox(strError);

		delete error;
		error = NULL;
	}

	bool bTimeApply = m_pOnboardSimSetting->IsBoardingTimeApply();
	if (bTimeApply)
		m_coolTree.SetCheckStatus(m_hBoardingTime,TRUE);
	else
		m_coolTree.SetCheckStatus(m_hBoardingTime,FALSE);

	bTimeApply = m_pOnboardSimSetting->IsDeplanementTimeApply();
	if (bTimeApply)
		m_coolTree.SetCheckStatus(m_hDeplanementTime,TRUE);
	else
		m_coolTree.SetCheckStatus(m_hDeplanementTime,FALSE);

	return true;

}


CString CSimEngSettingDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


InputTerminal* CSimEngSettingDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

void CSimEngSettingDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	try
	{
		m_pSimParam->loadDataSet( GetProjPath() );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	clearMobConstraintPointer();
	CDialog::OnCancel();
}

void CSimEngSettingDlg::OnOK() 
{
	// TODO: Add extra validation here

	//add by adam 2007-09-28
	//start time
	int nDay = (int)m_coolTree.GetItemData(m_hStartDay);
	long nSecondTime = (long)m_coolTree.GetItemData(m_hStartTime);
	ElapsedTime estStartTime(nSecondTime);
	estStartTime.SetDay(nDay);
	m_pSimParam->SetStartTime(estStartTime);

	//end time
	nDay = (int)m_coolTree.GetItemData(m_hEndDay);
	nSecondTime = (long)m_coolTree.GetItemData(m_hEndTime);
	ElapsedTime estEndTime(nSecondTime);
	estEndTime.SetDay(nDay);
	m_pSimParam->SetEndTime(estEndTime);

	//initial period
	nSecondTime = (long)m_coolTree.GetItemData(m_hInitialPeriodTime);
	ElapsedTime estInitialPerildTime(nSecondTime);
	m_pSimParam->SetInitPeriod(estInitialPerildTime);

	//follow up time
	nSecondTime = (long)m_coolTree.GetItemData(m_hFollowUpTime);
	ElapsedTime estFollowUpTime(nSecondTime);
	m_pSimParam->SetFollowupPeriod(estFollowUpTime);
	//End add by adam 2007-09-28

	if(m_coolTree.IsCheckItem(m_hOptional)==1&& m_pSimParam->GetSimPara())
	{
		if(m_coolTree.IsRadioItem(m_hAutoReport))
			SetAutoReport(m_pSimParam);			
		else
		{
			if(m_coolTree.GetChildItem(m_hTLOSProcessor)==NULL)
			{
				AfxMessageBox("Please add a processor.");
				return;
			}
			if( !SetTargetLevel( m_pSimParam ) )
			{
				return;
			}
		}
	}
	else if( m_coolTree.IsCheckItem(m_hOptional)==0&&m_pSimParam->GetSimPara() )	// restore to general paramter
	{
		SetGeneralPara(m_pSimParam);
	}

	m_pSimParam->saveDataSet( GetProjPath(), true );	
	if(m_pAirsideSimSettingClosure)
	{
		if (m_coolTree.IsCheckItem(m_hVehicles) == 1)
			m_pAirsideSimSettingClosure->SetVehicleService(true);
		else
			m_pAirsideSimSettingClosure->SetVehicleService(false);

		if (m_coolTree.IsCheckItem(m_hCyclicGroundRoute) == 1)
			m_pAirsideSimSettingClosure->SetCyclicGroundRoute(true);
		else
			m_pAirsideSimSettingClosure->SetCyclicGroundRoute(false);

		if(m_coolTree.IsCheckItem(m_hItinerantFlight) == 1)
			m_pAirsideSimSettingClosure->SetItinerantFlight(true);
		else
			m_pAirsideSimSettingClosure->SetItinerantFlight(false);

		if(m_coolTree.IsCheckItem(m_hTrainingFlight) == 1)
			m_pAirsideSimSettingClosure->SetTrainingFlight(true);
		else
			m_pAirsideSimSettingClosure->SetTrainingFlight(false);

		m_pAirsideSimSettingClosure->SaveData();
	}
	clearMobConstraintPointer();

	//onboard setting save
	if (m_pOnboardSimSetting != NULL)
	{
		if (m_coolTree.IsCheckItem(m_hBoardingTime) == 1)
			m_pOnboardSimSetting->SetBoardingTimeApply(true);
		else
			m_pOnboardSimSetting->SetBoardingTimeApply(false);

		if (m_coolTree.IsCheckItem(m_hDeplanementTime) == 1)
			m_pOnboardSimSetting->SetDeplanementTimeApply(true);
		else
			m_pOnboardSimSetting->SetDeplanementTimeApply(false);

		try
		{
			CADODatabase::BeginTransaction();
			m_pOnboardSimSetting->SaveData();
			CADODatabase::CommitTransaction();
		}
		catch (CADOException* error)
		{
			CADODatabase::RollBackTransation();
			CString strError = _T("");
			strError.Format("Database operation error: %s",error->ErrorMessage());
			MessageBox(strError);
            
			delete error;
			error = NULL;
		}
	}

	if (m_pLandsideSimSetting)
	{
		if (m_coolTree.IsCheckItem(m_hVehicleSim) == 1)
			m_pLandsideSimSetting->SetVehicleSim(true);
		else
			m_pLandsideSimSetting->SetVehicleSim(false);
		try
		{
			CADODatabase::BeginTransaction();
			m_pLandsideSimSetting->SaveData();
			CADODatabase::CommitTransaction();
		}
		catch (CADOException* error)
		{
			CADODatabase::RollBackTransation();
			CString strError = _T("");
			strError.Format("Database operation error: %s",error->ErrorMessage());
			MessageBox(strError);

			delete error;
			error = NULL;
		}
	}

	GetInputTerminal()->m_pProcToResourceDB->saveDataSet(GetProjPath(), false);

	CDialog::OnOK();
}

void CSimEngSettingDlg::InitLandsideTree(COOLTREE_NODE_INFO& cni)
{
	//time step root
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	m_hTimeStepRoot = m_coolTree.InsertItem(_T("Time Step"), cni, FALSE, FALSE, m_hLandside);


	//time step
	cni.nt=NT_NORMAL;
	cni.net = NET_EDIT_INT;

	CString strNode;
	long lTimeStep = m_pLandsideSimSetting?m_pLandsideSimSetting->GetDefaultTimeStep():0;
	strNode.Format(_T("Default: %d ms"),lTimeStep);
	cni.fMinValue = static_cast<float>(lTimeStep);
	m_hTimeStep=m_coolTree.InsertItem(strNode,cni,FALSE,FALSE,m_hTimeStepRoot);
	m_coolTree.Expand(m_hTimeStepRoot, TVE_EXPAND);

	if (m_pLandsideSimSetting == NULL)
		return;

	//Time range
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;
	cni.unMenuID = IDR_MENU_LSSIMSETTINGTIMESTEP;
	m_hTimeStepRangeRoot = m_coolTree.InsertItem(_T("Time Range"), cni, FALSE, FALSE, m_hTimeStepRoot);

	TimeStepRangeList* pTimeRangeList = m_pLandsideSimSetting->GetTimeStepRangeList();
	int nRangCount = pTimeRangeList->GetItemCount();
	for (int nRange = 0; nRange < nRangCount; ++ nRange)
	{

		TimeStepRange* pTimeRange = pTimeRangeList->GetItem(nRange);
		if(pTimeRange)
			AddLandisdeTimeStep(pTimeRange);
	}

	////vehicle sim
	//cni.net=NET_NORMAL;
	//cni.nt=NT_CHECKBOX;
	//m_hVehicleSim =  m_coolTree.InsertItem("Non Pax Vehicle Simulation",cni,TRUE,FALSE,m_hLandside);
	//BOOL bCheck = FALSE;
	//if (m_pLandsideSimSetting)
	//{
	//	bCheck = m_pLandsideSimSetting->GetVehicleSim()?TRUE:FALSE;
	//}
	//m_coolTree.SetCheckStatus(m_hVehicleSim,bCheck);

	cni.net = NET_NORMAL;
	cni.nt=NT_NORMAL;
	cni.unMenuID = IDR_MENU_LANDSIDE_ADDCLOSURE;
	m_hStrecthClos = m_coolTree.InsertItem(_T("Stretch Closure"),cni,TRUE,FALSE,m_hLandside);
	m_hIntersectionClos = m_coolTree.InsertItem(_T("Intersection Closure"),cni,TRUE,FALSE,m_hLandside);
	m_hRoundClos = m_coolTree.InsertItem(_T("Roundabout Closure"),cni,TRUE,FALSE,m_hLandside);
	m_hParkinglotClos = m_coolTree.InsertItem(_T("ParkingLot Closure"),cni,TRUE,FALSE,m_hLandside);
	m_hNameStretchClos = m_coolTree.InsertItem(_T("NamedStretchSegment Closure"),cni,TRUE,FALSE,m_hLandside);

	cni.unMenuID = IDR_MENU_ADDPARKINGLOT;
	//m_hParkingLot = m_coolTree.InsertItem(_T("Parking Lot Initial Occupancy:"),cni,TRUE,FALSE,m_hLandside);


	//Stretch Closure
	int nStretch = m_pLandsideSimSetting->GetStretchClosure()->GetCount();
	for (int i = 0; i < nStretch; i++)
	{
		LandsideClosure* pLandsideClosure = m_pLandsideSimSetting->GetStretchClosure()->GetItem(i);
		AddLandsideClosureItem(m_hStrecthClos,pLandsideClosure);
	}
	
	//Parking Lot Closure
	int nParkingLot = m_pLandsideSimSetting->GetParkingLotClosure()->GetCount();
	for (i = 0;  i < nParkingLot; i++)
	{
		LandsideClosure* plandsideClosure = m_pLandsideSimSetting->GetParkingLotClosure()->GetItem(i);
		AddLandsideClosureItem(m_hParkinglotClos,plandsideClosure);
	}

	//Named Stretch Segment Closure
	int nNamedStretchSegment = m_pLandsideSimSetting->GetNamedStretchSegmentClosure()->GetCount();
	for (i = 0; i < nNamedStretchSegment; i++)
	{
		LandsideClosure* pLandsideClosure = m_pLandsideSimSetting->GetNamedStretchSegmentClosure()->GetItem(i);
		AddLandsideClosureItem(m_hNameStretchClos,pLandsideClosure);
	}
	//Intersection Closure
	int nIntersection = m_pLandsideSimSetting->GetIntersectionClosure()->GetCount();
	for (int i = 0; i < nIntersection; i++)
	{
		LandsideClosure* pLandsideClosure = m_pLandsideSimSetting->GetIntersectionClosure()->GetItem(i);
		AddLandsideClosureItem(m_hIntersectionClos,pLandsideClosure);
	}

	//Roundabout Closure
	int nRound = m_pLandsideSimSetting->GetRoundaboutClosure()->GetCount();
	for (int i = 0; i < nRound; i++)
	{
		LandsideClosure* pLandsideClosure = m_pLandsideSimSetting->GetRoundaboutClosure()->GetItem(i);
		AddLandsideClosureItem(m_hRoundClos,pLandsideClosure);
	}
	
	//Parking Lot Initial Occupancy
	/*int nParkingLot = m_pLandsideSimSetting->GetParkingLotClosure()->GetCount();
	for (i = 0; i < nParkingLot; i++)
	{
		LandsideClosure* pLandsideClosure = m_pLandsideSimSetting->GetParkingLotClosure()->GetItem(i);
		AddParkinglotClosureItem(m_hParkingLot,pLandsideClosure);
	}*/
}

void CSimEngSettingDlg::InitTree()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	//Root
	//General
	m_hGeneral=m_coolTree.InsertItem("General",cni,FALSE);
	
	cni.nt=NT_RADIOBTN;
	cni.net = NET_EDITSPIN_WITH_VALUE;
	m_hRandomSeed=m_coolTree.InsertItem("Random Seed:  3",cni,FALSE,FALSE,m_hGeneral);
	m_coolTree.SetItemData( m_hRandomSeed, (DWORD)3 );
	m_coolTree.SetItemValueRange(m_hRandomSeed,0,100);
	
	cni.nt=NT_RADIOBTN; 
	cni.net = NET_NORMAL;
	m_TrueRandom=m_coolTree.InsertItem("True Random",cni,TRUE,FALSE,m_hGeneral);

	cni.nt=NT_NORMAL; 
	cni.net=NET_EDITSPIN_WITH_VALUE;
	m_hNuberOfRun = m_coolTree.InsertItem("Number Of Runs:  1",cni,TRUE,FALSE,m_hGeneral);	
	m_coolTree.SetItemData( m_hNuberOfRun,(DWORD) 1 );
	m_coolTree.SetItemValueRange(m_hNuberOfRun,1,100);

	cni.nt=NT_NORMAL; 
	cni.net = NET_NORMAL;
	ASSERT(m_pSimTimeRangeDlg);
	cni.pEditWnd = m_pSimTimeRangeDlg;
	m_hFlightFilter = m_coolTree.InsertItem("Simulation Time Range", cni, TRUE, FALSE, m_hGeneral);

	//add by adam 2007-09-27
	ElapsedTime estStartTime = m_pSimParam->GetStartTime();
	HTREEITEM hItem = NULL;
	CString strTimeString(_T(""));
	//start Day
	cni.nt=NT_NORMAL;
	cni.net = NET_EDITSPIN_WITH_VALUE;
	strTimeString.Format(_T("Start Day:   %d"), estStartTime.GetDay());
	m_hStartDay = m_coolTree.InsertItem(strTimeString,cni,FALSE,FALSE,m_hFlightFilter);
	m_coolTree.SetItemData( m_hStartDay, (DWORD)estStartTime.GetDay() );
	m_coolTree.SetItemValueRange(m_hStartDay,1,365);
	//start Time
	cni.nt=NT_NORMAL;
	cni.net = NET_DATETIMEPICKER;
	strTimeString.Format(_T("Time:   %02d:%02d:%02d"), estStartTime.GetHour(), estStartTime.GetMinute(), estStartTime.GetSecond());
	m_hStartTime=m_coolTree.InsertItem(strTimeString,cni,FALSE,FALSE,m_hFlightFilter);
	m_coolTree.SetItemData( m_hStartTime, (DWORD)estStartTime.asSeconds());
	//End Day
	cni.nt=NT_NORMAL;
	cni.net = NET_EDITSPIN_WITH_VALUE;
	ElapsedTime estEndTime = m_pSimParam->GetEndTime();
	strTimeString.Format(_T("End Day:   %d"), estEndTime.GetDay());
	m_hEndDay = m_coolTree.InsertItem(strTimeString,cni,FALSE,FALSE,m_hFlightFilter);
	m_coolTree.SetItemData( m_hEndDay, (DWORD)estEndTime.GetDay());
	m_coolTree.SetItemValueRange(m_hEndDay,1,365);
	//Time
	cni.nt=NT_NORMAL;
	cni.net = NET_DATETIMEPICKER;
	strTimeString.Format(_T("Time:   %02d:%02d:%02d"), estEndTime.GetHour(), estEndTime.GetMinute(), estEndTime.GetSecond());
	m_hEndTime=m_coolTree.InsertItem(strTimeString,cni,FALSE,FALSE,m_hFlightFilter);
	m_coolTree.SetItemData( m_hEndTime, (DWORD)estEndTime.asSeconds() );
	//Initialization Period:
	cni.nt=NT_NORMAL;
	cni.net = NET_DATETIMEPICKER;
	ElapsedTime estInitPeriod = m_pSimParam->GetInitPeriod();
	strTimeString.Format(_T("Initialization Period:   %02d:%02d:%02d"), estInitPeriod.GetHour(), estInitPeriod.GetMinute(), estInitPeriod.GetSecond());
	m_hInitialPeriodTime = m_coolTree.InsertItem(strTimeString, cni, FALSE, FALSE, m_hFlightFilter);
	m_coolTree.SetItemData( m_hInitialPeriodTime, (DWORD)m_pSimParam->GetInitPeriod().asSeconds() );
	//Follow Up Period:
	cni.nt=NT_NORMAL;
	cni.net = NET_DATETIMEPICKER;
	ElapsedTime estFollowUp = m_pSimParam->GetFollowupPeriod();
	strTimeString.Format(_T("Follow Up Period:   %02d:%02d:%02d"), estFollowUp.GetHour(), estFollowUp.GetMinute(), estFollowUp.GetSecond());
	m_hFollowUpTime = m_coolTree.InsertItem(strTimeString, cni, FALSE, FALSE, m_hFlightFilter);
	m_coolTree.SetItemData( m_hFollowUpTime, (DWORD)m_pSimParam->GetFollowupPeriod().asSeconds() );

	m_coolTree.Expand(m_hFlightFilter, TVE_EXPAND);
	//End add by adam 2007-09-27

//----------------------------------------------------------------------------------------------------
//terminal
	m_hTerminal = m_coolTree.InsertItem("Terminal",cni,FALSE);
		
	cni.nt=NT_CHECKBOX;
	cni.net = NET_EDITSPIN_WITH_VALUE;
	m_hMPCount=m_coolTree.InsertItem("Maximum Passenger Count:  30000",cni,FALSE,FALSE,m_hTerminal);
	m_coolTree.SetItemData( m_hMPCount,(DWORD) 30000 );
	m_coolTree.SetItemValueRange(m_hMPCount,1,300000);

	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;
	//General
	m_hNoPax = m_coolTree.InsertItem("Non Passenger",cni,TRUE,FALSE,m_hTerminal);
	//int iCount = GetInputTerminal()->m_pMobElemTypeStrDB->GetCount();
	for( int j=1; j<=NOPAX_COUNT; ++j )// 0 is pax
	{
		CString strType = GetInputTerminal()->m_pMobElemTypeStrDB->GetString( j );
		m_vAllNoPaxDetailItem.push_back( m_coolTree.InsertItem( strType, cni, TRUE, FALSE, m_hNoPax ) );
	}
	m_hBarrier =  m_coolTree.InsertItem("Barriers",cni,TRUE,FALSE,m_hTerminal);	
	m_hHub =  m_coolTree.InsertItem("Hubbing",cni,TRUE,FALSE,m_hTerminal);	
	//cni.nt=NT_NORMAL; 
	//cni.net=NET_EDITSPIN_WITH_VALUE;
	//m_hNuberOfRun = m_coolTree.InsertItem("Number Of Runs:  1",cni,TRUE,FALSE,m_hTerminal);	
	//m_coolTree.SetItemData( m_hNuberOfRun,(DWORD) 1 );
	//m_coolTree.SetItemValueRange(m_hNuberOfRun,1,100);
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;
	m_hCongestionImpact=m_coolTree.InsertItem("Congestion Impact",cni,FALSE,FALSE,m_hTerminal);

	//HTREEITEM hArea=m_coolTree.InsertItem("Area",cni,FALSE,FALSE,m_hCongestionImpact);
	//AddAreaToTree(hArea);
	//HTREEITEM hPipe=m_coolTree.InsertItem("Pipe",cni,FALSE,FALSE,m_hCongestionImpact);
	//AddPipeToTree(hPipe);
	//////////////////////////////////////////////////////////////////////////
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;
	m_hAvoidance = m_coolTree.InsertItem("Collision avoidance",cni,FALSE,FALSE,m_hTerminal) ;
	m_hArea_avoid=m_coolTree.InsertItem("Area",cni,FALSE,FALSE,m_hAvoidance);
	AddAreaToTree(m_hArea_avoid);
	m_hPipe_avoid=m_coolTree.InsertItem("Pipe",cni,FALSE,FALSE,m_hAvoidance);
	AddPipeToTree(m_hPipe_avoid);
	//////////////////////////////////////////////////////////////////////////
	//T395 Switch of processor & resource pool
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;

	CProcToResourceDataSet* pProc2ResDB = GetInputTerminal()->m_pProcToResourceDB;
	m_hProc2ResDB = m_coolTree.InsertItem("Processor & Resource Pool", cni, pProc2ResDB->getChecked(), FALSE, m_hTerminal);
	AddProc2ResItems(m_hProc2ResDB);
	//////////////////////////////////////////////////////////////////////////
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;
	cni.bAutoSetItemText=FALSE;

	m_hFireDrop=m_coolTree.InsertItem("Fire Drop",cni,FALSE,FALSE,m_hTerminal);
	m_hLocation=m_coolTree.InsertItem("Location",cni,FALSE,FALSE,m_hFireDrop);
	cni.nt=NT_NORMAL;
	cni.net=NET_COMBOBOX;
	m_coolTree.InsertItem("Floor:  [New Floor]",cni,FALSE,FALSE,m_hLocation);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	HTREEITEM hX=m_coolTree.InsertItem("X:  0",cni,FALSE,FALSE,m_hLocation);
	m_coolTree.SetItemData(hX,0);
	HTREEITEM hY=m_coolTree.InsertItem("Y:  0",cni,FALSE,FALSE,m_hLocation);
	m_coolTree.SetItemData(hY,0);
	cni.net=NET_NORMAL;
	m_hPropogationParams=m_coolTree.InsertItem("Propogation Parameters",cni,FALSE,FALSE,m_hFireDrop);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	HTREEITEM hA=m_coolTree.InsertItem("A:  0",cni,FALSE,FALSE,m_hPropogationParams);
	m_coolTree.SetItemData(hA,0);
	HTREEITEM hB=m_coolTree.InsertItem("B:  0",cni,FALSE,FALSE,m_hPropogationParams);
	m_coolTree.SetItemData(hB,0);
	HTREEITEM hC=m_coolTree.InsertItem("C:  0",cni,FALSE,FALSE,m_hPropogationParams);
	m_coolTree.SetItemData(hC,0);
	cni.bAutoSetItemText=TRUE;
	//Root
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;
	m_hOptional=m_coolTree.InsertItem("Optional",cni,FALSE, FALSE, m_hTerminal);
	cni.nt=NT_RADIOBTN;
	m_hAutoReport=m_coolTree.InsertItem("Auto Report",cni,TRUE,FALSE,m_hOptional);
	m_hTLOS=m_coolTree.InsertItem("Target Level of Service",cni,FALSE,FALSE,m_hOptional);
	cni.nt=NT_CHECKBOX;
	m_hAutoCreateProc = m_coolTree.InsertItem("Automatic Add Processor",cni,FALSE,FALSE,m_hTLOS);
	cni.nt=NT_NORMAL;
	m_hTLOSProcessor=m_coolTree.InsertItem("Processor",cni,FALSE,FALSE,m_hTLOS);
	//Auto Report
	m_hPassenger=m_coolTree.InsertItem("Passenger",cni,FALSE,FALSE,m_hAutoReport);
	m_hProcessors=m_coolTree.InsertItem("Processors",cni,FALSE,FALSE,m_hAutoReport);
	m_hSpace=m_coolTree.InsertItem("Space",cni,FALSE,FALSE,m_hAutoReport);
	cni.nt=NT_CHECKBOX;
	m_hAO=m_coolTree.InsertItem("Aircraft Operations",cni,FALSE,FALSE,m_hAutoReport);
	cni.nt=NT_NORMAL;
	m_hBaggage=m_coolTree.InsertItem("Baggage",cni,FALSE,FALSE,m_hAutoReport);
	cni.nt=NT_CHECKBOX;
	m_hEconomic=m_coolTree.InsertItem("Economic",cni,FALSE,FALSE,m_hAutoReport);

	//-------------------------------
	//Passenger
	m_hActivity = m_coolTree.InsertItem("Activity Log",cni,FALSE,FALSE,m_hPassenger);
	m_hDistance = m_coolTree.InsertItem("Distance Traveled",cni,FALSE,FALSE,m_hPassenger);
	m_hTimeInQue = 	m_coolTree.InsertItem("Time In Queues",cni,FALSE,FALSE,m_hPassenger);
	m_hTimeInTerm = m_coolTree.InsertItem("Time In Terminal",cni,FALSE,FALSE,m_hPassenger);
	m_hTimeInSer = m_coolTree.InsertItem("Time In Service",cni,FALSE,FALSE,m_hPassenger);

	m_hActivityBreakdown=m_coolTree.InsertItem("Activity Breakdown",cni,FALSE,FALSE,m_hPassenger);
	//Processors
	m_hUiti = m_coolTree.InsertItem("Utilization",cni,FALSE,FALSE,m_hProcessors);	
	m_hThroughtput = m_coolTree.InsertItem("Throughput",cni,FALSE,FALSE,m_hProcessors);	
	m_hQueueLen = m_coolTree.InsertItem("Queue Lengths",cni,FALSE,FALSE,m_hProcessors);
	m_hCriticalQuePara =  m_coolTree.InsertItem("Critical Queue Parameters",cni,FALSE,FALSE,m_hProcessors);
	//Space

	m_hOccupancy =  m_coolTree.InsertItem("Occupancy",cni,FALSE,FALSE,m_hSpace);
	m_hDensity = m_coolTree.InsertItem("Density",cni,FALSE,FALSE,m_hSpace);

	m_hSpaceThroutput = m_coolTree.InsertItem("Throughput",cni,FALSE,FALSE,m_hSpace);
	m_hCollision = m_coolTree.InsertItem("Collisions",cni,FALSE,FALSE,m_hSpace);
	//Baggage
	m_hBagCount = m_coolTree.InsertItem("Baggage Count",cni,FALSE,FALSE,m_hBaggage);
	m_hBagWaitTime=m_coolTree.InsertItem("Baggage Waiting Time",cni,FALSE,FALSE,m_hBaggage);
	m_hDistribution=m_coolTree.InsertItem("Distribution",cni,FALSE,FALSE,m_hBaggage);
	m_hDeliveryTime=m_coolTree.InsertItem("Delivery Times",cni,FALSE,FALSE,m_hBaggage);

	//Target Level of Service
	cni.bAutoSetItemText=TRUE;

//----------------------------------------------------------------------------------------------------
//airside
	cni.nt=NT_NORMAL; 
	cni.net = NET_NORMAL;
	m_hAirside  = m_coolTree.InsertItem("Airside",cni,FALSE);

	//add by Emily 2008.01.25
	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	m_hVehicles =  m_coolTree.InsertItem("Vehicle service",cni,TRUE,FALSE,m_hAirside);
	m_coolTree.Expand(m_hVehicles, TVE_EXPAND);
	
	//add by Colin 2011.04.21
	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	m_hCyclicGroundRoute =  m_coolTree.InsertItem("Allow Cyclic Ground Route",cni,TRUE,FALSE,m_hAirside);
	m_coolTree.Expand(m_hCyclicGroundRoute, TVE_EXPAND);

	//Itinerant flight
	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	m_hItinerantFlight =  m_coolTree.InsertItem("Itinerant Flight",cni,TRUE,FALSE,m_hAirside);

	//Training flight
	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	m_hTrainingFlight =  m_coolTree.InsertItem("Training Flight",cni,TRUE,FALSE,m_hAirside);



	//add by Peter 2007.10.25
	cni.nt=NT_NORMAL; 
	cni.net = NET_NORMAL;
	//cni.unMenuID = IDR_MENU_CLOSURE_AIRPORT;
	m_hRunwayClosures = m_coolTree.InsertItem(_T("Runway closures"),cni,FALSE,FALSE,m_hAirside);
	m_coolTree.Expand(m_hRunwayClosures, TVE_EXPAND);
	m_hTaxiwayClosures = m_coolTree.InsertItem(_T("Taxiway closures"),cni,FALSE,FALSE,m_hAirside);
	m_coolTree.Expand(m_hTaxiwayClosures, TVE_EXPAND);
	m_hStandGroupClosures = m_coolTree.InsertItem(_T("Stand closures"),cni,FALSE,FALSE,m_hAirside);
	m_coolTree.Expand(m_hStandGroupClosures, TVE_EXPAND);
//----------------------------------------------------------------------------------------------------
//landside
	cni.nt=NT_NORMAL; 
	cni.net = NET_NORMAL;
	m_hLandside  = m_coolTree.InsertItem("Landside",cni,FALSE);
	InitLandsideTree(cni);
//----------------------------------------------------------------------------------------------------
//Onboard
	cni.nt=NT_NORMAL; 
	cni.net = NET_NORMAL;
	m_hOnboard  = m_coolTree.InsertItem("Onboard",cni,FALSE);

	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	cni.unMenuID = 0;
	m_hBoardingTime =  m_coolTree.InsertItem("Apply boarding time to all flights of same flight type",cni,TRUE,FALSE,m_hOnboard);
	m_coolTree.Expand(m_hBoardingTime, TVE_EXPAND);

	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;	
	m_hDeplanementTime =  m_coolTree.InsertItem("Apply deplanement time to all flights of same flight type",cni,TRUE,FALSE,m_hOnboard);
	m_coolTree.Expand(m_hDeplanementTime, TVE_EXPAND);
	
	m_coolTree.Expand(m_hGeneral,TVE_EXPAND);

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if (pDoc&&pDoc->m_systemMode == EnvMode_Terminal) 
		m_coolTree.Expand(m_hTerminal,TVE_EXPAND);
	else if (pDoc&&pDoc->m_systemMode == EnvMode_AirSide)
		m_coolTree.Expand(m_hAirside,TVE_EXPAND);
	else if (pDoc&&pDoc->m_systemMode == EnvMode_LandSide)
		m_coolTree.Expand(m_hLandside,TVE_EXPAND);
	else if (pDoc&&pDoc->m_systemMode == EnvMode_OnBoard)
		m_coolTree.Expand(m_hOnboard,TVE_EXPAND);
	else
	{
		ASSERT(0);
	}

	m_coolTree.Expand(m_hOptional,TVE_EXPAND);
	m_coolTree.Expand(m_hMPCount,TVE_EXPAND);
	m_coolTree.Expand(m_hRandomSeed,TVE_EXPAND);
	m_coolTree.Expand(m_hNuberOfRun,TVE_EXPAND);
	m_coolTree.Expand(m_hNoPax,TVE_EXPAND);

	m_coolTree.SelectItem(m_hGeneral);
	m_coolTree.SelectSetFirstVisible(m_hGeneral);
}

LRESULT CSimEngSettingDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		{
			COOLTREE_NODE_INFO* pNodeInfo=(COOLTREE_NODE_INFO*)lParam;
			HTREEITEM hTreeItem=(HTREEITEM)wParam;
			if(pNodeInfo)
			{
				if (hTreeItem == m_hFlightFilter)
				{
					pNodeInfo->pEditWnd = m_pSimTimeRangeDlg;
				}	

				CRunwayClosureNode* pRunwayNode = 0;
				CTaxiwayClosureNode* pTaxiwayNode = 0;
				CStandGroupClosureNode * pStandGroupNode = 0;
				HTREEITEM hClosureItem = 0;
				DWORD dwData = m_coolTree.GetItemData(m_coolTree.GetParentItem(hTreeItem));
				hClosureItem = m_coolTree.GetParentItem(m_coolTree.GetParentItem(m_coolTree.GetParentItem(hTreeItem)));
				pRunwayNode = (CRunwayClosureNode* )dwData;
				pTaxiwayNode = (CTaxiwayClosureNode*)dwData;
				pStandGroupNode = (CStandGroupClosureNode*)dwData;
				
				if(hClosureItem == m_hRunwayClosures){
					pNodeInfo->pEditWnd = new CDlgTimeRange(pRunwayNode->m_startTime,pRunwayNode->m_endTime,FALSE, this);
				}else if(hClosureItem == m_hTaxiwayClosures){
					pNodeInfo->pEditWnd = new CDlgTimeRange(pTaxiwayNode->m_startTime,pTaxiwayNode->m_endTime, FALSE, this);
				}else if(hClosureItem == m_hStandGroupClosures){
					pNodeInfo->pEditWnd = new CDlgTimeRange(pStandGroupNode->m_startTime,pStandGroupNode->m_endTime,FALSE, this);
				}
			}
		}
		break;

	case UM_CEW_SHOW_DIALOGBOX_END:
		{
			COOLTREE_NODE_INFO* pNodeInfo=(COOLTREE_NODE_INFO*)lParam;
			HTREEITEM hTreeItem=(HTREEITEM)wParam;
			if(pNodeInfo)
			{
				if (hTreeItem == m_hFlightFilter)
				{
					LoadData();
				}	

				CRunwayClosureNode* pRunwayNode = 0;
				CTaxiwayClosureNode* pTaxiwayNode = 0;
				CStandGroupClosureNode * pStandGroupNode = 0;
				HTREEITEM hClosureItem = 0;
				DWORD dwData = m_coolTree.GetItemData(m_coolTree.GetParentItem(hTreeItem));
				hClosureItem = m_coolTree.GetParentItem(m_coolTree.GetParentItem(m_coolTree.GetParentItem(hTreeItem)));
				pRunwayNode = (CRunwayClosureNode* )dwData;
				pTaxiwayNode = (CTaxiwayClosureNode*)dwData;
				pStandGroupNode = (CStandGroupClosureNode*)dwData;
				CString strNodeTitle = _T("");

				if(hClosureItem == m_hRunwayClosures){
					pRunwayNode->m_startTime = ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetStartTime();
					pRunwayNode->m_endTime =  ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetEndTime();
					
					strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
						pRunwayNode->m_startTime.GetDay(),\
						pRunwayNode->m_startTime.GetHour(),\
						pRunwayNode->m_startTime.GetMinute(),\
						pRunwayNode->m_startTime.GetSecond(),\
						pRunwayNode->m_endTime.GetDay(),\
						pRunwayNode->m_endTime.GetHour(),\
						pRunwayNode->m_endTime.GetMinute(),\
						pRunwayNode->m_endTime.GetSecond());
					m_coolTree.SetItemText(hTreeItem,strNodeTitle);
				}else if(hClosureItem == m_hTaxiwayClosures){
					pTaxiwayNode->m_startTime = ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetStartTime();
					pTaxiwayNode->m_endTime =  ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetEndTime();

					strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
						pTaxiwayNode->m_startTime.GetDay(),\
						pTaxiwayNode->m_startTime.GetHour(),\
						pTaxiwayNode->m_startTime.GetMinute(),\
						pTaxiwayNode->m_startTime.GetSecond(),\
						pTaxiwayNode->m_endTime.GetDay(),\
						pTaxiwayNode->m_endTime.GetHour(),\
						pTaxiwayNode->m_endTime.GetMinute(),\
						pTaxiwayNode->m_endTime.GetSecond());
					m_coolTree.SetItemText(hTreeItem,strNodeTitle);
				}else if(hClosureItem == m_hStandGroupClosures){
					pStandGroupNode->m_startTime = ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetStartTime();
					pStandGroupNode->m_endTime =  ((CDlgTimeRange*)pNodeInfo->pEditWnd)->GetEndTime();

					strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
						pStandGroupNode->m_startTime.GetDay(),\
						pStandGroupNode->m_startTime.GetHour(),\
						pStandGroupNode->m_startTime.GetMinute(),\
						pStandGroupNode->m_startTime.GetSecond(),\
						pStandGroupNode->m_endTime.GetDay(),\
						pStandGroupNode->m_endTime.GetHour(),\
						pStandGroupNode->m_endTime.GetMinute(),\
						pStandGroupNode->m_endTime.GetSecond());
					m_coolTree.SetItemText(hTreeItem,strNodeTitle);
				}				

			}
		}
		break;
	case UM_CEW_COMBOBOX_BEGIN:
		{
			CWnd* pWnd=m_coolTree.GetEditWnd((HTREEITEM)wParam);
			CRect rectWnd;
			m_coolTree.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			CComboBox* pCB=(CComboBox*)pWnd;
			
			HTREEITEM hSel = m_coolTree.GetSelectedItem();
			CRunwayClosureNode* pRunwayNode = 0;
			CTaxiwayClosureNode* pTaxiwayNode = 0;
			HTREEITEM hClosureItem = 0;
			hClosureItem = m_coolTree.GetParentItem(m_coolTree.GetParentItem(m_coolTree.GetParentItem(hSel)));
			DWORD dwData = m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
			pRunwayNode = (CRunwayClosureNode* )dwData;
			pTaxiwayNode = (CTaxiwayClosureNode*)dwData;

			
			if(hClosureItem == m_hRunwayClosures ){
				pCB->ResetContent();
				Runway runway;
				runway.ReadObject(pRunwayNode->m_nRunwayID);
				IntersectionNodeList resltInterNode = runway.GetIntersectNodes();
				for (std::vector<IntersectionNode>::iterator itrInterNode = resltInterNode.begin();\
					itrInterNode != resltInterNode.end();++itrInterNode) {
						pCB->AddString((*itrInterNode).GetName());
						pCB->SetItemData(pCB->GetCount() - 1,(DWORD_PTR)(*itrInterNode).GetID());
				}
				return (0);
			}else if(hClosureItem == m_hTaxiwayClosures){
				pCB->ResetContent();
				Taxiway taxiway;
				taxiway.ReadObject(pTaxiwayNode->m_nTaxiwayID);
				IntersectionNodeList resltInterNode = taxiway.GetIntersectNodes();
				for (std::vector<IntersectionNode>::iterator itrInterNode = resltInterNode.begin();\
					itrInterNode != resltInterNode.end();++itrInterNode) {
						pCB->AddString((*itrInterNode).GetName());
						pCB->SetItemData(pCB->GetCount() - 1,(DWORD_PTR)(*itrInterNode).GetID());
				}
				return (0);
			}

			if(pCB->GetCount()==0)
			{
				CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
				int nCount = pDoc->GetCurModeFloor().m_vFloors.size();
				
				CString strFloor;
				for(int i=0;i<nCount;i++)
				{
					strFloor.Format("Floor %d",i+1);
					pCB->AddString(strFloor);
				}
			}			
		}
		break;
	case UM_CEW_COMBOBOX_END:
		{
			CWnd* pWnd=m_coolTree.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB=(CComboBox*)pWnd;
			int nInterID = pCB->GetItemData(pCB->GetCurSel());

			HTREEITEM hSel = (HTREEITEM)wParam;
			CRunwayClosureNode* pRunwayNode = 0;
			CTaxiwayClosureNode* pTaxiwayNode = 0;
			HTREEITEM hClosureItem = 0;
			hClosureItem = m_coolTree.GetParentItem(m_coolTree.GetParentItem(m_coolTree.GetParentItem(hSel)));
			DWORD dwData = m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
			pRunwayNode = (CRunwayClosureNode* )dwData;
			pTaxiwayNode = (CTaxiwayClosureNode*)dwData;

			if(hClosureItem != m_hRunwayClosures && hClosureItem != m_hTaxiwayClosures && hClosureItem != m_hStandGroupClosures){
				HTREEITEM hItem=(HTREEITEM)wParam;
				CString strText="Floor:  "+*(CString*)lParam;
				
				m_coolTree.SetItemText(hItem,strText);
			}
		}
		break;
	case UM_CEW_COMBOBOX_SELCHANGE:
		{
			CWnd* pWnd=m_coolTree.GetEditWnd((HTREEITEM)wParam);
			CComboBox* pCB=(CComboBox*)pWnd;
			int nInterID = pCB->GetItemData(pCB->GetCurSel());

			HTREEITEM hSel = (HTREEITEM)wParam;
			CRunwayClosureNode* pRunwayNode = 0;
			CTaxiwayClosureNode* pTaxiwayNode = 0;
			HTREEITEM hClosureItem = 0;
			hClosureItem = m_coolTree.GetParentItem(m_coolTree.GetParentItem(m_coolTree.GetParentItem(hSel)));
			DWORD dwData = m_coolTree.GetItemData(m_coolTree.GetParentItem(hSel));
			pRunwayNode = (CRunwayClosureNode* )dwData;
			pTaxiwayNode = (CTaxiwayClosureNode*)dwData;
			CString strNodeTitle = _T("");
			IntersectionNode interNode;

			if(hClosureItem == m_hRunwayClosures ){
				if(m_coolTree.GetNextItem(hSel,TVGN_PREVIOUS) && m_coolTree.GetNextItem(hSel,TVGN_NEXT)){
					if(nInterID == pRunwayNode->m_nIntersectNodeIDFrom){
						MessageBox(_T("Intersection node can't same."));
					}
					else{
						pRunwayNode->m_nIntersectNodeIDTo = nInterID;
					}
					interNode.ReadData(pRunwayNode->m_nIntersectNodeIDTo);						
					strNodeTitle.Format(_T("To:%s"),interNode.GetName());
					m_coolTree.SetItemText(hSel,strNodeTitle);
					return (0);
				}else{
					if(nInterID == pRunwayNode->m_nIntersectNodeIDTo){
						MessageBox(_T("Intersection node can't same."));
					}
					else{
						pRunwayNode->m_nIntersectNodeIDFrom = nInterID;
					}

					interNode.ReadData(pRunwayNode->m_nIntersectNodeIDFrom);						
					strNodeTitle.Format(_T("From:%s"),interNode.GetName());
					m_coolTree.SetItemText(hSel,strNodeTitle);
					return (0);
				}
			}else if(hClosureItem = m_hTaxiwayClosures){
				if(m_coolTree.GetNextItem(hSel,TVGN_PREVIOUS) && m_coolTree.GetNextItem(hSel,TVGN_NEXT)){
					if(nInterID == pTaxiwayNode->m_nIntersectNodeIDFrom){
						MessageBox(_T("Intersection node can't same."));
					}
					else{
						pTaxiwayNode->m_nIntersectNodeIDTo = nInterID;
					}
					interNode.ReadData(pTaxiwayNode->m_nIntersectNodeIDTo);						
					strNodeTitle.Format(_T("To:%s"),interNode.GetName());
					m_coolTree.SetItemText(hSel,strNodeTitle);
					return (0);
				}else{
					if(nInterID == pTaxiwayNode->m_nIntersectNodeIDTo){
						MessageBox(_T("Intersection node can't same."));
					}
					else{
						pTaxiwayNode->m_nIntersectNodeIDFrom = nInterID;						
					}
					interNode.ReadData(pTaxiwayNode->m_nIntersectNodeIDFrom);						
					strNodeTitle.Format(_T("From:%s"),interNode.GetName());
					m_coolTree.SetItemText(hSel,strNodeTitle);
					return (0);
				}
			}
		}
		break;
	case UM_CEW_SHOWMENU_BEGIN:
		{
			COOLTREE_NODE_INFO* pCNI=(COOLTREE_NODE_INFO*)lParam;
			HTREEITEM hItem=(HTREEITEM)wParam;
			if(pCNI)
			{
				HTREEITEM hParent = m_coolTree.GetParentItem(hItem);
				if( hItem == m_hTLOSProcessor
				 || hParent == m_hTLOSProcessor
				 || ( hParent && m_coolTree.GetParentItem(hParent) == m_hTLOSProcessor ) )
				{
					pCNI->unMenuID=IDR_SIMENG_COOLTREE;
				}	
			}

		}
		break;
		
	case UM_CEW_SHOWMENU_READY:
		{
			CMenu* pMenu=(CMenu*)lParam;
			if(pMenu)
			{	
				HTREEITEM hItem	  =(HTREEITEM)wParam;
				HTREEITEM hParent = m_coolTree.GetParentItem( hItem );
				if( hItem == m_hTLOSProcessor )
				{			
					DisableMenuItem(pMenu);
					pMenu->EnableMenuItem(ID_SIMENG_COOLTREE_POPUPMEN_ADDPROC,MF_ENABLED|MF_BYCOMMAND);
				}
				else if( hParent == m_hTLOSProcessor )
				{			
					DisableMenuItem(pMenu);
					pMenu->EnableMenuItem(ID_SIMENG_COOLTREE_POPUPMEN_ADDPAXTYPE,MF_ENABLED|MF_BYCOMMAND);
					pMenu->EnableMenuItem(ID_SIMENG_COOLTREE_POPUPMEN_DELPROC,MF_ENABLED|MF_BYCOMMAND);
				}
				else if( hParent && m_coolTree.GetParentItem( hParent) == m_hTLOSProcessor )
				{			
					DisableMenuItem(pMenu);
					pMenu->EnableMenuItem(ID_SIMENG_COOLTREE_POPUPMEN_DELPAXTYPE,MF_ENABLED|MF_BYCOMMAND);
				}
			}
		}
		break;
	case UM_CEW_SHOWMENU_END:
		break;
	case UM_CEW_EDITSPIN_END:
	case UM_CEW_EDIT_END:
		{	

			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_coolTree.GetItemNodeInfo(hItem);

			if( hItem ==  m_hRandomSeed ) 
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					m_pSimParam->SetSeed( 3 );
					m_coolTree.SetItemText( m_hRandomSeed, "Random Seed:  3" );
					m_coolTree.SetItemData( m_hRandomSeed, (DWORD)3);
				}
				else
				{
					m_pSimParam->SetSeed( atoi( strValue ) );			
					m_coolTree.SetItemText( m_hRandomSeed, "Random Seed:  "+strValue );
					m_coolTree.SetItemData( m_hRandomSeed, (DWORD)atoi( strValue ));
				}
				
			}
			else if (hItem == m_hStartDay)
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					//m_pSimParam->SetSeed( 3 );
					m_coolTree.SetItemText( m_hStartDay, "Start Day:  1" );
					m_coolTree.SetItemData( m_hStartDay, (DWORD)1);
				}
				else
				{
					//m_pSimParam->SetSeed( atoi( strValue ) );			
					m_coolTree.SetItemText( m_hStartDay, "Start Day:  "+strValue );
					m_coolTree.SetItemData( m_hStartDay, (DWORD)atoi( strValue ));
				}
			}
			else if (hItem == m_hEndDay)
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					//m_pSimParam->SetSeed( 3 );
					m_coolTree.SetItemText( m_hEndDay, "End Day:  1" );
					m_coolTree.SetItemData( m_hEndDay, (DWORD)1);
				}
				else
				{
					//m_pSimParam->SetSeed( atoi( strValue ) );			
					m_coolTree.SetItemText( m_hEndDay, "End Day:  "+strValue );
					m_coolTree.SetItemData( m_hEndDay, (DWORD)atoi( strValue ));
				}
			}
			else if (hItem == m_hStartTime)
			{
			}
			else if (hItem == m_hEndTime)
			{
			}
			else if( hItem ==  m_hMPCount ) 
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					m_pSimParam->SetPaxCount( 300000 );
					m_coolTree.SetItemText( m_hMPCount, "Maximum Passenger Count:  300000" );
					m_coolTree.SetItemData( m_hMPCount, (DWORD)300000 );
				}
				else
				{
					m_pSimParam->SetPaxCount( atoi( strValue ) );
					m_coolTree.SetItemText( m_hMPCount, "Maximum Passenger Count:  "+ strValue );
					m_coolTree.SetItemData( m_hMPCount, (DWORD)atoi( strValue ) );
				}
				
			}
			else if( hItem == m_hFlightFilterStartTime )
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					m_pSimParam->SetStartTime( ElapsedTime(0l) );
					m_coolTree.SetItemText( m_hFlightFilterStartTime, "Start Hours:  0" );
					m_coolTree.SetItemData( m_hFlightFilterStartTime,(DWORD)0);
				}
				else
				{
					int iValue = atoi( strValue ) ;				
					m_pSimParam->SetStartTime( ElapsedTime(iValue*3600l ) );
					m_coolTree.SetItemText( m_hFlightFilterStartTime, "Start Hours: "+strValue );
					m_coolTree.SetItemData( m_hFlightFilterStartTime,(DWORD)iValue);
				}
			}
			else if( hItem == m_hFlightFilterEndTime )
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					m_pSimParam->SetEndTime( ElapsedTime(3600*23+59*60+59l) );
					m_coolTree.SetItemText( m_hFlightFilterEndTime, "Start Hours:  24" );
					m_coolTree.SetItemData( m_hFlightFilterEndTime,(DWORD)24);
				}
				else
				{
					int iValue = atoi( strValue ) ;				
					m_pSimParam->SetEndTime( ElapsedTime(iValue*3600l ) );
					m_coolTree.SetItemText( m_hFlightFilterEndTime, "End Hours: "+strValue );
					m_coolTree.SetItemData( m_hFlightFilterEndTime,(DWORD)iValue);
				}
			}
			else if( hItem == m_hNuberOfRun ) 
			{
				CString strValue = *((CString*)lParam);
				if( strValue == "" )
				{
					m_pSimParam->GetSimPara()->SetNumberOfRun( 1 );
					m_coolTree.SetItemText( m_hNuberOfRun, "Number Of Runs:  1" );
					m_coolTree.SetItemData( m_hNuberOfRun,(DWORD)1);
				}
				else
				{
					int iValue = atoi( strValue ) ;
					if( m_bAutoCreateProc )
					{
						if( iValue != 1 )
						{
							m_pSimParam->GetSimPara()->SetNumberOfRun( 1 );
							m_coolTree.SetItemText( m_hNuberOfRun, "Number Of Runs:  1" );
							m_coolTree.SetItemData( m_hNuberOfRun,(DWORD)1);
							AfxMessageBox("Number of Runs must be 1 when Automatic add processor is checked !");
							return CDialog::DefWindowProc(message, wParam, lParam);
						}
					}
					m_pSimParam->GetSimPara()->SetNumberOfRun( iValue );
					m_coolTree.SetItemText( m_hNuberOfRun, "Number Of Runs:  "+ strValue );
					m_coolTree.SetItemData( m_hNuberOfRun,(DWORD)iValue);

					if( iValue > 1 )
					{
						m_coolTree.SetRadioStatus( m_TrueRandom );
						m_pSimParam->SetRandomFlag( 1 );
						m_pSimParam->GetSimPara()->SetRamdomSpeed( -1 );//true ramdom
						// set the randon seed num node disable
						m_coolTree.SetEnableStatus( m_hRandomSeed, FALSE );
					}
					else
					{
						// set the randon seed num node disable
						m_coolTree.SetEnableStatus( m_hRandomSeed, TRUE );
					}
				}
			}
			else if (hItem == m_hTimeStep)
			{
				CString strValue = *((CString*)lParam);
				long lTimeStep = atoi(strValue);
				if (m_pLandsideSimSetting)
				{
					m_pLandsideSimSetting->SetDefaultTimeStep(lTimeStep);
					CString strText;
					strText.Format(_T("Default: %d ms"),lTimeStep);
					pInfo->fMinValue = static_cast<float>(lTimeStep);
					m_coolTree.SetItemText(hItem,strText);
				}
			}
			else if (std::find(m_vParkingLot.begin(),m_vParkingLot.end(),hItem) != m_vParkingLot.end())
			{
				LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hItem);
				if (pLandsideClosure)
				{
					CString strValue = *((CString*)lParam);
					long lValue = atoi(strValue);
					pLandsideClosure->SetPercent(lValue);
					CString strNodeTitle = _T("");
					strNodeTitle.Format(_T("%s: %d%%"),pLandsideClosure->GetName().GetIDString(),lValue);
					pInfo->fMinValue = static_cast<float>(lValue);
					m_coolTree.SetItemText( hItem, strNodeTitle );
				}
			}
			else if( pInfo->net == NET_EDITSPIN_WITH_VALUE||pInfo->net==NET_EDIT_WITH_VALUE )
			{
				CString strValue = *((CString*)lParam);
				CString strItem = m_coolTree.GetItemText( hItem );
				int iPos = strItem.Find(':');
				strItem = strItem.Left(iPos+1);
				strItem +=CString("  ")+ strValue;
				m_coolTree.SetItemText( hItem, strItem );
				m_coolTree.SetItemData( hItem, (DWORD)atoi(strValue) );
			}	
			
			
			//else if()
			//{
			//....
			//}
			//if the item is clear, please put the judgement before this one
			
			else
			{//landisde time range step
				//check if the time step byond the time range
				HTREEITEM hParentItem = NULL;
				HTREEITEM hParParentItem = NULL;
				if(hItem != NULL)
					hParentItem =m_coolTree.GetParentItem(hItem);
				if(hParentItem)
					hParParentItem = m_coolTree.GetParentItem(hParentItem);
				if(hParParentItem == m_hTimeStepRangeRoot)
				{
					CString strValue = *((CString*)lParam);
					long lTimeStep = atoi(strValue);

					TimeStepRange *pTimeStepRange = (TimeStepRange *)m_coolTree.GetItemData(hItem);
					if (pTimeStepRange)
					{
						pTimeStepRange->setTimeStep(lTimeStep);
						CString strText;
						strText.Format(_T("Time step: %d ms"),lTimeStep);
						pInfo->fMinValue = static_cast<float>(lTimeStep);
						m_coolTree.SetItemText(hItem,strText);
					}
				}
			}
			break;
		}
	case UM_CEW_STATUS_CHANGE:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			if( hItem == m_hRandomSeed )
			{
				CString strValue = m_coolTree.GetItemText( m_coolTree.GetChildItem( hItem ) );
				if( strValue == "" )
				{
					m_pSimParam->SetSeed( 3 );
					m_coolTree.SetItemText( m_coolTree.GetChildItem( hItem ), "3" );
				}
				else
				{
					m_pSimParam->SetSeed( atoi( strValue ) );			
				}
			}
			else if( hItem == m_TrueRandom )
			{
				m_pSimParam->SetSeed( -1 );
			}
			else if( hItem == m_hMPCount )
			{
				if ( m_coolTree.IsCheckItem( hItem ) == 1 )
				{
					CString strValue = m_coolTree.GetItemText( m_coolTree.GetChildItem( hItem ) );
					if( strValue == "" )
					{
						m_pSimParam->SetPaxCount( 30000 );
						m_coolTree.SetItemText( m_coolTree.GetChildItem( hItem ), "30000" );
					}
					else
					{
						m_pSimParam->SetPaxCount( atoi( strValue ) );
					}
					
				}
				else
				{
					m_pSimParam->SetPaxCount( -1 );
				}				
			}
			else if( hItem == m_hNoPax )
			{
				m_pSimParam->SetVisitorsFlag( m_coolTree.IsCheckItem( hItem ) == 1 ? 1 :0 );
			}
			else if( hItem == m_hBarrier )
			{
				m_pSimParam->SetBarrierFlag( m_coolTree.IsCheckItem( hItem ) == 1 ? 1 :0 );
			}
			else if( hItem == m_hHub )
			{
				m_pSimParam->SetHubbingFlag( m_coolTree.IsCheckItem( hItem ) == 1 ? 1 :0 );
			}
			else if (hItem == m_hCongestionImpact)
			{
				m_pSimParam->SetCongestionFlag(m_coolTree.IsCheckItem( hItem ) == 1 ? 1 :0 );
			}
			else if( hItem == m_hAutoCreateProc )
			{
				m_bAutoCreateProc = m_coolTree.IsCheckItem( hItem ) == 1 ? TRUE :FALSE ;
				
				if( m_bAutoCreateProc )
				{
					m_coolTree.SetItemText( m_hNuberOfRun, "Number Of Runs:  1" );
					m_coolTree.SetItemData( m_hNuberOfRun,(DWORD)1);
					m_pSimParam->GetSimPara()->SetNumberOfRun( 1 );
				}
				
			}
			else if(hItem == m_hAvoidance)
			{
				m_pSimParam->SetCollisionFlag(m_coolTree.IsCheckItem(hItem) == 1 ? 1:0);
			}
			else if (hItem == m_hArea_avoid)
			{
				m_pSimParam->SetAreaFlag(m_coolTree.IsCheckItem(hItem) == 1 ? 1:0);
			}
			else if (hItem == m_hPipe_avoid)
			{
				m_pSimParam->SetPipeFlag(m_coolTree.IsCheckItem(hItem) == 1 ? 1:0);
			}
			else if ( HandleNoPaxDetailFlags( hItem ) )
			{
					
			}
			else if(std::find(m_vAreaItem.begin(),m_vAreaItem.end(),hItem) != m_vAreaItem.end())
			{
				HandleAreaItem(hItem);
			}
			else if (std::find(m_vPipeItem.begin(),m_vPipeItem.end(),hItem) != m_vPipeItem.end())
			{
				HandlePipeItem(hItem);
			}
			else if(hItem == m_hProc2ResDB)
			{
				GetInputTerminal()->m_pProcToResourceDB->setChecked(m_coolTree.IsCheckItem(hItem));
			}
			else if(m_coolTree.GetParentItem(hItem) == m_hProc2ResDB)
			{
				CProcToResource* pProc2Res = reinterpret_cast<CProcToResource*>(m_coolTree.GetItemData(hItem));
				if(pProc2Res != NULL)
				{
					pProc2Res->setChecked(m_coolTree.IsCheckItem(hItem));
				}
			}
		}
		break;
	case UM_CEW_DATETIME_END:
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			long lSecondTime = (long)lParam;
			CString strText(_T(""));

			ElapsedTime estTime(lSecondTime);

			if (hItem == m_hStartTime)
			{
				strText.Format(_T("Time:   %02d:%02d:%02d"), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
				m_coolTree.SetItemText(hItem, strText);
				m_coolTree.SetItemData(hItem, estTime.asSeconds());
			}
			else if (hItem == m_hEndTime)
			{
				strText.Format(_T("Time:   %02d:%02d:%02d"), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
				m_coolTree.SetItemText(hItem, strText);
				m_coolTree.SetItemData(hItem, estTime.asSeconds());
			}
			else if (hItem == m_hInitialPeriodTime)
			{
				strText.Format(_T("Initialization Period:   %02d:%02d:%02d"), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
				m_coolTree.SetItemText(hItem, strText);
				m_coolTree.SetItemData(hItem, estTime.asSeconds());
			}
			else if (hItem == m_hFollowUpTime)
			{
				strText.Format(_T("Follow Up Period:   %02d:%02d:%02d"), estTime.GetHour(), estTime.GetMinute(), estTime.GetSecond());
				m_coolTree.SetItemText(hItem, strText);
				m_coolTree.SetItemData(hItem, estTime.asSeconds());
			}
		}
		break;
	case WM_LBUTTONDBLCLK:
		{
			OnLButtonDblClk(wParam,lParam);
			return 0;
		}
		break;
	default:
		break;
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}

bool CSimEngSettingDlg::HandleNoPaxDetailFlags(HTREEITEM _hItem )
{
	int iCount  = m_vAllNoPaxDetailItem.size();
	for( int i=0; i<iCount; ++i )
	{
		if( _hItem == m_vAllNoPaxDetailItem[ i ] )
		{
			m_pSimParam->GetNoPaxDetailFlag().at( i ) = m_coolTree.IsCheckItem( _hItem ) == 1 ? 1 :0;
			return true;
		}		
	}	
	return false;
}

void CSimEngSettingDlg::SetTreeData()
{
	
}

void CSimEngSettingDlg::OnSimengCooltreePopupmenAddproc() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem=m_coolTree.GetSelectedItem();
	if( hItem != m_hTLOSProcessor )
		return;

	CProcesserDialog dlg(GetInputTerminal());
	if(dlg.DoModal()==IDOK)
	{
		ProcessorID pid;
		ProcessorIDList idList;
		dlg.GetProcessorIDList(idList);
		int nIdCount = idList.getCount();
		for(int i =0 ;i < nIdCount;i++ )
		{
			pid = *idList[i];
			bool bError = FALSE;
			///check if all processor have queue
			GroupIndex group = GetInputTerminal()->procList->getGroupIndex( pid );
			for( int i=group.start; i<=group.end; i++ )
			{
				Processor* pProc = GetInputTerminal()->procList->getProcessor( i );
				if( pProc->GetProcessorQueue() == NULL )
				{
					CString strMsg = "Not all processor have queue,so can not add the processor!";
					MessageBox( strMsg, "Warning", MB_OK|MB_ICONINFORMATION );
					bError = TRUE;
					//return;					
					break;
				}
			}
			//////////////////////////////////////////////////////////////////////////
			if ( !bError )
			{
				CString strProcID;
				strProcID=pid.GetIDString();
				
				COOLTREE_NODE_INFO cni;
				CCoolTree::InitNodeInfo(this,cni);
				m_coolTree.InsertItem(strProcID,cni,FALSE,FALSE,hItem);
				m_coolTree.Invalidate();				
			}
		}
	}
}

void CSimEngSettingDlg::OnSimengCooltreePopupmenAddpaxtype() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	HTREEITEM hParent = m_coolTree.GetParentItem( hItem );
	if( hParent != m_hTLOSProcessor )
		return;
	

	CPassengerTypeDialog dlg(m_pParent);
	if(dlg.DoModal()==IDOK)
	{
		CMobileElemConstraint mec(GetInputTerminal());
		//char buf[256];
		CString buf;
		dlg.GetMobileSelection().screenPrint( buf );
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);
		HTREEITEM hChildItem = m_coolTree.InsertItem(buf,cni,FALSE,FALSE,hItem);
		//////////////////////////////////////////////////////////////////////////
		CMobileElemConstraint* pMob = new CMobileElemConstraint(GetInputTerminal());
		*pMob = dlg.GetMobileSelection();
		pMob->SetInputTerminal( GetInputTerminal() );
		m_coolTree.SetItemData( hChildItem, (DWORD)pMob );
		//////////////////////////////////////////////////////////////////////////
		InsertAllTypeChildItem( hChildItem );
		m_coolTree.Invalidate();
	}
}

void CSimEngSettingDlg::OnSimengCooltreePopupmenDelproc()
{
	HTREEITEM hItem=m_coolTree.GetSelectedItem();
	HTREEITEM hParent = m_coolTree.GetParentItem( hItem );
	if( hParent != m_hTLOSProcessor )
		return;

	HTREEITEM hPaxItem = m_coolTree.GetChildItem( hItem );
	while( hPaxItem )
	{
		CMobileElemConstraint* pMob = (CMobileElemConstraint*)m_coolTree.GetItemData( hPaxItem );	
		assert(pMob);
		delete pMob;
		
		hPaxItem = m_coolTree.GetNextSiblingItem( hPaxItem );
	}
	
	m_coolTree.DeleteItem( hItem );
	m_coolTree.Invalidate();
}

void CSimEngSettingDlg::OnSimengCooltreePopupmenDelpaxtype()
{
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	HTREEITEM hParent = m_coolTree.GetParentItem( m_coolTree.GetParentItem( hItem) );
	if( hParent != m_hTLOSProcessor )
		return;
	
	CMobileElemConstraint* pMob = (CMobileElemConstraint*) m_coolTree.GetItemData( hItem );
	if( pMob )
		delete pMob;

	m_coolTree.DeleteItem( hItem );
	m_coolTree.Invalidate();
}	

void CSimEngSettingDlg::DisableMenuItem(CMenu *pMenu)
{
	int nCount=pMenu->GetMenuItemCount();
	for(int i=0;i<nCount;i++)
	{
		pMenu->EnableMenuItem(i,MF_GRAYED|MF_BYPOSITION);
	}
}

void CSimEngSettingDlg::InsertAllTypeChildItem(HTREEITEM hItem,CString* pStrQLU,CString* pStrQLL,
											   CString* pStrQWSU,CString* pStrQWSL,
											   CString* pStrAN,CString* pStrDAU,CString* pStrDAL)
{
	HTREEITEM hTmpItem;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_NORMAL;
	cni.net=NET_NORMAL;
	HTREEITEM hAllTypeMQL=m_coolTree.InsertItem("Queue Length",cni,FALSE,FALSE,hItem);
	HTREEITEM hAllTypeMQWT=m_coolTree.InsertItem("Queue Waiting Time(mins)",cni,FALSE,FALSE,hItem);
//	HTREEITEM hAllTypeMQD=m_coolTree.InsertItem("Queue Density",cni,FALSE,FALSE,hItem);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.bAutoSetItemText=FALSE;
	if(pStrQLU==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strUpper + ":  100",cni,FALSE,FALSE,hAllTypeMQL);
		m_coolTree.SetItemData( hTmpItem,(DWORD)100 );
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strUpper +":  "+ *pStrQLU,cni,FALSE,FALSE,hAllTypeMQL);
		m_coolTree.SetItemData( hTmpItem,(DWORD) atoi(*pStrQLU) );
	}

	if(pStrQLL==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  10",cni,FALSE,FALSE,hAllTypeMQL);
		m_coolTree.SetItemData(hTmpItem, (DWORD)10 );
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  "+ *pStrQLL,cni,FALSE,FALSE,hAllTypeMQL);
		m_coolTree.SetItemData( hTmpItem, (DWORD) atoi(*pStrQLL) );
	}

	if(pStrQWSU==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strUpper + ":  60",cni,FALSE,FALSE,hAllTypeMQWT);
		m_coolTree.SetItemData( hTmpItem, (DWORD) 60 );
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strUpper + ":  "+*pStrQWSU,cni,FALSE,FALSE,hAllTypeMQWT);
		m_coolTree.SetItemData( hTmpItem, (DWORD) atoi(*pStrQWSU));
	}

	if(pStrQWSL==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  10",cni,FALSE,FALSE,hAllTypeMQWT);
		m_coolTree.SetItemData( hTmpItem, (DWORD) 10);
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  "+*pStrQWSL,cni,FALSE,FALSE,hAllTypeMQWT);
		m_coolTree.SetItemData( hTmpItem, (DWORD) atoi(*pStrQWSL));
	}

/*
	cni.nt=NT_CHECKBOX;
	cni.net=NET_NORMAL;
	cni.bAutoSetItemText=TRUE;
	HTREEITEM hAllTypeMQDArea=m_coolTree.InsertItem("Area",cni,FALSE,FALSE,hAllTypeMQD);
	//HTREEITEM hAllTypeMQDValue=m_coolTree.InsertItem("Value",cni,FALSE,FALSE,hAllTypeMQD);
	AddAreaToTree(hAllTypeMQDArea);

	cni.nt=NT_NORMAL;
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.bAutoSetItemText=FALSE;
	if(pStrDAU==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strUpper + ":  100",cni,FALSE,FALSE,hAllTypeMQD);
		m_coolTree.SetItemData( hTmpItem, (DWORD) 100);
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strUpper + ":  "+*pStrDAU,cni,FALSE,FALSE,hAllTypeMQD);
		m_coolTree.SetItemData( hTmpItem, (DWORD) atoi(*pStrDAU));
	}

	if(pStrDAL==NULL)
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  10",cni,FALSE,FALSE,hAllTypeMQD);
		m_coolTree.SetItemData( hTmpItem, (DWORD) 10);
	}
	else
	{
		hTmpItem = m_coolTree.InsertItem(strLower + ":  "+*pStrDAL,cni,FALSE,FALSE,hAllTypeMQD);
		m_coolTree.SetItemData( hTmpItem, (DWORD) atoi(*pStrDAL));
	}
*/
}

bool CSimEngSettingDlg::SetTargetLevel(CSimParameter* pSP)
{
	CMobileElemConstraint mob(GetInputTerminal());
	
	CSimGeneralPara* pSGP=pSP->GetSimPara();
	CSimLevelOfService* pSLOS=new CSimLevelOfService( pSGP->GetStartDate() );
	pSLOS->SetMaxCount(pSGP->GetMaxCount());
	pSLOS->SetNoPaxDetailFlag(pSGP->GetNoPaxDetailFlag());
	pSLOS->SetNumberOfRun(pSGP->GetNumberOfRun());
	pSLOS->SetRamdomSpeed(pSGP->GetRamdomSpeed());
	pSLOS->EnableBarriers(pSGP->IfBarriersEnable());
	pSLOS->EnableHubbing(pSGP->IfHubbingEnable());
	pSLOS->EnableCongestionImpact(pSGP->IfCongestionImpactEnable());
	pSLOS->EnableNoPax(pSGP->IfNoPaxEnable());

	pSLOS->SetStartTime(pSGP->GetStartTime());
	pSLOS->SetEndTime(pSGP->GetEndTime());
	pSLOS->SetFollowupPeriod(pSGP->GetFollowupPeriod());
	pSLOS->SetInitPeriod(pSGP->GetInitPeriod());

	
	HTREEITEM hProcItem = m_coolTree.GetChildItem( m_hTLOSProcessor );
	while( hProcItem )
	{
		CSimLevelOfServiceItem item;
		CString strProcName = m_coolTree.GetItemText( hProcItem );
		item.setProcessorName( strProcName );
		HTREEITEM hPaxItem = m_coolTree.GetChildItem( hProcItem );
		if( hPaxItem == NULL )
		{
			CString strMsg;
			strMsg.Format("You must add some passenger type after processor %s",strProcName );
			MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );

			delete pSLOS;	
			return false;
		}
		std::vector<Mob_ServicTime>	mapSPTS;
		while( hPaxItem )
		{
			CMobileElemConstraint* pMob = (CMobileElemConstraint*)m_coolTree.GetItemData( hPaxItem );	
			assert(pMob);
			mob = * pMob;
			mob.SetInputTerminal( GetInputTerminal() );

			SERVICE_ITEMS si;
			if( !GetMaxValueFromTreeItem( hPaxItem, si ) )
			{
				CString strMsg = "The Upper Value must more than Lower Value!";
				MessageBox( strMsg, "ERROR", MB_OK|MB_ICONERROR );
				
				delete pSLOS;				
				return false;
			}
			
			Mob_ServicTime mob_st;
			mob_st.first = mob;
			mob_st.second = si;
			mapSPTS.push_back( mob_st );
			
			hPaxItem = m_coolTree.GetNextSiblingItem( hPaxItem );
		}
		item.setSpecificPaxTypeService( mapSPTS );

		pSLOS->addSettingItem( item );
		hProcItem = m_coolTree.GetNextSiblingItem( hProcItem );
	}

	pSP->clear();
	pSLOS->setDynamicCreateProcFlag( m_bAutoCreateProc );
	pSP->SetSimPara(pSLOS);
	return true;
}

void CSimEngSettingDlg::SetAutoReport(CSimParameter *pSP)
{
	CSimGeneralPara* pSGP=pSP->GetSimPara();
	CSimAutoReportPara* pSARP=new CSimAutoReportPara( pSGP->GetStartDate() );
	pSARP->SetMaxCount(pSGP->GetMaxCount());
	pSARP->SetNoPaxDetailFlag(pSGP->GetNoPaxDetailFlag());
	pSARP->SetNumberOfRun(pSGP->GetNumberOfRun());
	pSARP->SetRamdomSpeed(pSGP->GetRamdomSpeed());
	pSARP->EnableBarriers(pSGP->IfBarriersEnable());
	pSARP->EnableHubbing(pSGP->IfHubbingEnable());
	pSARP->EnableNoPax(pSGP->IfNoPaxEnable());
	
	pSARP->SetStartTime(pSGP->GetStartTime());
	pSARP->SetEndTime(pSGP->GetEndTime());
	pSARP->SetInitPeriod(pSGP->GetInitPeriod());
	pSARP->SetFollowupPeriod(pSGP->GetFollowupPeriod());

	if(m_coolTree.IsCheckItem(m_hActivity)==1)
		pSARP->AddReport( ENUM_PAXLOG_REP );
	if(m_coolTree.IsCheckItem(m_hDistance)==1)
		pSARP->AddReport( ENUM_DISTANCE_REP );
	if(m_coolTree.IsCheckItem(m_hTimeInQue)==1)
		pSARP->AddReport( ENUM_QUEUETIME_REP );
	if(m_coolTree.IsCheckItem(m_hTimeInTerm)==1)
		pSARP->AddReport( ENUM_DURATION_REP );
	if(m_coolTree.IsCheckItem(m_hTimeInSer)==1)
		pSARP->AddReport( ENUM_SERVICETIME_REP );
	if(m_coolTree.IsCheckItem(m_hActivityBreakdown)==1)
		pSARP->AddReport( ENUM_ACTIVEBKDOWN_REP );
	if(m_coolTree.IsCheckItem(m_hUiti)==1)
		pSARP->AddReport( ENUM_UTILIZATION_REP );
	if(m_coolTree.IsCheckItem(m_hThroughtput)==1)
		pSARP->AddReport( ENUM_THROUGHPUT_REP );
	if(m_coolTree.IsCheckItem(m_hQueueLen)==1)
		pSARP->AddReport( ENUM_QUEUELENGTH_REP );
	if(m_coolTree.IsCheckItem(m_hCriticalQuePara)==1)
		pSARP->AddReport( ENUM_AVGQUEUELENGTH_REP );
	if(m_coolTree.IsCheckItem(m_hOccupancy)==1)
		pSARP->AddReport(ENUM_PAXCOUNT_REP);
	if(m_coolTree.IsCheckItem(m_hDensity)==1)
		pSARP->AddReport(ENUM_PAXDENS_REP);
	if(m_coolTree.IsCheckItem(m_hSpaceThroutput)==1)
		pSARP->AddReport(ENUM_SPACETHROUGHPUT_REP);
	if(m_coolTree.IsCheckItem(m_hCollision)==1)
		pSARP->AddReport(ENUM_COLLISIONS_REP);
	if(m_coolTree.IsCheckItem(m_hAO)==1)
		pSARP->AddReport(ENUM_ACOPERATION_REP);
	if(m_coolTree.IsCheckItem(m_hBagCount)==1)
		pSARP->AddReport(ENUM_BAGCOUNT_REP);
	if(m_coolTree.IsCheckItem(m_hBagWaitTime)==1)
		pSARP->AddReport(ENUM_BAGWAITTIME_REP);
	if(m_coolTree.IsCheckItem(m_hDistribution)==1)
		pSARP->AddReport(ENUM_BAGDISTRIBUTION_REP);
	if(m_coolTree.IsCheckItem(m_hDeliveryTime)==1)
		pSARP->AddReport(ENUM_BAGDELIVTIME_REP);
	if(m_coolTree.IsCheckItem(m_hEconomic)==1)
		pSARP->AddReport(ENUM_ECONOMIC_REP);
	
	pSP->clear();
	pSP->SetSimPara(pSARP);
}

void CSimEngSettingDlg::SetGeneralPara( CSimParameter* pSP )
{
	CSimGeneralPara* pSGP=pSP->GetSimPara();
	CSimGeneralPara* pSARP=new CSimGeneralPara( pSGP->GetStartDate() );
	pSARP->SetMaxCount(pSGP->GetMaxCount());
	pSARP->SetNoPaxDetailFlag(pSGP->GetNoPaxDetailFlag());
	pSARP->SetPipeFlag(pSGP->GetPipeFlag());
	pSARP->SetAreaFlag(pSGP->GetAreaFlag());
	pSARP->SetNumberOfRun(pSGP->GetNumberOfRun());
	pSARP->SetRamdomSpeed(pSGP->GetRamdomSpeed());
	pSARP->EnableBarriers(pSGP->IfBarriersEnable());
	pSARP->EnableHubbing(pSGP->IfHubbingEnable());
	pSARP->EnableCongestionImpact(pSGP->IfCongestionImpactEnable());
	pSARP->EnableCollision(pSGP->IfCollisionEnable());
	pSARP->EnableArea(pSGP->IfAreaEnable());
	pSARP->EnablePipe(pSGP->IfPipeEnable());
	pSARP->EnableNoPax(pSGP->IfNoPaxEnable());
	

	pSARP->SetStartTime(pSGP->GetStartTime());
	pSARP->SetEndTime(pSGP->GetEndTime());
	pSARP->SetInitPeriod(pSGP->GetInitPeriod());
	pSARP->SetFollowupPeriod(pSGP->GetFollowupPeriod());
	
	pSP->clear();
	pSP->SetSimPara(pSARP);
}

bool CSimEngSettingDlg::GetMaxValueFromTreeItem(HTREEITEM hItem,int& nQLU,int& nQLL,long& lQWSU,long& lQWSL,CString& strAN,int& nADU,int& nADL)
{
	HTREEITEM hChild=m_coolTree.GetChildItem(hItem);
	HTREEITEM hTempItem;
	int nIndex=0;
	while(hChild)
	{
		switch(nIndex)
		{
		case 0:
			hTempItem = m_coolTree.GetChildItem(hChild);
			//nQLU=atoi(m_coolTree.GetItemText(hTempItem));
			nQLU=m_coolTree.GetItemData(hTempItem);
			hTempItem = m_coolTree.GetNextSiblingItem( hTempItem );
			//nQLL=atoi(m_coolTree.GetItemText(hTempItem));
			nQLL=m_coolTree.GetItemData(hTempItem);
			if( nQLU<nQLL )
				return false;
			break;
		case 1:
			hTempItem = m_coolTree.GetChildItem(hChild);
			//lQWSU=atol(m_coolTree.GetItemText(hTempItem));
			lQWSU= m_coolTree.GetItemData(hTempItem);
			hTempItem = m_coolTree.GetNextSiblingItem( hTempItem );
			//lQWSL=atol(m_coolTree.GetItemText(hTempItem)); 
			lQWSL= m_coolTree.GetItemData(hTempItem); 
			if( lQWSU<lQWSL )
				return false;
			break;
		case 2:
			{
				hTempItem = m_coolTree.GetChildItem(hChild);
				strAN=m_coolTree.GetItemText(m_coolTree.GetChildItem(hTempItem));
				
				hTempItem = m_coolTree.GetNextSiblingItem( hTempItem );
				//nADU = atoi(m_coolTree.GetItemText( hTempItem ));
				nADU = m_coolTree.GetItemData( hTempItem );
				
				hTempItem = m_coolTree.GetNextSiblingItem( hTempItem );
				//nADL = atoi(m_coolTree.GetItemText( hTempItem ));
				nADL = m_coolTree.GetItemData( hTempItem );
				if( nADU<nADL )
					return false;
				/*************************************************
				HTREEITEM hChild2=m_coolTree.GetChildItem(hChild);
				int nIndex2=0;
				while(hChild2)
				{
					switch(nIndex2)
					{
					case 0:
						strAN=m_coolTree.GetItemText(m_coolTree.GetChildItem(hChild2));
						break;
					case 1:
						nAD=atoi(m_coolTree.GetItemText(m_coolTree.GetChildItem(hChild2)));
						break;
					}
					hChild2=m_coolTree.GetNextSiblingItem(hChild2);
					nIndex2++;	
				}
				*************************************************/
			}
			break;
		}
		hChild=m_coolTree.GetNextSiblingItem(hChild);
		nIndex++;
	}

	return true;
}

bool CSimEngSettingDlg::GetMaxValueFromTreeItem(HTREEITEM hItem, SERVICE_ITEMS &SI)
{
	return GetMaxValueFromTreeItem(hItem,SI.m_iUpperQueueLength,SI.m_iLowerQueueLength,
							SI.m_lUpperQueueWaitingSeconds,SI.m_lLowerQueueWaitingSeconds,
							SI.m_strAreaName,SI.m_iUpperAreaDensity, SI.m_iLowerAreaDensity);
}


void CSimEngSettingDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_btnOk.m_hWnd == NULL )
		return;
	
	CRect rcWnd;
	// 1. move button
	m_btnCancel.GetWindowRect( &rcWnd );
	m_btnCancel.MoveWindow( cx-15-rcWnd.Width(), cy-15-rcWnd.Height(), rcWnd.Width(), rcWnd.Height(), TRUE );
	m_btnOk.MoveWindow( cx-15-2*rcWnd.Width()-10, cy-15-rcWnd.Height(), rcWnd.Width(), rcWnd.Height(), TRUE );
	// 2. move cool tree
	m_coolTree.MoveWindow( 15,15,cx-2*15, cy-15-15-rcWnd.Height()-10 ,TRUE );

	InvalidateRect( NULL );
}

void CSimEngSettingDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 450;
	lpMMI->ptMinTrackSize.y = 300;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CSimEngSettingDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

void CSimEngSettingDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if( pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl)))
		{
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);		
	}

	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);
}

void CSimEngSettingDlg::clearMobConstraintPointer()
{
	HTREEITEM hProcItem = m_coolTree.GetChildItem( m_hTLOSProcessor );
	while( hProcItem )
	{
		HTREEITEM hPaxItem = m_coolTree.GetChildItem( hProcItem );
		while( hPaxItem )
		{
			CMobileElemConstraint* pMob = (CMobileElemConstraint*)m_coolTree.GetItemData( hPaxItem );	
			assert(pMob);
			delete pMob;

			hPaxItem = m_coolTree.GetNextSiblingItem( hPaxItem );
		}

		hProcItem = m_coolTree.GetNextSiblingItem( hProcItem );
	}
	
}

void CSimEngSettingDlg::AddAreaToTree(HTREEITEM hItem)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_CHECKBOX;
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	int nCount=pDoc->GetTerminal().m_pAreas->m_vAreas.size();
	for(int i=0;i<nCount;i++)
	{
		HTREEITEM hAreaItem = m_coolTree.InsertItem(pDoc->GetTerminal().m_pAreas->m_vAreas[i]->name,cni,FALSE,FALSE,hItem);
		m_pSimParam->SetAreaItemFlag(pDoc->GetTerminal().m_pAreas->m_vAreas[i]->name,m_pSimParam->GetAreaItemFlag(pDoc->GetTerminal().m_pAreas->m_vAreas[i]->name));
		m_vAreaItem.push_back(hAreaItem);
	}
}

void CSimEngSettingDlg::AddPipeToTree(HTREEITEM hItem)
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CPipeDataSet* pPipeDataSet=pDoc->GetTerminal().m_pPipeDataSet;
	int nPipeCount	= pPipeDataSet->GetPipeCount();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt=NT_CHECKBOX;
	
	for(int i=0; i<nPipeCount; i++ )
	{
		HTREEITEM hPipeItem = m_coolTree.InsertItem(pPipeDataSet->GetPipeAt(i)->GetPipeName(),cni,FALSE,FALSE,hItem);
		m_pSimParam->SetPipeItemFlag(pPipeDataSet->GetPipeAt(i)->GetPipeName(),m_pSimParam->GetPipeItemFlag(pPipeDataSet->GetPipeAt(i)->GetPipeName()));
		m_vPipeItem.push_back(hPipeItem);
	}
	
}

void CSimEngSettingDlg::HandleAreaItem(HTREEITEM hItem)
{
	m_pSimParam->SetAreaItemFlag(m_coolTree.GetItemText(hItem),m_coolTree.IsCheckItem(hItem));
}

void CSimEngSettingDlg::HandlePipeItem(HTREEITEM hItem)
{	
	m_pSimParam->SetPipeItemFlag(m_coolTree.GetItemText(hItem),m_coolTree.IsCheckItem(hItem));
}

void CSimEngSettingDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CSimEngSettingDlg::OnAddLandsideClosure()
{
	if (m_pLandsideSimSetting == NULL)
		return;

	LandsideFacilityLayoutObjectList* pLandsideObjectList = GetLandsideObjectList();
	if (pLandsideObjectList == NULL)
		return;

	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	if (hItem == m_hStrecthClos)
	{
		CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LSTRETCH,m_pParentWnd);
		if (dlg.DoModal() == IDOK)
		{
			LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetStretchClosure();
			LandsideClosure* pLandsideClosure = new LandsideClosure();
			pLandsideClosure->SetType(LandsideClosure::STRETCH_CLOSURE);
			pLandsideClosure->SetName(dlg.getALTObjectID());

			AddLandsideClosureItem(hItem,pLandsideClosure);
			pLandsideClosureList->AddItem(pLandsideClosure);
		}
	}
	else if(hItem == m_hIntersectionClos)
	{
		CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LINTERSECTION,m_pParentWnd);
		if (dlg.DoModal() == IDOK)
		{
			LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetIntersectionClosure();
			LandsideClosure* pLandsideClosure = new LandsideClosure();
			pLandsideClosure->SetName(dlg.getALTObjectID());
			pLandsideClosure->SetType(LandsideClosure::INTERSECTION_CLOSURE);

			AddLandsideClosureItem(hItem,pLandsideClosure);
			pLandsideClosureList->AddItem(pLandsideClosure);
		}
	}
	else if (hItem == m_hRoundClos)
	{
		CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LROUNDABOUT,m_pParentWnd);
		if (dlg.DoModal() == IDOK)
		{
			LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetRoundaboutClosure();
			LandsideClosure* pLandsideClosure = new LandsideClosure();
			pLandsideClosure->SetName(dlg.getALTObjectID());
			pLandsideClosure->SetType(LandsideClosure::ROUNDABOUT_CLOSURE);

			AddLandsideClosureItem(hItem,pLandsideClosure);
			pLandsideClosureList->AddItem(pLandsideClosure);
		}
	}
	else if (hItem == m_hParkinglotClos)
	{
		CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LPARKINGLOT,m_pParentWnd);
		if (dlg.DoModal() == IDOK)
		{
			LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetParkingLotClosure();
			LandsideClosure* pLandsideClosure = new LandsideClosure();
			pLandsideClosure->SetName(dlg.getALTObjectID());
			pLandsideClosure->SetType(LandsideClosure::PARKINGLOT_CLOSURE);

			AddLandsideClosureItem(hItem,pLandsideClosure);
			pLandsideClosureList->AddItem(pLandsideClosure);
		}
	}
	else if (hItem == m_hNameStretchClos)
	{
		CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LSTRETCHSEGMENT,m_pParentWnd);
		if (dlg.DoModal() == IDOK)
		{
			LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetNamedStretchSegmentClosure();
			LandsideClosure* pLandsideClosure = new LandsideClosure();
			pLandsideClosure->SetName(dlg.getALTObjectID());
			pLandsideClosure->SetType(LandsideClosure::NAMEDSTRETCHSEGMENT_CLOSURE);

			AddLandsideClosureItem(hItem,pLandsideClosure);
			pLandsideClosureList->AddItem(pLandsideClosure);
		}
	}
}

void CSimEngSettingDlg::OnDeleteLandsideClosure()
{
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	HTREEITEM hParent = m_coolTree.GetParentItem(hItem);
	LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hItem);
	if (hParent == m_hStrecthClos)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetStretchClosure();
		pLandsideClosureList->DeleteItem(pLandsideClosure);
		m_coolTree.DeleteItem(hItem);
	}
	else if(hParent == m_hIntersectionClos)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetIntersectionClosure();
		pLandsideClosureList->DeleteItem(pLandsideClosure);
		m_coolTree.DeleteItem(hItem);
	}
	else if (hParent == m_hRoundClos)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetRoundaboutClosure();
		pLandsideClosureList->DeleteItem(pLandsideClosure);
		m_coolTree.DeleteItem(hItem);
	}
	else if (hParent == m_hParkinglotClos)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetParkingLotClosure();
		pLandsideClosureList->DeleteItem(pLandsideClosure);
		m_coolTree.DeleteItem(hItem);
	}
	else if (hParent == m_hNameStretchClos)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetNamedStretchSegmentClosure();
		pLandsideClosureList->DeleteItem(pLandsideClosure);
		m_coolTree.DeleteItem(hItem);
	}
}

void CSimEngSettingDlg::OnAddParkingLot()
{
	LandsideFacilityLayoutObjectList* pLandsideObjectList = GetLandsideObjectList();
	if (pLandsideObjectList == NULL)
		return;
	
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	if (m_pLandsideSimSetting == NULL)
		return;

	CDlgLandsideObjectSelect dlg(pLandsideObjectList,ALT_LPARKINGLOT,m_pParentWnd);
	if (dlg.DoModal() == IDOK)
	{
		LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetParkingLotClosure();
		LandsideClosure* pLandsideClosure = new LandsideClosure();
		pLandsideClosure->SetName(dlg.getALTObjectID());
		pLandsideClosure->SetType(LandsideClosure::PARKINGLOT_CLOSURE);

		AddParkinglotClosureItem(hItem,pLandsideClosure);
		pLandsideClosureList->AddItem(pLandsideClosure);
	}
	

}

void CSimEngSettingDlg::OnDeleteParkingLot()
{
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hItem);
	LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetParkingLotClosure();
	pLandsideClosureList->DeleteItem(pLandsideClosure);
	m_coolTree.DeleteItem(hItem);
	std::vector<HTREEITEM>::iterator iter = std::find(m_vParkingLot.begin(),m_vParkingLot.end(),hItem);
	if (iter != m_vParkingLot.end())
	{
		m_vParkingLot.erase(iter);
	}
}

LandsideFacilityLayoutObjectList* CSimEngSettingDlg::GetLandsideObjectList()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	if(pDoc == NULL)
		return NULL;
	
	CARCPort* pArcport = pDoc->getARCport();
	if (pArcport == NULL)
		return NULL;

	InputLandside* pInputLandside = pArcport->GetInputLandside();
	if (pInputLandside == NULL)
		return NULL;
	
	return &(pInputLandside->getObjectList());
}

void CSimEngSettingDlg::AddLandsideClosureItem( HTREEITEM hItem, LandsideClosure* pLandsideClosure )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);	

	CString strNodeTitle;
	strNodeTitle = pLandsideClosure->GetName().GetIDString();

	cni.net = NET_NORMAL;
	cni.unMenuID = IDR_MENU_DELCLOSURE;
	HTREEITEM hParkingLot = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hItem);
	m_coolTree.SetItemData(hParkingLot,(DWORD_PTR)pLandsideClosure); 
	for (int i = 0; i < pLandsideClosure->GetCount(); i++)
	{
		LandsideTimeRange* pTimeRange = pLandsideClosure->GetItem(i);

		AddTimeRangeItem(hParkingLot,pTimeRange);
	}
	m_coolTree.Expand(hItem,TVE_EXPAND);
}

void CSimEngSettingDlg::OnLButtonDblClk( WPARAM wParam, LPARAM lParam )
{
	UINT uFlags;
	CPoint point = *(CPoint*)lParam;
	HTREEITEM hItem = m_coolTree.HitTest(point, &uFlags);

	if (std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem) != m_vTimeRange.end())
	{
		LandsideTimeRange* pTimeRange = (LandsideTimeRange*)m_coolTree.GetItemData(hItem);
		if (pTimeRange == NULL)
			return;
		CDlgTimeRange dlg(pTimeRange->GetStartTime(),pTimeRange->GetEndTime(),FALSE, this);
		if (dlg.DoModal() == IDOK)
		{
			pTimeRange->SetStartTime(dlg.GetStartTime());
			pTimeRange->SetEndTime(dlg.GetEndTime());

			CString strNodeTitle;
			strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
				pTimeRange->GetStartTime().GetDay(),\
				pTimeRange->GetStartTime().GetHour(),\
				pTimeRange->GetStartTime().GetMinute(),\
				pTimeRange->GetStartTime().GetSecond(),\
				pTimeRange->GetEndTime().GetDay(),\
				pTimeRange->GetEndTime().GetHour(),\
				pTimeRange->GetEndTime().GetMinute(),\
				pTimeRange->GetEndTime().GetSecond());

			m_coolTree.SetItemText(hItem,strNodeTitle);
		}
	}

	//check if the landside time step - time range item
	HTREEITEM hParentItem = m_coolTree.GetParentItem(hItem);
	if(hParentItem == m_hTimeStepRangeRoot)
	{

		TimeStepRange* pTimeStepRange = (TimeStepRange*)m_coolTree.GetItemData(hItem);
		if (pTimeStepRange == NULL)
			return;
		CDlgTimeRange dlg(pTimeStepRange->getTimeRange().GetStartTime(),pTimeStepRange->getTimeRange().GetEndTime(),FALSE, this);
		if (dlg.DoModal() == IDOK)
		{
			pTimeStepRange->setTimeRage(dlg.GetStartTime(),dlg.GetEndTime());

			CString strNodeTitle;
			strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
				pTimeStepRange->getTimeRange().GetStartTime().GetDay(),\
				pTimeStepRange->getTimeRange().GetStartTime().GetHour(),\
				pTimeStepRange->getTimeRange().GetStartTime().GetMinute(),\
				pTimeStepRange->getTimeRange().GetStartTime().GetSecond(),\
				pTimeStepRange->getTimeRange().GetEndTime().GetDay(),\
				pTimeStepRange->getTimeRange().GetEndTime().GetHour(),\
				pTimeStepRange->getTimeRange().GetEndTime().GetMinute(),\
				pTimeStepRange->getTimeRange().GetEndTime().GetSecond());

			m_coolTree.SetItemText(hItem,strNodeTitle);
		}
	}

}

void CSimEngSettingDlg::AddTimeRangeItem( HTREEITEM hItem,LandsideTimeRange* pTimeRange )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	cni.net = NET_NORMAL;
	cni.unMenuID = IDR_MENU_DELTIMERANGE;

	CString strNodeTitle;
	strNodeTitle.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
		pTimeRange->GetStartTime().GetDay(),\
		pTimeRange->GetStartTime().GetHour(),\
		pTimeRange->GetStartTime().GetMinute(),\
		pTimeRange->GetStartTime().GetSecond(),\
		pTimeRange->GetEndTime().GetDay(),\
		pTimeRange->GetEndTime().GetHour(),\
		pTimeRange->GetEndTime().GetMinute(),\
		pTimeRange->GetEndTime().GetSecond());
	HTREEITEM hTimeRange = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hItem);
	m_vTimeRange.push_back(hTimeRange);
	m_coolTree.SetItemData(hTimeRange,(DWORD_PTR)pTimeRange);

	m_coolTree.Expand(hItem,TVE_EXPAND);
}

void CSimEngSettingDlg::DeleteTimeRangeItem(HTREEITEM hItem,LandsideTimeRange* pTimeRange)
{
	HTREEITEM hClosureItem = m_coolTree.GetParentItem(hItem);
	if (hClosureItem)
	{
		LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hClosureItem);
		if (pLandsideClosure == NULL)
			return;
		
		pLandsideClosure->DeleteItem(pTimeRange);
		m_coolTree.DeleteItem(hItem);
		std::vector<HTREEITEM>::iterator iter = std::find(m_vTimeRange.begin(),m_vTimeRange.end(),hItem);
		if (iter != m_vTimeRange.end())
		{
			m_vTimeRange.erase(iter);
		}
		
	}
}

void CSimEngSettingDlg::OnAddLandsideTimeRange()
{
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	if (hItem == NULL)
		return;
	
	LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hItem);
	if (pLandsideClosure == NULL)
		return;
	
	LandsideTimeRange* pTimeRange = new LandsideTimeRange;
	CDlgTimeRange dlg(pTimeRange->GetStartTime(),pTimeRange->GetEndTime(), FALSE, this);
	if (dlg.DoModal() != IDOK)
		return;

	pTimeRange->SetStartTime(dlg.GetStartTime());
	pTimeRange->SetEndTime(dlg.GetEndTime());

	if (!pLandsideClosure->availableTimeRange(pTimeRange))
	{
		MessageBox(_T("Time range is invalid"),_T("Warning"));
		delete pTimeRange;
		return;
	}
	AddTimeRangeItem(hItem,pTimeRange);
	pLandsideClosure->AddItem(pTimeRange);
}

void CSimEngSettingDlg::OnDeleteLandsideTimeRange()
{
	HTREEITEM hItem = m_coolTree.GetSelectedItem();
	if (hItem == NULL)
		return;

	LandsideTimeRange* pTimeRange = (LandsideTimeRange*)m_coolTree.GetItemData(hItem);
	if (pTimeRange == NULL)
		return;
	
	HTREEITEM hClosureItem = m_coolTree.GetParentItem(hItem);
	if (hClosureItem == NULL)
		return;
	
	LandsideClosure* pLandsideClosure = (LandsideClosure*)m_coolTree.GetItemData(hClosureItem);
	if (pLandsideClosure == NULL)
		return;

	pLandsideClosure->DeleteItem(pTimeRange);
	m_coolTree.DeleteItem(hItem);



}

void CSimEngSettingDlg::AddParkinglotClosureItem( HTREEITEM hItem, LandsideClosure* pLandsideClosure )
{
	LandsideClosureList* pLandsideClosureList = m_pLandsideSimSetting->GetParkingLotClosure();

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);	

	CString strNodeTitle = _T("");
	strNodeTitle.Format(_T("%s: %d%%"),pLandsideClosure->GetName().GetIDString(),pLandsideClosure->GetPercent());

	cni.nt = NT_NORMAL;
	cni.net=NET_EDIT_INT;
	cni.unMenuID = IDR_MENU_DELPARKINGLOT;
	cni.fMinValue = static_cast<float>(pLandsideClosure->GetPercent());
	HTREEITEM hParkingLot = m_coolTree.InsertItem(strNodeTitle,cni,FALSE,FALSE,hItem);
	m_coolTree.SetItemData(hParkingLot,(DWORD_PTR)pLandsideClosure); 
	m_coolTree.Expand(hItem,TVE_EXPAND);
	m_vParkingLot.push_back(hParkingLot);
}

void CSimEngSettingDlg::LoadLandsideData()
{
	if (m_pLandsideSimSetting)
	{
		delete m_pLandsideSimSetting;
		m_pLandsideSimSetting = NULL;
	}
	m_pLandsideSimSetting = new LandsideSimSetting;
	m_pLandsideSimSetting->ReadData();
}

void CSimEngSettingDlg::AddLandisdeTimeStep( TimeStepRange *pTimeRange )
{
	if(pTimeRange == NULL)
		return;

	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);	

	const TimeRange & tRange = pTimeRange->getTimeRange();
	CString strTimeRange;
	strTimeRange.Format("TimeRange:Day%d %02d:%02d:%02d - Day%d %02d:%02d:%02d",\
		tRange.GetStartTime().GetDay(),\
		tRange.GetStartTime().GetHour(),\
		tRange.GetStartTime().GetMinute(),\
		tRange.GetStartTime().GetSecond(),\
		tRange.GetEndTime().GetDay(),\
		tRange.GetEndTime().GetHour(),\
		tRange.GetEndTime().GetMinute(),\
		tRange.GetEndTime().GetSecond());

	cni.nt = NT_NORMAL;
	cni.net = NET_SHOW_DIALOGBOX;
	cni.unMenuID = IDR_MENU_LSSIMSETTINGTIMESTEP_DEL;
	HTREEITEM hRangeItem = m_coolTree.InsertItem(strTimeRange, cni, FALSE, FALSE, m_hTimeStepRangeRoot);
	m_coolTree.SetItemData(hRangeItem, (DWORD_PTR)pTimeRange);

	//insert time step
	cni.nt = NT_NORMAL;
	cni.net = NET_EDIT_INT;
	cni.unMenuID = 0;
	cni.fMinValue = static_cast<float>(pTimeRange->getTimeStep());
	CString strTimeStep;
	strTimeStep.Format(_T("Time Step: %d ms"), pTimeRange->getTimeStep());
	HTREEITEM hStepItem = m_coolTree.InsertItem(strTimeStep, cni, FALSE, FALSE, hRangeItem);
	m_coolTree.SetItemData(hStepItem, (DWORD_PTR)pTimeRange);

	m_coolTree.Expand(hRangeItem, TVE_EXPAND);

}

void CSimEngSettingDlg::RemoveLandsideTineStep( HTREEITEM hTimeRangItem )
{
	//`m_pLandsideSimSetting->GetTimeStepRangeList()->DeleteData();

	//current select item
	HTREEITEM hSelectedItem = m_coolTree.GetSelectedItem();
	if (hSelectedItem == NULL)
		return;

	//validate the item
	HTREEITEM hParentItem = m_coolTree.GetParentItem(hSelectedItem);
	if(hParentItem != m_hTimeStepRangeRoot)
		return;

	//delete the item

	TimeStepRange* pTimeStepRange = (TimeStepRange*)m_coolTree.GetItemData(hTimeRangItem);
	if (pTimeStepRange == NULL)
		return;

	m_pLandsideSimSetting->GetTimeStepRangeList()->DeleteItem(pTimeStepRange);
	m_coolTree.DeleteItem(hSelectedItem);


}

//from menu
void CSimEngSettingDlg::OnAddLandsideTimeStepRange()
{
	//show time range dialog
	HTREEITEM hSelectedItem = m_coolTree.GetSelectedItem();
	if (hSelectedItem == NULL)
		return;

	if(hSelectedItem != m_hTimeStepRangeRoot)
		return;

	//Time range list
	TimeStepRangeList* pTimeRangeList = m_pLandsideSimSetting->GetTimeStepRangeList();





	CDlgTimeRange dlg( ElapsedTime(0L), ElapsedTime(24*60*60L -1), FALSE, this);
	if (dlg.DoModal() != IDOK)
		return;

	TimeStepRange* pTimeStepRange = new TimeStepRange;
	pTimeStepRange->setTimeRage(dlg.GetStartTime(), dlg.GetEndTime());

	if (m_pLandsideSimSetting->IsTimeRangeOverlapped(pTimeStepRange))
	{
		MessageBox(_T("Time range is overlapped"),_T("Warning"));
		delete pTimeStepRange;
		return;
	}
	m_pLandsideSimSetting->GetTimeStepRangeList()->AddItem(pTimeStepRange);

	//show in tree
	AddLandisdeTimeStep(pTimeStepRange);
	m_coolTree.Expand(m_hTimeStepRangeRoot, TVE_EXPAND);

}

void CSimEngSettingDlg::OnDelLandsideTimeStepRange()
{
	//show time range dialog
	HTREEITEM hSelectedItem = m_coolTree.GetSelectedItem();
	if (hSelectedItem == NULL)
		return;
	RemoveLandsideTineStep(hSelectedItem);
}

void CSimEngSettingDlg::AddProc2ResItems(HTREEITEM hProc2ResDB)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.net=NET_NORMAL;
	cni.nt=NT_CHECKBOX;

	CProcToResourceDataSet* pProc2ResDB = GetInputTerminal()->m_pProcToResourceDB;
	PROC2RESSET& proc2ResList = pProc2ResDB->getProc2ResList();

	PROC2RESSET::iterator itor = proc2ResList.begin();
	HTREEITEM hProc2Res = NULL;
	for(; itor!= proc2ResList.end(); itor++)
	{
		CString strPoolName = itor->getProcessorID().GetIDString();
		hProc2Res = m_coolTree.InsertItem(strPoolName, cni, itor->getChecked(), FALSE, hProc2ResDB);
		m_coolTree.SetItemData(hProc2Res, reinterpret_cast<DWORD>(&(*itor)));
	}
	int nCount = pProc2ResDB->getProc2ResListSize();
}
