// DlgIntersectionTrafficCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgIntersectionTrafficCtrl.h"
#include "DlgIntersectionSelect.h"
#include ".\dlgintersectiontrafficctrl.h"
#include "DlgSequentialLightSyn.h"

// CDlgIntersectionTrafficCtrl dialog

IMPLEMENT_DYNAMIC(CDlgIntersectionTrafficControlManagement, CXTResizeDialog)
CDlgIntersectionTrafficControlManagement::CDlgIntersectionTrafficControlManagement(CTermPlanDoc *tmpDoc,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgIntersectionTrafficControlManagement::IDD, pParent)
	, m_pDoc(tmpDoc)
{
	m_pIntersectionTrafficCtrlList=new CIntersectionTrafficControlManagement;

	m_pIntersectionTrafficCtrlList->SetInputLandside(tmpDoc->GetInputLandside());
	m_pIntersectionTrafficCtrlList->ReadData(-1);

}

CDlgIntersectionTrafficControlManagement::~CDlgIntersectionTrafficControlManagement()
{
}

void CDlgIntersectionTrafficControlManagement::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_INTERSECTIONNODE, m_listIntersectionNode);
	DDX_Control(pDX, IDC_LIST_TRAFFICCONTROL, m_listCtrlItem);
}
BOOL CDlgIntersectionTrafficControlManagement::OnInitDialog()
{
	if(!CXTResizeDialog::OnInitDialog())
		return FALSE;

	OnInitToolBar();
	OnInitItemList();
	SetIntersectionListContent();

	SetResize(IDC_STATIC_GROUPBOX,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_INTERSECTIONNODE,SZ_TOP_LEFT,SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_TRAFFICCONTROL,SZ_TOP_CENTER,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_RULE,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(ID_BN_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	m_toolBarItem.GetToolBarCtrl().SetDlgCtrlID(IDR_UPDWON_INTERSECTIONGROUP1);
	SetResize(IDR_UPDWON_INTERSECTIONGROUP1,SZ_TOP_CENTER,SZ_TOP_CENTER);

	GetDlgItem(ID_BN_SAVE)->EnableWindow(FALSE);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	
	m_listIntersectionNode.SetFocus();
	if(m_listIntersectionNode.GetCount() > 0)
	{
		m_listIntersectionNode.SetCurSel(0);
		OnLbnSelchangeListIntersectionnode();
	}



	return FALSE;
}

BEGIN_MESSAGE_MAP(CDlgIntersectionTrafficControlManagement, CXTResizeDialog)
	ON_BN_CLICKED(ID_BN_SAVE, OnBnClickedBnSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_RULE,OnBnClickSyn)
	ON_COMMAND(ID_TOOLBARBUTTONUP,OnItemUp)
	ON_COMMAND(ID_TOOLBARBUTTONDOWN,OnItemDown)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_INTERSECTIONNODE, OnLbnSelchangeListIntersectionnode)

	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_TRAFFICCONTROL, OnLvnEndlabeleditListTrafficcontrol)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_TRAFFICCONTROL, OnLvnItemchangedListTrafficcontrol)
END_MESSAGE_MAP()


