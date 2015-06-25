// DlgEngineParameters.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgEngineParameters.h"
#include "../Common/ProbDistManager.h"
#include "../Common/AirportDatabase.h"
#include "../Inputs/IN_TERM.H"
#include "../Common/WinMsg.h"


// CDlgEngineParameters dialog

IMPLEMENT_DYNAMIC(CDlgEngineParameters, CXTResizeDialog)
CDlgEngineParameters::CDlgEngineParameters(InputAirside * pInputAirside,CAirportDatabase* pAirportDB,int nProjID,PFuncSelectFlightType pSelectFlightType,PSelectProbDistEntry m_pSelectProbDistEntry,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgEngineParameters::IDD, pParent)
	,m_pInputAirside(pInputAirside)
	,m_nPorjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
	,m_pSelectProbDistEntry(m_pSelectProbDistEntry)
	,m_engineParametersList(pAirportDB)
{
}

CDlgEngineParameters::~CDlgEngineParameters()
{
}

void CDlgEngineParameters::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgEngineParameters, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnSelChangePassengerType)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_DATA,OnEndlabeledit)
	ON_COMMAND(ID_TOOLBAR_RUNWAY_ADD,OnAddPaxType)
	ON_COMMAND(ID_TOOLBAR_RUNWAY_DEL,OnRemovePaxType)
	ON_COMMAND(ID_TOOLBAR_RUNWAY_EDIT,OnEditPaxType)
	ON_MESSAGE(WM_NOEDIT_DBCLICK, OnDBClick)
	ON_BN_CLICKED(IDC_BUTTON_SAVE,OnSave)
END_MESSAGE_MAP()


// CDlgEngineParameters message handlers
BOOL CDlgEngineParameters::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_engineParametersList.ReadData(m_nPorjID);
	OnInitToolbar();
	OnUpdateToolbar();

	//initialize unit manager
	CUnitPiece::InitializeUnitPiece(m_nPorjID,UM_ID_23,this);
	CRect rectItem;
	GetDlgItem(IDOK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);

	OnInitListCtrl();

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

int CDlgEngineParameters::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP|CBRS_TOOLTIPS)||
		!m_wndToolbar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}

void CDlgEngineParameters::OnInitToolbar()
{
	CRect rect;
	m_wndListCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.bottom = rect.top - 6;
	rect.top = rect.bottom - 22;
	rect.left = rect.left + 2;
	m_wndToolbar.MoveWindow(&rect,true);
	m_wndToolbar.ShowWindow(SW_SHOW);

	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,TRUE);
	m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,TRUE);
}

void CDlgEngineParameters::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_engineParametersList.SaveData(m_nPorjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	
	CDialog::OnOK();
}

void CDlgEngineParameters::DisplayData()
{
	m_wndListCtrl.DeleteAllItems();
	int nCount = m_engineParametersList.getCount();
	for (int i = 0; i < nCount; i++)
	{
		CEngineParametersItem* pItem = m_engineParametersList.getItem(i);
		char szFltType[1024] = {0};
		pItem->GetFltType().screenPrint(szFltType);
		m_wndListCtrl.InsertItem(i,szFltType);
		CString strValue = _T("");
		strValue.Format(_T("%s"),pItem->getStartTime().getPrintDist());
		m_wndListCtrl.SetItemText(i,1,strValue);
		strValue.Format(_T("%d"),(int)pItem->getJetBlastAngle());
		m_wndListCtrl.SetItemText(i,2,strValue);
		strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceStart())));
		m_wndListCtrl.SetItemText(i,3,strValue);
		strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceTaxi())));
		m_wndListCtrl.SetItemText(i,4,strValue);
		strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceTakeoff())));
		m_wndListCtrl.SetItemText(i,5,strValue);
		m_wndListCtrl.SetItemData(i,(DWORD_PTR)pItem);
	}
}

void CDlgEngineParameters::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_engineParametersList.SaveData(m_nPorjID);
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
}

void CDlgEngineParameters::OnUpdateToolbar()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,TRUE);
	}
	else
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_ADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_DEL,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_RUNWAY_EDIT,FALSE);
	}
}

void CDlgEngineParameters::OnSelChangePassengerType(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	OnUpdateToolbar();
}

