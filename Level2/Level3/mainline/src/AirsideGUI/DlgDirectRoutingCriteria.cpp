#include "stdafx.h"
#include "DlgDirectRoutingCriteria.h"
#include "DlgDirectRoutingSelectARWayPoint.h"
#include "..\InputAirside\AirRoute.h"
#include "..\InputAirside\ALTObject.h"

#include "../Common/WinMsg.h"
#include "../Database/DBElementCollection_Impl.h"


static const UINT ID_NEW_ITEM = 10;
static const UINT ID_DEL_ITEM = 11;
//static const UINT ID_EDIT_ITEM = 12;

IMPLEMENT_DYNAMIC(CDlgDirectRoutingCriteria, CXTResizeDialog)
CDlgDirectRoutingCriteria::CDlgDirectRoutingCriteria(int nProjID, CWnd* pParent /*=NULL*/)
: CXTResizeDialog(CDlgDirectRoutingCriteria::IDD, pParent)
,m_nProjID(nProjID)
,m_strSegFromFirst(_T(""))
,m_strSegToFirst(_T(""))
,m_strSegMaxFirst(_T(""))
{
	m_pDirectRoutingList = new CDirectRoutingList();
}

CDlgDirectRoutingCriteria::~CDlgDirectRoutingCriteria()
{
	delete m_pDirectRoutingList;
}

void CDlgDirectRoutingCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DIRECTROUTING, m_wndListCtrl);
}

BOOL CDlgDirectRoutingCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	m_wndListCtrl.SetProjID(m_nProjID);
	InitToolBar();
//	GetStrListSegFrom();
	InitListCtrl();
	m_pDirectRoutingList->ReadData(m_nProjID);
	SetListContent();
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
//	GetDlgItem(ID_EDIT_ITEM)->ShowWindow(SW_HIDE);
	GetStrSegFirst();

	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);

	SetResize(IDC_STATIC_TOOLBARCONTENTER, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_DIRECTROUTING, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	//SetResize(IDC_STATIC_AREA, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);	
	SetResize(IDC_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgDirectRoutingCriteria::GetStrSegFirst()
{
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	int nFlag;
	nFlag = 0;
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			m_strSegFromFirst = pARWaypoint->getWaypoint().GetObjNameString();
			m_nSegFromIDFirst = pARWaypoint->getWaypoint().getID();
			nFlag = 1;
			break;			
		}
		if(nFlag == 1)
			break;
	}

	nFlag = 0;
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			if(pARWaypoint->getWaypoint().getID() == m_nSegFromIDFirst)
			{
				if(j > 0)
				{
					ARWaypoint* pARWaypointPre = pAirRoute->GetWayPointByIdx(j - 1);
					m_strSegToFirst = pARWaypointPre->getWaypoint().GetObjNameString();
					m_nSegToIDFirst = pARWaypointPre->getWaypoint().getID();
					break;										
				}
				if(j < nARWayPointCount - 1)
				{
					ARWaypoint* pARWaypointNext = pAirRoute->GetWayPointByIdx(j + 1);
					m_strSegToFirst = pARWaypointNext->getWaypoint().GetObjNameString();
					m_nSegToIDFirst = pARWaypointNext->getWaypoint().getID();
					break;						
				}						
			}			
		}
		if(nFlag == 1)
			break;
	}

	nFlag = 0;
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount - 2; j++)
		{
			ARWaypoint* pARWaypoint1 = pAirRoute->GetWayPointByIdx(j);
			ARWaypoint* pARWaypoint2 = pAirRoute->GetWayPointByIdx(j+1);
			if((pARWaypoint1->getWaypoint().getID() == m_nSegFromIDFirst && pARWaypoint2->getWaypoint().getID() == m_nSegToIDFirst))
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j+2);
				m_strSegMaxFirst = pARWaypoint->getWaypoint().GetObjNameString();
				m_nSegMaxIDFirst = pARWaypoint->getWaypoint().getID();							
				break;
			}
		}
		if(nFlag == 1)
			break;
	}	
}
bool CDlgDirectRoutingCriteria::IsInStrListSegFrom(CString str)
{
	if((m_strListSegFrom.Find(str)) == NULL)
		return false;
	else
		return true;
}
bool CDlgDirectRoutingCriteria::IsInStrListSegTo(CString str)
{
	if((m_strListSegTo.Find(str)) == NULL)
		return false;
	else
		return true;
}
bool CDlgDirectRoutingCriteria::IsInStrListSegMax(CString str)
{
	if((m_strListSegMax.Find(str)) == NULL)
		return false;
	else
		return true;
}
void CDlgDirectRoutingCriteria::InitToolBar()
{
	ASSERT(::IsWindow(m_wndToolBar.m_hWnd));
	CRect rectToolBar;
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->GetWindowRect(&rectToolBar);
	ScreenToClient(&rectToolBar);
	rectToolBar.left += 2;
	m_wndToolBar.MoveWindow(&rectToolBar);
	m_wndToolBar.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOOLBARCONTENTER)->ShowWindow(SW_HIDE);

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ITEM);
	toolbar.SetCmdID(1, ID_DEL_ITEM);