void CDlgIntersectionTrafficControlManagement::OnInitToolBar()
{
	CRect rect;

// 	m_listIntersectionNode.GetWindowRect(rect);
// 	ScreenToClient(rect);
// 	rect.top-=28;
// 	rect.left+=2;
// 	rect.bottom=rect.top+26;
// 	m_toolBarIntersection.MoveWindow(rect);
// 	m_toolBarIntersection.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
// 	m_toolBarIntersection.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONEDIT,FALSE);
// 	m_toolBarIntersection.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,FALSE);


	m_listCtrlItem.GetWindowRect(rect);
	ScreenToClient(rect);
	rect.top-=28;
	rect.bottom=rect.top+26;
	m_toolBarItem.MoveWindow(rect);
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,FALSE);
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,FALSE);

}
void CDlgIntersectionTrafficControlManagement::OnInitItemList()
{

	DWORD dwStyle = m_listCtrlItem.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrlItem.SetExtendedStyle(dwStyle);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;

	lvc.pszText = _T("Sequence");
	m_listCtrlItem.InsertColumn(0,&lvc);

	lvc.cx = 120;
	lvc.pszText = _T("Link Group Name");
	m_listCtrlItem.InsertColumn(1,&lvc);

	lvc.fmt = LVCFMT_NUMBER;

	lvc.pszText = _T("Active Time(sec)");
	m_listCtrlItem.InsertColumn(2,&lvc);

	lvc.pszText = _T("Buffer Time(sec)");
	m_listCtrlItem.InsertColumn(3,&lvc);

}
void CDlgIntersectionTrafficControlManagement::SetIntersectionListContent()
{
	m_listIntersectionNode.ResetContent();
	for (int i=0;i<m_pIntersectionTrafficCtrlList->GetItemCount();i++)
	{
		//LandsideIntersectionNode *pIntersection=new LandsideIntersectionNode;
		CIntersectionTrafficControlIntersection *pIntersectionNode=(CIntersectionTrafficControlIntersection *)m_pIntersectionTrafficCtrlList->GetItem(i);
		
		m_listIntersectionNode.InsertString(i,pIntersectionNode->GetIntersectionName());			
		
		m_listIntersectionNode.SetItemData(i,(DWORD)pIntersectionNode);	
	}
}
void CDlgIntersectionTrafficControlManagement::SetItemListContent()
{
	m_listCtrlItem.DeleteAllItems();
	int nIndex=m_listIntersectionNode.GetCurSel();
	if (nIndex<0)
	{
		return;
	}
	CIntersectionTrafficControlIntersection *pIntersectionNode=(CIntersectionTrafficControlIntersection *)m_listIntersectionNode.GetItemData(nIndex);
	if (pIntersectionNode == NULL)
	{
		return;
	}
	for (int nNode=0;nNode<pIntersectionNode->GetItemCount();nNode++)
	{
		CString strText;
		CIntersectionTrafficControlItem *pItem=(CIntersectionTrafficControlItem *)pIntersectionNode->GetItem(nNode);
		
		CString strIndex;
		strIndex.Format("%d", nNode+1);
		m_listCtrlItem.InsertItem(nNode,strIndex);

		m_listCtrlItem.SetItemText(nNode,1,pItem->GetLinkGroupName());

		strText.Format(_T("%d"),pItem->GetActiveTime());
		m_listCtrlItem.SetItemText(nNode,2,strText);

		strText.Format(_T("%d"),pItem->GetBufferTime());
		m_listCtrlItem.SetItemText(nNode,3,strText);

		m_listCtrlItem.SetItemData(nNode,(DWORD)pItem);
	}
}

void CDlgIntersectionTrafficControlManagement::OnItemUp()
{
	int nIndex=m_listCtrlItem.GetCurSel();
	if (nIndex<1 || nIndex>m_listCtrlItem.GetItemCount()-1)
	{
		return;
	}
	int nNodeIndex=m_listIntersectionNode.GetCurSel();
	if (nNodeIndex<0)
	{
		return;
	}
	CIntersectionTrafficControlIntersection *pIntersectionNode=(CIntersectionTrafficControlIntersection *)m_listIntersectionNode.GetItemData(nNodeIndex);
	if (pIntersectionNode == NULL)
	{
		return;
	}
	pIntersectionNode->ItemUp(nIndex);
	SetItemListContent();

	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,TRUE);
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,TRUE);

	if (nIndex==1)
	{
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,FALSE);
	}	

	m_listCtrlItem.SetCurSel(nIndex-1);
	GetDlgItem(ID_BN_SAVE)->EnableWindow(TRUE);
	
}
void CDlgIntersectionTrafficControlManagement::OnItemDown()
{
	int nIndex=m_listCtrlItem.GetCurSel();
	if (nIndex<0 || nIndex>m_listCtrlItem.GetItemCount()-2)
	{
		return;
	}
	int nNodeIndex=m_listIntersectionNode.GetCurSel();
	if (nNodeIndex<0)
	{
		return;
	}
	CIntersectionTrafficControlIntersection *pIntersectionNode=(CIntersectionTrafficControlIntersection *)m_listIntersectionNode.GetItemData(nNodeIndex);
	if (pIntersectionNode == NULL)
	{
		return;
	}
	pIntersectionNode->ItemDown(nIndex);
	SetItemListContent();

	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,TRUE);
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,TRUE);

	if (nIndex == m_listCtrlItem.GetItemCount()-2)	 
	{		
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,FALSE);
	}

	m_listCtrlItem.SetCurSel(nIndex+1);
	GetDlgItem(ID_BN_SAVE)->EnableWindow(TRUE);
	
}
int CDlgIntersectionTrafficControlManagement::IntersectionExist(CString strName)
{	
	//find Intersection in list by id
	CString strCurName;
	for (int i=0;i<m_listIntersectionNode.GetCount();i++)
	{
		m_listIntersectionNode.GetText(i,strCurName);
		if (strName == strCurName)
		{
			return i;
		}
	}
	return -1;
}
// CDlgIntersectionTrafficCtrl message handlers