void CDlgEngineParameters::OnInitListCtrl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	char* columnLabel[] = {	"Flight Type","Starting time (secs)", "Jetblast angle (deg)","Jetblast distance start","Jetblast distance taxi",\
		"Jetblast distance take-off"};
	int DefaultColumnWidth[] = { 150,130,120,160,160,160};

	int nColFormat[] = 
	{	
		LVCFMT_NOEDIT,
			LVCFMT_DROPDOWN,
			LVCFMT_EDIT,
			LVCFMT_EDIT,
			LVCFMT_EDIT,
			LVCFMT_EDIT
	};

	CString strStart = _T("");
	strStart.Format(_T("%s (%s)"),columnLabel[3],CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	columnLabel[3] = strStart.GetBuffer();

	CString strTaxi = _T("");
	strTaxi.Format(_T("%s (%s)"),columnLabel[4],CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	columnLabel[4] = strTaxi.GetBuffer();

	CString strTakeOff = _T("");
	strTakeOff.Format(_T("%s (%s)"),columnLabel[5],CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	columnLabel[5] = strTakeOff.GetBuffer();

	CStringList strList;
	CProbDistManager* pProbDistMan = 0;	
	CProbDistEntry* pPDEntry = 0;
	int nCount = -1;
	if(m_pInputAirside)
		pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	strList.AddTail(_T("New Probability Distribution..."));
	if(pProbDistMan)
		nCount = pProbDistMan->getCount();
	for( int i=0; i< nCount; i++ )
	{
		pPDEntry = pProbDistMan->getItem( i );
		if(pPDEntry)
			strList.AddTail(pPDEntry->m_csName);
	}

	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	for (int i = 0; i < 6; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.csList = &strList;
		m_wndListCtrl.InsertColumn(i, &lvc);
	}
	DisplayData();
}

void CDlgEngineParameters::OnAddPaxType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType;

	if( !(*m_pSelectFlightType)(NULL,fltType))
		return;


	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	int nCount = m_engineParametersList.getCount();
	for (int i=0; i<nCount; i++)
	{
		CEngineParametersItem* pData = m_engineParametersList.getItem(i);
		char szFltType[1024] = {0};
		pData->GetFltType().screenPrint(szFltType);
		if (!strcmp(szBuffer,szFltType))
		{
			MessageBox(_T("The flight Type already exists!"));

			return;
		}
	}	
	CEngineParametersItem* pItem = new CEngineParametersItem();
	pItem->SetFltType(fltType);
	m_engineParametersList.addItem(pItem);
	nCount = m_wndListCtrl.GetItemCount();

	CString strValue = _T("");
	char szFltType[1024] = {0};
	pItem->GetFltType().screenPrint(szFltType);
	m_wndListCtrl.InsertItem(nCount,szFltType);
	strValue.Format(_T("%s"),pItem->getStartTime().getPrintDist());
	m_wndListCtrl.SetItemText(nCount,1,strValue);
	strValue.Format(_T("%d"),pItem->getJetBlastAngle());
	m_wndListCtrl.SetItemText(nCount,2,strValue);
	strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceStart())));
	m_wndListCtrl.SetItemText(nCount,3,strValue);
	strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceTaxi())));
	m_wndListCtrl.SetItemText(nCount,4,strValue);
	strValue.Format(_T("%d"),(int)(CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),(double)pItem->getJetBlastDistanceTakeoff())));
	m_wndListCtrl.SetItemText(nCount,5,strValue);
	m_wndListCtrl.SetItemData(nCount,(DWORD_PTR)pItem);
	m_wndListCtrl.SetItemState( nCount,LVIS_SELECTED,LVIS_SELECTED );
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

void CDlgEngineParameters::OnRemovePaxType()
{
	int nSel = m_wndListCtrl.GetCurSel();
	if (nSel != LB_ERR)
	{
		CEngineParametersItem* pItem = (CEngineParametersItem*)m_wndListCtrl.GetItemData(nSel);
		m_engineParametersList.removeItem(pItem);
		m_wndListCtrl.DeleteItem(nSel);
		GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	}
}