//	toolbar.SetCmdID(2, ID_EDIT_ITEM);

	UpdateToolBar();
}

void CDlgDirectRoutingCriteria::InitListCtrl()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	m_wndListCtrl.ModifyStyle(0, LVS_SHOWSELALWAYS|LVS_SINGLESEL);
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	CStringList strList;
	DRLV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.fmt = LVCFMT_DROPDOWN;
	lvc.csList = &strList;

//	lvc.csList = &m_strListSegFrom;

	lvc.pszText = _T("RouteSegmentFrom");
	lvc.cx = 150;
//	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

//	lvc.csList = &m_strListSegTo;
	lvc.pszText = _T("RouteSegmentTo");
	lvc.cx = 150;
//	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

//	lvc.csList = &m_strListSegMax;
	lvc.pszText = _T("Permitted Direct To Max");
	lvc.cx = 150;
//	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

}

void CDlgDirectRoutingCriteria::UpdateToolBar()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	// if the item count > MAXITEMCOUNT, disable the new item button.
	BOOL bEnable = TRUE;
	int nItemCount = m_wndListCtrl.GetItemCount();
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ITEM, bEnable);

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	bEnable = (pos == NULL ? FALSE : TRUE);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ITEM, bEnable);
//	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ITEM, bEnable);

}

BEGIN_MESSAGE_MAP(CDlgDirectRoutingCriteria, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_ITEM, OnNewItem)
	ON_COMMAND(ID_DEL_ITEM, OnDeleteItem)
//	ON_COMMAND(ID_EDIT_ITEM, OnEditItem)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_SAVE, OnBnClickedSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DIRECTROUTING, OnLvnItemchangedListContents)
	//ON_NOTIFY(NM_DBLCLK, IDC_LIST_DIRECTROUTING, OnNMDblclkListContents)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
END_MESSAGE_MAP()


// CDlgDirectRoutingCriteria message handlers

int CDlgDirectRoutingCriteria::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_wndToolBar.CreateEx(this);
	m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL);

	return 0;
}