void CDlgIntersectionTrafficControlManagement::OnBnClickedBnSave()
{
	// TODO: Add your control notification handler code here
	m_pIntersectionTrafficCtrlList->SaveData(-1);
	GetDlgItem(ID_BN_SAVE)->EnableWindow(FALSE);
}

void CDlgIntersectionTrafficControlManagement::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnBnClickedBnSave();
	CXTResizeDialog::OnOK();
}

int CDlgIntersectionTrafficControlManagement::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_toolBarItem.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarItem.LoadToolBar(IDR_UPDWON_INTERSECTIONGROUP1))
	{
		return -1;
	}
	return 0;
}

void CDlgIntersectionTrafficControlManagement::OnLbnSelchangeListIntersectionnode()
{
	// TODO: Add your control notification handler code here
	int nIndex=m_listIntersectionNode.GetCurSel();
	if (nIndex<0)	
	{

		return;
	}
	SetItemListContent();
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,FALSE);
	m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,FALSE);
}


void CDlgIntersectionTrafficControlManagement::OnLvnEndlabeleditListTrafficcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nIndex=pDispInfo->item.iItem;
	CIntersectionTrafficControlItem *pItem=(CIntersectionTrafficControlItem *)m_listCtrlItem.GetItemData(nIndex);
	if (pItem==NULL)
	{
		return;
	}
	int nValue=atoi(pDispInfo->item.pszText);
	if (nValue<0)
	{
		nValue=0;
	}
	CString strValue;
	strValue.Format(_T("%d"),nValue);
	switch (pDispInfo->item.iSubItem)
	{	
	case 2:
		{
			
			pItem->SetActiveTime(nValue);
			m_listCtrlItem.SetItemText(nIndex,2,strValue);
			GetDlgItem(ID_BN_SAVE)->EnableWindow(TRUE);
			break;
		}
	case 3:
		{
			pItem->SetBufferTime(nValue);
			m_listCtrlItem.SetItemText(nIndex,3,strValue);
			GetDlgItem(ID_BN_SAVE)->EnableWindow(TRUE);
			break;
		}
	default:
		{
			break;
		}
	}
	*pResult = 0;


}

void CDlgIntersectionTrafficControlManagement::OnLvnItemchangedListTrafficcontrol(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nIndex=m_listCtrlItem.GetCurSel();

	if (nIndex!=-1)
	{
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,TRUE);
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,TRUE);
	}

	if (nIndex==0)
	{
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,FALSE);
	}
	if (nIndex == m_listCtrlItem.GetItemCount()-1)
	{	
		m_toolBarItem.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,FALSE);
	}
	
	*pResult = 0;
}

void CDlgIntersectionTrafficControlManagement::OnBnClickSyn()
{
	CDlgSequentialLightSyn dlg(m_pDoc);
	dlg.DoModal();
}