void CDlgEngineParameters::OnEditPaxType()
{
	if (m_pSelectFlightType == NULL)
		return;

	FlightConstraint fltType;

	if( !(*m_pSelectFlightType)(NULL,fltType))
		return;


	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	int nItem = m_wndListCtrl.GetCurSel();
	int nCount = m_engineParametersList.getCount();
	CEngineParametersItem* pItem = (CEngineParametersItem*)m_wndListCtrl.GetItemData(nItem);
	for (int i=0; i<nCount; i++)
	{
		CEngineParametersItem* pData = m_engineParametersList.getItem(i);
		char szFltType[1024] = {0};
		pData->GetFltType().screenPrint(szFltType);
		if (!strcmp(szBuffer,szFltType) && pItem != pData)
		{
			MessageBox(_T("The Flight Type already exists!"));

			return;
		}
	}

	pItem->SetFltType(fltType);
	DisplayData();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

LRESULT CDlgEngineParameters::OnDBClick(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	if (m_pSelectFlightType == NULL)
		return FALSE;

	FlightConstraint fltType;

	if( !(*m_pSelectFlightType)(NULL,fltType))
		return FALSE;


	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);

	int nItem = m_wndListCtrl.GetCurSel();
	int nCount = m_engineParametersList.getCount();
	CEngineParametersItem* pItem = (CEngineParametersItem*)m_wndListCtrl.GetItemData(nItem);
	for (int i=0; i<nCount; i++)
	{
		CEngineParametersItem* pData = m_engineParametersList.getItem(i);
		char szFltType[1024] = {0};
		pItem->GetFltType().screenPrint(szFltType);
		if (!strcmp(szBuffer,szFltType) && pItem != pData)
		{
			MessageBox(_T("The Flight Type already exists!"));

			return FALSE;
		}
	}

	pItem->SetFltType(fltType);
	DisplayData();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
	return TRUE;
}

void CDlgEngineParameters::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* dispinfo = (LV_DISPINFO * )pNMHDR;
	*pResult = 0;

	CEngineParametersItem* pItem = (CEngineParametersItem*)m_wndListCtrl.GetItemData(dispinfo->item.iItem);
	if(!pItem)return;
	CString strSel;
	strSel = dispinfo->item.pszText;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistManager* pProbDistMan = m_pInputAirside->m_pAirportDB->getProbDistMan();
	switch(dispinfo->item.iSubItem)
	{
	case 1:
		{
			if (strSel == _T("New Probability Distribution..." ) )
			{
				CProbDistEntry* pPDEntry = NULL;
				pPDEntry = (*m_pSelectProbDistEntry)(NULL,m_pInputAirside);
				if(pPDEntry)
				{
					pItem->setStartTime(pPDEntry);
					m_wndListCtrl.SetItemText(dispinfo->item.iItem,dispinfo->item.iSubItem,pPDEntry->m_csName);
				}
			}
			else
			{
				CProbDistEntry* pPDEntry = NULL;

				int nCount = pProbDistMan->getCount();
				for( int i=0; i<nCount; i++ )
				{
					pPDEntry = pProbDistMan->getItem( i );
					if(pPDEntry->m_csName == strSel)
						break;
				}
				pItem->setStartTime(pPDEntry);
			}
		}
		break;
	case 2:
		{
			pItem->setJetBlastAngle(atof(strSel));
		}
		break;
	case 3:
		{
			double dJetBlastDistanceStart = (CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)atof(strSel)));
			pItem->setJetBlastDistanceStart(dJetBlastDistanceStart);
		}
		break;
	case 4:
		{
			double dJetBlastDistanceTaxi = (CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)atof(strSel)));
			pItem->setJetBlastDistanceTaxi(dJetBlastDistanceTaxi);
		}
		break;
	case 5:
		{
			double dJetBlastDistanceTakeoff = (CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,(double)atof(strSel)));
			pItem->setJetBlastDistanceTakeoff(dJetBlastDistanceTakeoff);
		}
		break;
	default:
		break;
	}

	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(TRUE);
}

bool CDlgEngineParameters::ConvertUnitFromDBtoGUI(void)
{
	return (false);
}

bool CDlgEngineParameters::RefreshGUI(void)
{
	OnInitListCtrl();
	return (true);
}

bool CDlgEngineParameters::ConvertUnitFromGUItoDB(void)
{
	return (false);
}