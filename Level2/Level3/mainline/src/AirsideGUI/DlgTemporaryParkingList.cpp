// DlgRunwayExitSelect.cpp : implementation file
//

#include "stdafx.h"

#include "../InputAirside/ALTAirport.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside\ALTObjectGroup.h"
#include "DlgTemporaryParkingList.h"
#include "DlgOccupiedAssignedStandSelectTemporaryParking.h"


// CDlgTemporaryParkingList

IMPLEMENT_DYNAMIC(CDlgTemporaryParkingList, CXTResizeDialog)
CDlgTemporaryParkingList::CDlgTemporaryParkingList(const UINT nID,int nProjID,ParkingList &parkingList, AltObjectVectorMap *temporaryParkingVectorMap,CWnd* pParent)
:CXTResizeDialog(CDlgTemporaryParkingList::IDD, pParent),
m_nPrjID(nProjID)
{
	m_vParkingList.clear();
	m_vParkingList.assign(parkingList.begin(),parkingList.end());
	m_TemporaryParkingVectorMap=temporaryParkingVectorMap;
}

CDlgTemporaryParkingList::~CDlgTemporaryParkingList()
{
}

BEGIN_MESSAGE_MAP(CDlgTemporaryParkingList, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD,OnAddList)
	ON_COMMAND(ID_TOOLBARBUTTONDEL,OnDelParking)
	ON_COMMAND(ID_TOOLBARBUTTONUP,OnParkingUp)
	ON_COMMAND(ID_TOOLBARBUTTONDOWN,OnParkingDown)

	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ORDER, OnTvnSelchangedTreeParking)
END_MESSAGE_MAP()

void CDlgTemporaryParkingList::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_TREE_ORDER,m_treeParkingOrder);
}

BOOL CDlgTemporaryParkingList::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	// 	InitListCtrl();
	SetDlgItemText(IDC_STATIC,_T("Temporary Parkings"));
	SetWindowText("Temporary Parking Order");
	CRect rcToolbar;
	m_treeParkingOrder.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_toolBarParking.MoveWindow(rcToolbar);
	m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
	m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
	m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
	m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
	SetParkingOrderContent();
	InvalidateRect(NULL);
	return true;
}
int CDlgTemporaryParkingList::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	if (!m_toolBarParking.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_toolBarParking.LoadToolBar(IDR_TOOLBAR_TEMPPARKING))
	{
		return -1;
	}
	return 0;
}