void CDlgDirectRoutingCriteria::OnNewItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	int nId = m_wndListCtrl.GetItemCount();
	if(m_strSegFromFirst == _T(""))
		m_wndListCtrl.InsertItem(nId, _T("No Node"));
	else
	{	
		CDirectRouting* pDirectRouting = new CDirectRouting;
		pDirectRouting->SetARWaypointStartID(m_nSegFromIDFirst);
		pDirectRouting->SetARWaypointEndID(m_nSegToIDFirst);
		pDirectRouting->SetARWaypointMaxID(m_nSegMaxIDFirst);
		m_pDirectRoutingList->AddNewItem(pDirectRouting);

		m_wndListCtrl.InsertItem(nId, m_strSegFromFirst);
		m_wndListCtrl.SetItemText(nId, 1, m_strSegToFirst);
		m_wndListCtrl.SetItemText(nId, 2, m_strSegMaxFirst);
		m_wndListCtrl.SetItemData(nId, (DWORD_PTR)pDirectRouting);
	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
	UpdateToolBar();
/*
	CDirectRouting* pDirectRouting = new CDirectRouting;
	CDlgDirectRoutingSelectARWayPoint dlg(m_nProjID, pDirectRouting);

	if(IDOK == dlg.DoModal())
	{

		int nIndex = m_wndListCtrl.GetItemCount();
		CString strTempValue;
		strTempValue = _T("");
		int nARWayPointStartID = pDirectRouting->GetARWaypointStartID();
		AirRouteList airRouteList;
		airRouteList.ReadData(m_nProjID);
		int nAirRouteCount = airRouteList.GetAirRouteCount();
		int nFlag;
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointStartID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)
				break;
		}
		m_wndListCtrl.InsertItem(nIndex,strTempValue);

		int nARWayPointEndID = pDirectRouting->GetARWaypointEndID();
		strTempValue = _T("");
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointEndID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)		
				break;
			
		}
		m_wndListCtrl.SetItemText(nIndex, 1, strTempValue);
		
		int nARWayPointMaxID = pDirectRouting->GetARWaypointMaxID();
		strTempValue = _T("");
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointMaxID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)		
				break;

		}
		m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);

		m_pDirectRoutingList->AddNewItem(pDirectRouting);
	
		GetDlgItem(IDC_SAVE)->EnableWindow();
		UpdateToolBar();
	}
	*/
}
void CDlgDirectRoutingCriteria::SetListContent()
{
	size_t nDirectRoutingCount = m_pDirectRoutingList->GetElementCount();

	CString strTempValue;
	for (size_t i = 0; i < nDirectRoutingCount; i++)
	{
		CDirectRouting* pDirectRouting = m_pDirectRoutingList->GetItem(i);
		
		CString strTempValue;
		strTempValue = _T("");
		int nARWayPointStartID = pDirectRouting->GetARWaypointStartID();
		AirRouteList airRouteList;
		airRouteList.ReadData(m_nProjID);
		int nAirRouteCount = airRouteList.GetAirRouteCount();
		int nFlag;
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointStartID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)
				break;
		}
		m_wndListCtrl.InsertItem((int)i, strTempValue);

		int nARWayPointEndID = pDirectRouting->GetARWaypointEndID();
		strTempValue = _T("");
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointEndID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)		
				break;

		}
		m_wndListCtrl.SetItemText((int)i, 1, strTempValue);

		int nARWayPointMaxID = pDirectRouting->GetARWaypointMaxID();
		strTempValue = _T("");
		nFlag = 0;
		for (int j = 0; j < nAirRouteCount; j++)
		{
			CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
			int nARWayPointCount = pAirRoute->GetWayPointCount();
			for (int k = 0; k < nARWayPointCount; k++)
			{
				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
				if(pARWaypoint->getWaypoint().getID() == nARWayPointMaxID)
				{
					strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)		
				break;

		}
		m_wndListCtrl.SetItemText((int)i, 2, strTempValue);
		m_wndListCtrl.SetItemData((int)i, (DWORD_PTR)pDirectRouting);
	}
}
void CDlgDirectRoutingCriteria::OnDeleteItem()
{
	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));

	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		//while (pos)
		//{
			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
			//TODO
			m_wndListCtrl.DeleteItemEx(nItem);
			m_pDirectRoutingList->DeleteItem(nItem);
		//}
	}

	GetDlgItem(IDC_SAVE)->EnableWindow();
	UpdateToolBar();
}

