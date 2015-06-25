#include "stdafx.h"
#include "Resource.h"
#include "DlgItinerantFlightSchedule.h"
#include ".\dlgitinerantflightschedule.h"
#include "..\InputAirside\ItinerantFlight.h"
#include "..\InputAirside\ItinerantFlightSchedule.h"
#include "..\InputAirside\ALTObjectGroup.h"
#include "../Database/DBElementCollection_Impl.h"

static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgItinerantFlightSchedule, CXTResizeDialog)
CDlgItinerantFlightSchedule::CDlgItinerantFlightSchedule(/*ItinerantFlightList* pItinerantFltList, */ItinerantFlightSchedule* pItinerantFlightSchedule,
														 std::vector<ALTObject>&vWaypoint, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgItinerantFlightSchedule::IDD, pParent)
	//,m_pItinerantFltList(pItinerantFltList)
	,m_pItinerantFlightSchedule(pItinerantFlightSchedule)
	,m_vectWaypoint(vWaypoint)
{
}

CDlgItinerantFlightSchedule::~CDlgItinerantFlightSchedule()
{
}

void CDlgItinerantFlightSchedule::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgItinerantFlightSchedule, CXTResizeDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDSAVE, OnBnClickedSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgItinerantFlightSchedule message handlers

int CDlgItinerantFlightSchedule::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT);

	return 0;
}

void CDlgItinerantFlightSchedule::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBAR)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, FALSE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, FALSE);

	//m_wndToolBar.EnableWindow(FALSE);
}

void CDlgItinerantFlightSchedule::InitListCtrl()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.csList = &strList;

	lvc.pszText = _T("Name");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("ID");
	lvc.cx = 40;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = _T("ArrTime");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("DepTime");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(3,&lvc);

	lvc.pszText = _T("From");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(4, &lvc);


	lvc.pszText = _T("To");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.pszText = _T("AC Type");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(6, &lvc);

}

BOOL CDlgItinerantFlightSchedule::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitListCtrl();

	SetResize(IDC_STATIC_TOOLBAR, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDSAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);	


	ResetListCtrl();


	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgItinerantFlightSchedule::OnBnClickedSave()
{
}

void CDlgItinerantFlightSchedule::OnBnClickedOk()
{
	OnOK();
}

void CDlgItinerantFlightSchedule::ResetListCtrl()
{
	ASSERT(m_pItinerantFlightSchedule != NULL);

	size_t nCount = m_pItinerantFlightSchedule->GetElementCount();
	for (size_t i = 0; i < nCount; i++)
	{
		ItinerantFlightScheduleItem* pItem = m_pItinerantFlightSchedule->GetItem( i );
		ElapsedTime pArrTime = pItem->getArrTime();
		ElapsedTime pDepTime = pItem->getDepTime();

		CString strTemp;
		char strValue[256] = {0};
		pItem->getAirline(strValue);
		m_wndListCtrl.InsertItem((int)i,strValue);
		
		pItem->getFlightIDString(strValue);
		strTemp.Format("%s",strValue);
		m_wndListCtrl.SetItemText(i, 1, strTemp );

		if(pItem->GetEntryID() != -1)
		{

			strTemp.Format("%02d::%02d::%02d", pArrTime.GetHour(), pArrTime.GetMinute(), pArrTime.GetSecond());
			m_wndListCtrl.SetItemText(i, 2, strTemp);
		}
		if (pItem->GetExitID() != -1 && !pItem->GetEnRoute())
		{
			strTemp.Format("%02d::%02d::%02d", pDepTime.GetHour(), pDepTime.GetMinute(), pDepTime.GetSecond());
			m_wndListCtrl.SetItemText(i, 3, strTemp);
		}
		//--------------------------------------------------------
		int nEntryFlag = pItem->GetEntryFlag();
		int nEntryID = pItem->GetEntryID();

		pItem->getOrigin(strValue);
		strTemp.Format("%s",strValue);
		m_wndListCtrl.SetItemText(i,4,strTemp);

		pItem->getDestination(strValue);
		strTemp.Format("%s",strValue);
		m_wndListCtrl.SetItemText(i,5,strTemp);
// 		if (nEntryID != -1)
// 		{
// 			if(nEntryFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
// 			{
// 				for (int i = 0; i < static_cast<int>(m_vectWaypoint.size()); ++ i)
// 				{
// 					if(m_vectWaypoint[i].getID() == nEntryID)
// 					{
// 						CString strEntry = m_vectWaypoint[i].GetObjNameString();
// 						strTemp.Format("%s",strEntry);
// 						break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				//must not be group!!
// 				ALTObject *pObject = ALTObject::ReadObjectByID(nEntryID);
// 				strTemp = pObject->GetObjNameString();
// 				delete pObject;
// 			}
// 			m_wndListCtrl.SetItemText(i,4, strTemp);
// 		}
// 
// 		int nExitFlag = pItem->GetExitFlag();
// 		int nExitID = pItem->GetExitID();
// 		if (nExitID != -1)
// 		{
// 			if(nExitFlag == ALTOBJECT_TYPE(ALT_WAYPOINT))
// 			{
// 				for (int i = 0; i < static_cast<int>(m_vectWaypoint.size()); ++ i)
// 				{
// 					if(m_vectWaypoint[i].getID() == nExitID)
// 					{
// 						CString strExit = m_vectWaypoint[i].GetObjNameString();
// 						strTemp.Format("%s",strExit);
// 						break;
// 					}
// 				}
// 			}
// 			else
// 			{
// 				//must not be group!!
// 				ALTObject *pObject = ALTObject::ReadObjectByID(nExitID);
// 				strTemp = pObject->GetObjNameString();
// 				delete pObject;
// 			}
// 			m_wndListCtrl.SetItemText(i, 5, strTemp);
// 		}
// 
// 		//-------------------------------------------------------
		pItem->getACType(strValue);

		m_wndListCtrl.SetItemText(i, 6, strValue );
	}
}