void CDlgTemporaryParkingList::OnAddList()
{
	AltObjectVectorMapIter mapiter = m_TemporaryParkingVectorMap->begin();
	if(mapiter != m_TemporaryParkingVectorMap->end())
	{
		CDlgOccupiedSelectTemporaryParking dlgTemporaryParking(m_TemporaryParkingVectorMap);
		if(IDOK != dlgTemporaryParking.DoModal())
			return;
		int nTemporaryParkingID = dlgTemporaryParking.GetTemporaryParkingID();
		CString strTemporaryParking = dlgTemporaryParking.GetTemporaryParkingName();
		m_vParkingList.push_back(nTemporaryParkingID);
		SetParkingOrderContent();


	}
	else
		MessageBox("There is no Temporary Parking.");

}
void CDlgTemporaryParkingList::OnDelParking()
{
	HTREEITEM hSelItem=m_treeParkingOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int parkingIndex=(int)m_treeParkingOrder.GetItemData(hSelItem);
	m_vParkingList.erase(m_vParkingList.begin()+parkingIndex);
	HTREEITEM hNextItem=m_treeParkingOrder.GetNextItem(hSelItem,TVGN_NEXT);
	HTREEITEM hPreItem=m_treeParkingOrder.GetPrevSiblingItem(hSelItem);
	if (hNextItem!=NULL)
	{
		m_treeParkingOrder.SelectItem(hNextItem);
	}else if(hPreItem!=NULL)
	{
		m_treeParkingOrder.SelectItem(hPreItem);
	}
	while(hNextItem!=NULL)
	{
		m_treeParkingOrder.SetItemData(hNextItem,parkingIndex);
		hNextItem=m_treeParkingOrder.GetNextItem(hNextItem,TVGN_NEXT);
		parkingIndex++;
	}
	m_treeParkingOrder.DeleteItem(hSelItem);
	//   	SetParkingOrderContent();
}
void CDlgTemporaryParkingList::OnParkingUp()
{
	HTREEITEM hSelItem=m_treeParkingOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int selIndex=(int)m_treeParkingOrder.GetItemData(hSelItem);
	if (selIndex==0)
	{
		return;
	}
	ParkingList::iterator selParkingIter=m_vParkingList.begin()+selIndex;
	int tmpParking=(int)(*selParkingIter);
	*selParkingIter=*(selParkingIter-1);
	*(selParkingIter-1)=tmpParking;

	CString strPreItem,strSelItem;
	HTREEITEM hPreItem=m_treeParkingOrder.GetPrevSiblingItem(hSelItem);
	strPreItem=m_treeParkingOrder.GetItemText(hPreItem);
	strSelItem=m_treeParkingOrder.GetItemText(hSelItem);
	m_treeParkingOrder.SetItemText(hPreItem,strSelItem);
	m_treeParkingOrder.SetItemText(hSelItem,strPreItem);
	m_treeParkingOrder.SelectItem(hPreItem);

}
void CDlgTemporaryParkingList::OnParkingDown()
{
	HTREEITEM hSelItem=m_treeParkingOrder.GetSelectedItem();
	if (hSelItem==NULL)
	{
		return;
	}
	int selIndex=(int)m_treeParkingOrder.GetItemData(hSelItem);
	if (selIndex+1==(int)m_vParkingList.size())
	{
		return;
	}
	ParkingList::iterator selParkingIter=m_vParkingList.begin()+selIndex;
	int tmpParking=(int)(*selParkingIter);
	*selParkingIter=*(selParkingIter+1);
	*(selParkingIter+1)=tmpParking;

	CString strNextItem,strSelItem;
	HTREEITEM hNextItem=m_treeParkingOrder.GetNextItem(hSelItem,TVGN_NEXT);
	strNextItem=m_treeParkingOrder.GetItemText(hNextItem);
	strSelItem=m_treeParkingOrder.GetItemText(hSelItem);
	m_treeParkingOrder.SetItemText(hNextItem,strSelItem);
	m_treeParkingOrder.SetItemText(hSelItem,strNextItem);
	m_treeParkingOrder.SelectItem(hNextItem);

}
void CDlgTemporaryParkingList::OnTvnSelchangedTreeParking(NMHDR *pNMHDR, LRESULT *pResult)
{
	HTREEITEM hSelItem=m_treeParkingOrder.GetSelectedItem();
	if (hSelItem!=NULL && hSelItem!=m_hTreeRoot)
	{
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN);
		return;
	}
	if (m_hTreeRoot!=NULL && m_hTreeRoot==hSelItem)
	{
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL,false);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONUP,false);
		m_toolBarParking.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDOWN,false);
	}


}
void CDlgTemporaryParkingList::OnSize(UINT nType, int cx, int cy)
{

}

CDlgTemporaryParkingList::ParkingList& CDlgTemporaryParkingList::GetParkingList()
{
	return m_vParkingList;
}

void CDlgTemporaryParkingList::SetParkingOrderContent(void)
{
	m_treeParkingOrder.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	cni.nt=NT_NULL; 
	CCoolTree::InitNodeInfo(this,cni);

	m_hTreeRoot=m_treeParkingOrder.InsertItem("Parking Order",cni,FALSE);
	HTREEITEM hChild;
	ParkingList::iterator parkingIter=m_vParkingList.begin();
	CString strParkingName;
	int i=0;
	AltObjectVectorMapIter iter;
	for (;parkingIter!=m_vParkingList.end();parkingIter++)
	{
		int nFlag = 0;
		iter = m_TemporaryParkingVectorMap->begin();
		for(; iter != m_TemporaryParkingVectorMap->end(); iter++)
		{
			AltObjectVector& vec = iter->second;
			for(AltObjectVectorIter it = vec.begin();it != vec.end(); it++)
			{
				if(it->second == (int)(*parkingIter))
				{
					strParkingName = it->first;
					nFlag = 1;
					break;
				}
			}
			if(nFlag == 1)
				break;
		}

		hChild=m_treeParkingOrder.InsertItem(strParkingName,cni,FALSE,FALSE,m_hTreeRoot);
		m_treeParkingOrder.SetItemData(hChild,i);
		i++;
	}
	m_treeParkingOrder.Expand(m_hTreeRoot,TVE_EXPAND);

}
void CDlgTemporaryParkingList::addList(ParkingList &parkingList)
{
	m_vParkingList.insert(m_vParkingList.end(),parkingList.begin(),parkingList.end());
}

void CDlgTemporaryParkingList::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CXTResizeDialog::OnOK();
}