//void CDlgDirectRoutingCriteria::OnEditItem()
//{
//	ASSERT(::IsWindow(m_wndListCtrl.m_hWnd));
//
//	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
//	if (pos != NULL)
//	{
//		while (pos)
//		{
//			int nItem = m_wndListCtrl.GetNextSelectedItem(pos);
//			//TODO
//			CDirectRouting* pDirectRouting = m_pDirectRoutingList->GetItem(nItem);
//			CDlgDirectRoutingSelectARWayPoint dlg(m_nProjID, pDirectRouting);
//
//			if(IDOK == dlg.DoModal())
//			{
//				int nIndex = nItem;
//				CString strTempValue;
//				int nARWayPointStartID = pDirectRouting->GetARWaypointStartID();
//				strTempValue = _T("");
//				AirRouteList airRouteList;
//				airRouteList.ReadData(m_nProjID);
//				int nAirRouteCount = airRouteList.GetAirRouteCount();
//				int nFlag;
//				nFlag = 0;
//				for (int j = 0; j < nAirRouteCount; j++)
//				{
//					CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
//					int nARWayPointCount = pAirRoute->GetWayPointCount();
//					for (int k = 0; k < nARWayPointCount; k++)
//					{
//						ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
//						if(pARWaypoint->getWaypoint().getID() == nARWayPointStartID)
//						{
//							strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
//							nFlag = 1;
//							break;
//						}
//					}
//					if(nFlag == 1)
//						break;
//				}
//				m_wndListCtrl.SetItemText(nIndex, 0, strTempValue);
//
//				int nARWayPointEndID = pDirectRouting->GetARWaypointEndID();
//				strTempValue = _T("");
//				nFlag = 0;
//				for (int j = 0; j < nAirRouteCount; j++)
//				{
//					CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
//					int nARWayPointCount = pAirRoute->GetWayPointCount();
//					for (int k = 0; k < nARWayPointCount; k++)
//					{
//						ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
//						if(pARWaypoint->getWaypoint().getID() == nARWayPointEndID)
//						{
//							strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
//							nFlag = 1;
//							break;
//						}
//					}
//					if(nFlag == 1)		
//						break;
//
//				}
//				m_wndListCtrl.SetItemText(nIndex, 1, strTempValue);
//
//				int nARWayPointMaxID = pDirectRouting->GetARWaypointMaxID();
//				strTempValue = _T("");
//				nFlag = 0;
//				for (int j = 0; j < nAirRouteCount; j++)
//				{
//					CAirRoute* pAirRoute = airRouteList.GetAirRoute(j);
//					int nARWayPointCount = pAirRoute->GetWayPointCount();
//					for (int k = 0; k < nARWayPointCount; k++)
//					{
//						ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
//						if(pARWaypoint->getWaypoint().getID() == nARWayPointMaxID)
//						{
//							strTempValue = pARWaypoint->getWaypoint().GetObjNameString();
//							nFlag = 1;
//							break;
//						}
//					}
//					if(nFlag == 1)		
//						break;
//
//				}
//				m_wndListCtrl.SetItemText(nIndex, 2, strTempValue);
//			}
//		}
//
//	}
//
//	GetDlgItem(IDC_SAVE)->EnableWindow();
//	UpdateToolBar();
//}


void CDlgDirectRoutingCriteria::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pDirectRoutingList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	OnOK();
}

void CDlgDirectRoutingCriteria::OnBnClickedCancel()
{
	OnCancel();
}

void CDlgDirectRoutingCriteria::OnBnClickedSave()
{
//	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pDirectRoutingList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	OnOK();
}

void CDlgDirectRoutingCriteria::OnLvnItemchangedListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}

//void CDlgDirectRoutingCriteria::OnNMDblclkListContents(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: Add your control notification handler code here
//	OnEditItem();
//	*pResult = 0;
//}
LRESULT CDlgDirectRoutingCriteria::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;	

	CDirectRouting* pDirectRouting = (CDirectRouting*)m_wndListCtrl.GetItemData(nCurSel);
