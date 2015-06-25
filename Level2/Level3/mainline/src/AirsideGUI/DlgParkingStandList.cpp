// DlgRunwayExitSelect.cpp : implementation file
//

#include "stdafx.h"
// #include "AirsideGUI.h"

#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside\ALTObjectGroup.h"
#include "DlgParkingStandList.h"
#include "DlgParkingStandSelect.h"


// CDlgParkingStandList

IMPLEMENT_DYNAMIC(CDlgParkingStandList, CXTResizeDialog)
CDlgParkingStandList::CDlgParkingStandList(const UINT nID,int nProjID,StandList &standList, CWnd* pParent)
:CXTResizeDialog(CDlgParkingStandList::IDD, pParent),
m_nPrjID(nProjID)
{
	m_vStandList.clear();
	m_vStandList.assign(standList.begin(),standList.end());
}

CDlgParkingStandList::~CDlgParkingStandList()
{
}

BEGIN_MESSAGE_MAP(CDlgParkingStandList, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnAddList)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnDelStand)
	ON_COMMAND(ID_TOOLBARBUTTONUP,OnStandUp)
	ON_COMMAND(ID_TOOLBARBUTTONDOWN,OnStandDown)
	
 	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ORDER, OnTvnSelchangedTreeStand)
END_MESSAGE_MAP()

void CDlgParkingStandList::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_ORDER,m_treeStandOrder);
}

BOOL CDlgParkingStandList::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
// 	InitListCtrl();
	SetDlgItemText(IDC_STATIC,_T("Stands"));
	SetWindowText("Stand Order");
	CRect rcToolbar;
	m_treeStandOrder.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_toolBarStand.MoveWindow(rcToolbar);
	m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
	m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
	m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
	SetStandOrderContent();
	InvalidateRect(NULL);
	return true;
}
int CDlgParkingStandList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBarStand.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarStand.LoadToolBar(IDR_TOOLBAR_ASSIGNEDSTAND_ORDER1))
	{
		return -1;
	}
	return 0;
}

void CDlgParkingStandList::OnAddList()
{
	CDlgParkingStandSelect dlg(CDlgParkingStandSelect::IDD,m_nPrjID,_T(""));
	if(IDOK != dlg.DoModal())
	{
		return;
	}
	CString strParkingStandName=dlg.GetStandFamilyName();
	StandList vStands=dlg.GetStandGroupList();
	addList(vStands);
	SetStandOrderContent();
	
}
void CDlgParkingStandList::OnDelStand()
{
	HTREEITEM hSelItem=m_treeStandOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int standIndex=(int)m_treeStandOrder.GetItemData(hSelItem);
	m_vStandList.erase(m_vStandList.begin()+standIndex);
	HTREEITEM hNextItem=m_treeStandOrder.GetNextItem(hSelItem,TVGN_NEXT);
	HTREEITEM hPreItem=m_treeStandOrder.GetPrevSiblingItem(hSelItem);
	if (hNextItem!=NULL)
	{
		m_treeStandOrder.SelectItem(hNextItem);
	}else if(hPreItem!=NULL)
	{
		m_treeStandOrder.SelectItem(hPreItem);
	}

	while(hNextItem!=NULL)
	{
		m_treeStandOrder.SetItemData(hNextItem,standIndex);
		hNextItem=m_treeStandOrder.GetNextItem(hNextItem,TVGN_NEXT);
		standIndex++;
	}
  	m_treeStandOrder.DeleteItem(hSelItem);	

}
void CDlgParkingStandList::OnStandUp()
{
	HTREEITEM hSelItem=m_treeStandOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int selIndex=(int)m_treeStandOrder.GetItemData(hSelItem);
	if (selIndex==0)
	{
		return;
	}
	StandList::iterator selStandIter=m_vStandList.begin()+selIndex;
	int tmpStand=(int)(*selStandIter);
	*selStandIter=*(selStandIter-1);
	*(selStandIter-1)=tmpStand;

	CString strPreItem,strSelItem;
	HTREEITEM hPreItem=m_treeStandOrder.GetPrevSiblingItem(hSelItem);
	strPreItem=m_treeStandOrder.GetItemText(hPreItem);
	strSelItem=m_treeStandOrder.GetItemText(hSelItem);
	m_treeStandOrder.SetItemText(hPreItem,strSelItem);
	m_treeStandOrder.SetItemText(hSelItem,strPreItem);
	m_treeStandOrder.SelectItem(hPreItem);
	
}
void CDlgParkingStandList::OnStandDown()
{
	HTREEITEM hSelItem=m_treeStandOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int selIndex=(int)m_treeStandOrder.GetItemData(hSelItem);
	if (selIndex+1==(int)m_vStandList.size())
	{
		return;
	}
	StandList::iterator selStandIter=m_vStandList.begin()+selIndex;
	int tmpStand=(int)(*selStandIter);
	*selStandIter=*(selStandIter+1);
	*(selStandIter+1)=tmpStand;

	CString strNextItem,strSelItem;
	HTREEITEM hNextItem=m_treeStandOrder.GetNextItem(hSelItem,TVGN_NEXT);
	strNextItem=m_treeStandOrder.GetItemText(hNextItem);
	strSelItem=m_treeStandOrder.GetItemText(hSelItem);
	m_treeStandOrder.SetItemText(hNextItem,strSelItem);
	m_treeStandOrder.SetItemText(hSelItem,strNextItem);
	m_treeStandOrder.SelectItem(hNextItem);

}
void CDlgParkingStandList::OnTvnSelchangedTreeStand(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem=m_treeStandOrder.GetSelectedItem();
	if (hSelItem!=NULL && hSelItem!=m_hTreeRoot)
	{
        m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN);
		return;
	}
	if (hSelItem==NULL||(m_hTreeRoot!=NULL && m_hTreeRoot==hSelItem))
	{
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
		m_toolBarStand.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
		return;
	}



}
void CDlgParkingStandList::OnSize(UINT nType, int cx, int cy)
{

}

CDlgParkingStandList::StandList& CDlgParkingStandList::GetStandList()
{
	return m_vStandList;
}

void CDlgParkingStandList::SetStandOrderContent(void)
{
	m_treeStandOrder.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	cni.nt=NT_NULL; 
	CCoolTree::InitNodeInfo(this,cni);

	m_hTreeRoot=m_treeStandOrder.InsertItem("Stand Order",cni,FALSE);
 	HTREEITEM hChild;
	StandList::iterator standIter=m_vStandList.begin();
	CString strStandName;
	int i=0;
	for (;standIter!=m_vStandList.end();standIter++)
	{
		int nStandID = (int)(*standIter);
		ALTObjectGroup altObjGroup;
		altObjGroup.ReadData(nStandID);
		ALTObjectID altObjID = altObjGroup.getName();
		strStandName=altObjID.GetIDString();
		if (strStandName==_T("")||strStandName==_T(" "))
		{
			int iii=0;
		}
		hChild=m_treeStandOrder.InsertItem(strStandName,cni,FALSE,FALSE,m_hTreeRoot);
		m_treeStandOrder.SetItemData(hChild,i);
		i++;
	}
	m_treeStandOrder.Expand(m_hTreeRoot,TVE_EXPAND);




}
void CDlgParkingStandList::addList(StandList &standList)
{
	m_vStandList.insert(m_vStandList.end(),standList.begin(),standList.end());
}

void CDlgParkingStandList::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	CXTResizeDialog::OnOK();
}