//	CDirectRouting* pDirectRouting = m_pDirectRoutingList->GetItem(nCurSel);
	CString str = m_wndListCtrl.GetItemText(nCurSel,nCurSubSel);
	std::vector<ALTObject> vWaypoint;
	ALTObject::GetObjectList(ALT_WAYPOINT,m_nProjID,vWaypoint);

	switch (nCurSubSel)
	{
	case 0: // SegFrom
		{
			int i = 0; 
			for (;i < static_cast<int>(vWaypoint.size()); ++ i)
			{
				if(vWaypoint[i].GetObjNameString() == str)
				{
					pDirectRouting->SetARWaypointStartID(vWaypoint[i].getID());
					break;
				}	
			}
			if(i == static_cast<int>(vWaypoint.size()))
				pDirectRouting->SetARWaypointStartID(-1);
			else
			{
				GetStrListSegTo(vWaypoint[i].getID());
				CString strSegTo = m_wndListCtrl.GetItemText(nCurSel,1);
				if(!IsInStrListSegTo(strSegTo))
				{
					m_wndListCtrl.SetItemText(nCurSel,1,_T(""));
					m_wndListCtrl.SetItemText(nCurSel,2,_T(""));
					pDirectRouting->SetARWaypointEndID(-1);
					pDirectRouting->SetARWaypointMaxID(-1);
				}
			}
		}
		break;
	case 1: // SegTo
		{
			int i = 0;
			for (; i < static_cast<int>(vWaypoint.size()); ++ i)
			{
				if(vWaypoint[i].GetObjNameString() == str)
				{
					pDirectRouting->SetARWaypointEndID(vWaypoint[i].getID());
					break;
				}	
			}

			if(i == static_cast<int>(vWaypoint.size()))
				pDirectRouting->SetARWaypointEndID(-1);
			else
			{
				CString strSegFrom = m_wndListCtrl.GetItemText(nCurSel,0);
				int j = 0; 
				for (;j < static_cast<int>(vWaypoint.size()); ++ j)
				{
					if(vWaypoint[j].GetObjNameString() == strSegFrom)
					{
						break;
					}	
				}

				GetStrListSegMax(vWaypoint[j].getID(),vWaypoint[i].getID());
				CString strSegMax = m_wndListCtrl.GetItemText(nCurSel,2);
				if(!IsInStrListSegMax(strSegMax))
				{
					m_wndListCtrl.SetItemText(nCurSel,2,_T(""));
					pDirectRouting->SetARWaypointMaxID(-1);
				}
			}
		}
		break;
	case 2: // SegMax
		{
			int i = 0; 
			for (;i < static_cast<int>(vWaypoint.size()); ++ i)
			{
				if(vWaypoint[i].GetObjNameString() == str)
				{
					pDirectRouting->SetARWaypointMaxID(vWaypoint[i].getID());
					break;
				}	
			}
			if(i == static_cast<int>(vWaypoint.size()))
				pDirectRouting->SetARWaypointMaxID(-1);
		}
		break;
	default:
		break;
	}	

	//int nComboxSel = (int)wParam;
	//if (nComboxSel == LB_ERR)
	//	return 0;

	//LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	//int nCurSel = dispinfo->item.iItem;
	//int nCurSubSel = dispinfo->item.iSubItem;	

	//CDirectRouting* pDirectRouting = (CDirectRouting*)m_wndListCtrl.GetItemData(nCurSel);

	//switch (nCurSubSel)
	//{
	//case 0: // SegFrom
	//	{
	//		CString str = m_wndListCtrl.GetItemText(nCurSel,nCurSubSel);
	//		SegMapIter iter = m_SegFromMap.find(str);
	//		int nARWaypointStartID = (*iter).second;
	//		pDirectRouting->SetARWaypointStartID(nARWaypointStartID);
	//		m_strListSegTo.RemoveAll();
	//		AirRouteList airRouteList;
	//		airRouteList.ReadData(m_nProjID);
	//		int nAirRouteCount = airRouteList.GetAirRouteCount();
	//		for (int i = 0; i < nAirRouteCount; i++)
	//		{
	//			CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
	//			int nARWayPointCount = pAirRoute->GetWayPointCount();
	//			for (int j = 0; j < nARWayPointCount; j++)
	//			{
	//				ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
	//				if(pARWaypoint->getWaypoint().getID() == nARWaypointStartID)
	//				{
	//					if(j > 0)
	//					{
	//						ARWaypoint* pARWaypointPre = pAirRoute->GetWayPointByIdx(j - 1);
	//						CString strNamePre =pARWaypointPre->getWaypoint().GetObjNameString();
	//						if(!IsInStrListSegTo(strNamePre))
	//						{
	//							m_strListSegTo.InsertAfter(m_strListSegTo.GetTailPosition(), strNamePre);
	//							int nSegToID = pARWaypointPre->getWaypoint().getID();
	//							m_SegToMap.insert(std::make_pair(strNamePre,nSegToID));
	//						}							
	//					}
	//					if(j < nARWayPointCount - 1)
	//					{
	//						ARWaypoint* pARWaypointNext = pAirRoute->GetWayPointByIdx(j + 1);
	//						CString strNameNext =pARWaypointNext->getWaypoint().GetObjNameString();
	//						if(!IsInStrListSegTo(strNameNext))
	//						{
	//							m_strListSegTo.InsertAfter(m_strListSegTo.GetTailPosition(), strNameNext);
	//							int nSegToID = pARWaypointNext->getWaypoint().getID();
	//							m_SegToMap.insert(std::make_pair(strNameNext,nSegToID));
	//						}	
	//					}						
	//				}			
	//			}
	//		}
	//	}
	//	break;
	//case 1: // SegTo
	//	{
	//		//int nCurNodeEndID = (m_vNodeList.at(nComboxSel)).m_nUniqueID;
	//		//pCurData->SetEndID(nCurNodeEndID);
	//	}
	//	break;
	//case 2: // SegMax
	//	{
	//		//int nCurNodeEntryID = (m_vNodeList.at(nComboxSel)).m_nUniqueID;
	//		//pCurData->SetEntryID(nCurNodeEntryID);
	//	}
	//	break;
	//default:
	//	break;
	//}	

	return 0;
}
void CDlgDirectRoutingCriteria::GetStrListSegFrom()
{
	m_strListSegFrom.RemoveAll();
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			CString strName = pARWaypoint->getWaypoint().GetObjNameString();
			if(!IsInStrListSegFrom(strName))
			{
				m_strListSegFrom.InsertAfter(m_strListSegFrom.GetTailPosition(), strName);
				//		m_nSegFromID = pARWaypoint->getWaypoint().getID();
				//		m_SegFromMap.insert(std::make_pair(strName,m_nSegFromID));
			}
		}
	}
}

void CDlgDirectRoutingCriteria::GetStrListSegTo(int nSegFromID)
{
	m_strListSegTo.RemoveAll();
	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount; j++)
		{
			ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(j);
			if(pARWaypoint->getWaypoint().getID() == nSegFromID)
			{
				if(j > 0)
				{
					ARWaypoint* pARWaypointPre = pAirRoute->GetWayPointByIdx(j - 1);
					CString strNamePre =pARWaypointPre->getWaypoint().GetObjNameString();
					if(!IsInStrListSegTo(strNamePre))
					{
						m_strListSegTo.InsertAfter(m_strListSegTo.GetTailPosition(), strNamePre);
						//			int nSegToID = pARWaypointPre->getWaypoint().getID();
						//			m_SegToMap.insert(std::make_pair(strNamePre,nSegToID));
					}							
				}
				if(j < nARWayPointCount - 1)
				{
					ARWaypoint* pARWaypointNext = pAirRoute->GetWayPointByIdx(j + 1);
					CString strNameNext =pARWaypointNext->getWaypoint().GetObjNameString();
					if(!IsInStrListSegTo(strNameNext))
					{
						m_strListSegTo.InsertAfter(m_strListSegTo.GetTailPosition(), strNameNext);
						//			int nSegToID = pARWaypointNext->getWaypoint().getID();
						//			m_SegToMap.insert(std::make_pair(strNameNext,nSegToID));
					}	
				}						
			}			
		}
	}
}
void CDlgDirectRoutingCriteria::GetStrListSegMax(int nSegFromID,int nSegToID)
{
	m_strListSegMax.RemoveAll();

	AirRouteList airRouteList;
	airRouteList.ReadData(m_nProjID);
	int nAirRouteCount = airRouteList.GetAirRouteCount();
	for (int i = 0; i < nAirRouteCount; i++)
	{
		CAirRoute* pAirRoute = airRouteList.GetAirRoute(i);
		int nARWayPointCount = pAirRoute->GetWayPointCount();
		for (int j = 0; j < nARWayPointCount - 1; j++)
		{
			ARWaypoint* pARWaypoint1 = pAirRoute->GetWayPointByIdx(j);
			ARWaypoint* pARWaypoint2 = pAirRoute->GetWayPointByIdx(j+1);
			if((pARWaypoint1->getWaypoint().getID() == nSegFromID && pARWaypoint2->getWaypoint().getID() == nSegToID))
			{
				for (int k = j+2; k < nARWayPointCount; k++)
				{
					ARWaypoint* pARWaypoint = pAirRoute->GetWayPointByIdx(k);
					CString strName = pARWaypoint->getWaypoint().GetObjNameString();
					if(!IsInStrListSegMax(strName))
						m_strListSegMax.InsertAfter(m_strListSegMax.GetTailPosition(),strName);
				}
			}
		}
	}
}