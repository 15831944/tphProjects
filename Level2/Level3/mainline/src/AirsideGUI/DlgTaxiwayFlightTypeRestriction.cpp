// DlgTaxiwayFlightTypeRestriction.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include ".\dlgtaxiwayflighttyperestriction.h"

#include "..\InputAirside\InputAirside.h"

#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
#include "../Database/DBElementCollection_Impl.h"

// CDlgTaxiwayFlightTypeRestriction dialog

static const UINT ID_NEW_FLTYPERET = 11;
static const UINT ID_DEL_FLTYPERET = 12;
static const UINT ID_EDIT_FLTYPERET = 13;

static const UINT TAXIWAY = 1;
static const UINT FROMPOS = 2;
static const UINT TOPOS = 3;

IMPLEMENT_DYNAMIC(CDlgTaxiwayFlightTypeRestriction, CXTResizeDialog)
CDlgTaxiwayFlightTypeRestriction::CDlgTaxiwayFlightTypeRestriction(int nProjID,PSelectFlightType pSelectFlightType, CAirportDatabase *pAirPortdb, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTaxiwayFlightTypeRestriction::IDD, pParent)
	,m_nProjID(nProjID)
	,m_pSelectFlightType(pSelectFlightType)
{
	m_pFlightTypeRestrictionList = new CFlightTypeRestrictionList;
	m_pFlightTypeRestriction = NULL;
	m_pFlightTypeRestrictionList->SetProjID(m_nProjID);
	m_pFlightTypeRestrictionList->SetAirportDatabase(pAirPortdb);
	m_nTaxiwayCount = m_nCurNodeCount =0;
	m_strListTaxiway.RemoveAll();
	m_strListNode.RemoveAll();
	m_vTaxiwaysIDs.clear();
	m_vNodeList.clear();
}

CDlgTaxiwayFlightTypeRestriction::~CDlgTaxiwayFlightTypeRestriction()
{
}

void CDlgTaxiwayFlightTypeRestriction::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_MyList);
}


BEGIN_MESSAGE_MAP(CDlgTaxiwayFlightTypeRestriction, CXTResizeDialog)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_NEW_FLTYPERET, OnNewFlightType)
	ON_COMMAND(ID_DEL_FLTYPERET, OnDelFlightType)
	ON_COMMAND(ID_EDIT_FLTYPERET, OnEditFlightType)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListData)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
END_MESSAGE_MAP()


// CDlgTaxiwayFlightTypeRestriction message handlers


BOOL CDlgTaxiwayFlightTypeRestriction::OnInitDialog(void)
{
	CXTResizeDialog::OnInitDialog();
	InitToolBar();
	InitListCtrlEx();
	GetAllTaxiway();
	SetListCtrlEx(FALSE);
	ResizeDlg();
	LoadListData();

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

    return TRUE;
}

int CDlgTaxiwayFlightTypeRestriction::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (!m_MyToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_MyToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL_EDIT))
	{
		return -1;
	}

	CToolBarCtrl& fltToolbar = m_MyToolBar.GetToolBarCtrl();
	fltToolbar.SetCmdID(0, ID_NEW_FLTYPERET);
	fltToolbar.SetCmdID(1, ID_DEL_FLTYPERET);
	fltToolbar.SetCmdID(2, ID_EDIT_FLTYPERET);	
	return 0;
}

void CDlgTaxiwayFlightTypeRestriction::OnDestroy()
{
	CXTResizeDialog::OnDestroy();

	// TODO: Add your message handler code here
	if(m_pFlightTypeRestrictionList != NULL)
	{
		delete m_pFlightTypeRestrictionList;
		m_pFlightTypeRestrictionList = NULL;
	}

	//if(m_pFlightTypeRestriction != NULL)
	//{
	//	delete m_pFlightTypeRestriction;
	//	m_pFlightTypeRestriction = NULL;
	//}
	m_strListTaxiway.RemoveAll();
	m_strListNode.RemoveAll();
}



void CDlgTaxiwayFlightTypeRestriction::InitListCtrlEx()
{

	if(m_MyList.m_hWnd == NULL)
	{
		return;
	}
	DWORD dwStyle = m_MyList.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	m_MyList.SetExtendedStyle(dwStyle);

}

void CDlgTaxiwayFlightTypeRestriction::SetListCtrlEx(BOOL bReset)
{
	const int nColCount = 4;

	//set item data
	char* columnLabel[] = {	"Flight Type", "Taxiway", "From" ,"To"};

	int DefaultColumnWidth[] = { 140, 140, 140, 140 };
	int nColFormat[] = { LVCFMT_NOEDIT, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN };

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;

	CStringList strList;
	strList.RemoveAll();
	lvc.csList = &strList;
	if(!bReset)
	{
		
		lvc.fmt = nColFormat[0];
		lvc.pszText = columnLabel[0];
		lvc.cx = 140;		
		m_MyList.InsertColumn(0, &lvc);

		lvc.fmt = nColFormat[1];
		lvc.pszText = columnLabel[01];
		lvc.cx = 140;	
		lvc.csList = &m_strListTaxiway;
		m_MyList.InsertColumn(1, &lvc);

		lvc.fmt = nColFormat[2];
		lvc.pszText = columnLabel[2];
		lvc.cx = 140;
		lvc.csList = &strList;
		m_MyList.InsertColumn(2, &lvc);

		lvc.fmt = nColFormat[3];
		lvc.pszText = columnLabel[3];
		lvc.cx = 140;
		lvc.csList = &strList;
		m_MyList.InsertColumn(3, &lvc);	
	}
	else
	{
		lvc.csList = &m_strListNode;

		LVCOLDROPLIST *plvcs;
		plvcs = m_MyList.GetColumnStyle(2);
		plvcs->List.RemoveAll();
		plvcs->List.AddTail(lvc.csList);


		plvcs = m_MyList.GetColumnStyle(3);
		plvcs->List.RemoveAll();
		plvcs->List.AddTail(lvc.csList);	
	}	
}


void CDlgTaxiwayFlightTypeRestriction::InitToolBar()
{
	CRect rcToolBar;
	GetDlgItem(IDC_STATIC)->GetWindowRect(&rcToolBar);
	ScreenToClient(&rcToolBar);
	rcToolBar.top +=10;
	rcToolBar.left +=3;
	m_MyToolBar.MoveWindow(&rcToolBar);

	m_MyToolBar.GetToolBarCtrl().EnableButton(ID_NEW_FLTYPERET);
	m_MyToolBar.GetToolBarCtrl().EnableButton(ID_DEL_FLTYPERET,FALSE);
	m_MyToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTYPERET,FALSE);
}

void CDlgTaxiwayFlightTypeRestriction::ResizeDlg()
{
	SetResize(m_MyToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_STATIC, SZ_TOP_LEFT , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
}

void CDlgTaxiwayFlightTypeRestriction::GetAllTaxiway()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	std::vector<int>::iterator iterAirportID = vAirportIds.begin();
	for (; iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, m_vTaxiwaysIDs);

		std::vector<int>::iterator iterTaxiwayID = m_vTaxiwaysIDs.begin();
		for (; iterTaxiwayID != m_vTaxiwaysIDs.end(); ++iterTaxiwayID)
		{
			Taxiway taxiway;
			taxiway.ReadObject(*iterTaxiwayID);

			CString strTmpTaxiway;
			strTmpTaxiway.Format("%s", taxiway.GetObjNameString());
			m_strListTaxiway.InsertAfter(m_strListTaxiway.GetTailPosition(), strTmpTaxiway);
		}
	}
	m_nTaxiwayCount = m_vTaxiwaysIDs.size();
}

void CDlgTaxiwayFlightTypeRestriction::GetNodeListByTaxiwayID(int nTaxiwayID)
{
	if (nTaxiwayID == -1)	// have no taxiway
	{
		m_vNodeList.clear();
		return;
	}

	int nCnt = m_vTaxiwaysIDs.size();

	// get taxiway name
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);

	CString strTaxiway;
	strTaxiway.Format("%s", taxiway.GetObjNameString());

	// get the list of intersect node
	m_vNodeList.clear();
	m_vNodeList = taxiway.GetIntersectNodes();
	m_nCurNodeCount = m_vNodeList.size();
	if (m_nCurNodeCount <= 0)
		return;

	m_strListNode.RemoveAll();
	std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
	m_nCurFirtNodeID = (*iter).GetID();
	for (; iter != m_vNodeList.end(); ++iter)
	{
		//CString strObjName;
		/*ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
		if(pObj)
			strObjName = pObj->GetObjNameString();

		CString strNode;
		strNode.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);*/
		m_strListNode.InsertAfter( m_strListNode.GetTailPosition(), iter->GetName() );
	}
}

void CDlgTaxiwayFlightTypeRestriction::UpdateToolBar()
{
	if (m_MyList.GetItemCount() > 0 && m_MyList.GetCurSel() != -1)
	{
		m_MyToolBar.GetToolBarCtrl().EnableButton(ID_DEL_FLTYPERET, TRUE);
		m_MyToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTYPERET, TRUE);
	}
	else
	{
		m_MyToolBar.GetToolBarCtrl().EnableButton(ID_DEL_FLTYPERET, FALSE);
		m_MyToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_FLTYPERET, FALSE);
	}	

}

void CDlgTaxiwayFlightTypeRestriction::OnNewFlightType()
{
	if (m_pSelectFlightType == NULL)
	{
		return;
	}

	FlightConstraint fltType = (*m_pSelectFlightType)(NULL);

	m_pFlightTypeRestriction = new CFlightTypeRestriction();
	m_pFlightTypeRestriction->SetFltType(fltType);
	int nTaxiwayID = m_vTaxiwaysIDs.at(0);
	GetNodeListByTaxiwayID(nTaxiwayID);
	SetListCtrlEx(TRUE);
	m_pFlightTypeRestriction->SetTaxiwayID(nTaxiwayID);
	m_pFlightTypeRestriction->SetFromPos(0);
	m_pFlightTypeRestriction->SetToPos(0);	
	m_pFlightTypeRestrictionList->AddNewItem(m_pFlightTypeRestriction);


	char szBuffer[1024] = {0};
	fltType.screenPrint(szBuffer);
	int nIndex = m_MyList.GetItemCount();
	m_MyList.InsertItem(nIndex,szBuffer);
	CString strTaxiway = _T("");
	strTaxiway = m_strListTaxiway.GetCount() > 0 ? m_strListTaxiway.GetHead() : _T("");
	
	m_MyList.SetItemText(nIndex, 1,strTaxiway);
	CString strPos = _T("");
	m_MyList.SetItemText(nIndex, 2,strPos);
	m_MyList.SetItemText(nIndex, 3,strPos);
	m_MyList.SetItemData(nIndex, (DWORD_PTR)m_pFlightTypeRestriction);

	UpdateToolBar();

}

void CDlgTaxiwayFlightTypeRestriction::OnDelFlightType()
{
	POSITION pos = m_MyList.GetFirstSelectedItemPosition();	
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_MyList.GetNextSelectedItem(pos);
			CFlightTypeRestriction *pDelData = (CFlightTypeRestriction*)(m_MyList.GetItemData(nItem));
			m_pFlightTypeRestrictionList->DeleteItem(pDelData);

			m_MyList.DeleteItemEx(nItem);
		}
	}
	UpdateToolBar();
}
void CDlgTaxiwayFlightTypeRestriction::OnEditFlightType()
{
	UpdateToolBar();
}

BOOL CDlgTaxiwayFlightTypeRestriction::LoadListData()
{	
	m_pFlightTypeRestrictionList->ReadData(m_nProjID);
	int nCount = m_pFlightTypeRestrictionList->GetElementCount();
	CString strTempValue;
	for (int i = 0; i < nCount; i++)
	{
		CFlightTypeRestriction* pFlightTypeRestriction = m_pFlightTypeRestrictionList->GetItem(i);

		FlightConstraint FltType = pFlightTypeRestriction->GetFltType();
		char szBuffer[1024] = {0};
		FltType.screenPrint(szBuffer);
		m_MyList.InsertItem(i,szBuffer);

		int nTaxiwayID = pFlightTypeRestriction->GetTaxiwayID();
		CString strTaxiway;
		Taxiway taxiway;
		taxiway.ReadObject(nTaxiwayID);
		strTaxiway.Format("%s", taxiway.GetObjNameString());
		m_MyList.SetItemText(i,1,strTaxiway);

		
		GetNodeListByTaxiwayID(nTaxiwayID);
		SetListCtrlEx(TRUE);
		CString strFromPos,  strToPos;	
		std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
		for (; iter != m_vNodeList.end(); ++iter)
		{
			/*CString strObjName;
			ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
			if(pObj)
			{
				strObjName = pObj->GetObjNameString();
			}*/

			if (pFlightTypeRestriction->GetFromPos() == (*iter).GetID())
			{
				strFromPos = iter->GetName();//Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
			}
			if (pFlightTypeRestriction->GetToPos() == (*iter).GetID())
			{
				strToPos = iter->GetName(); //Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
			}
		}

		m_MyList.SetItemText(i,2,strFromPos);
		m_MyList.SetItemText(i,3,strToPos);		
		m_MyList.SetItemData((int)i, (DWORD_PTR)pFlightTypeRestriction);
	}
	return FALSE;
}

void CDlgTaxiwayFlightTypeRestriction::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if((pNMLV->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pNMLV->uOldState & LVIS_SELECTED) && !(pNMLV->uNewState & LVIS_SELECTED))
		return;

	if(pNMLV->uOldState & LVIS_SELECTED)
		return;

	UpdateToolBar();
	int nCurSel = pNMLV->iItem;
	CFlightTypeRestriction* pFlightTypeRestriction = m_pFlightTypeRestrictionList->GetItem(nCurSel);
	int nTaxiwayID = pFlightTypeRestriction->GetTaxiwayID();
	GetNodeListByTaxiwayID(nTaxiwayID);
	SetListCtrlEx(TRUE);
	*pResult = 0;
}

void CDlgTaxiwayFlightTypeRestriction::OnBnClickedButtonSave()
{
	// TODO: Add your control notification handler code here
	if(!IsSamePos())
	{
		return;
	}
	m_pFlightTypeRestrictionList->SaveData(m_nProjID);
}

void CDlgTaxiwayFlightTypeRestriction::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	if(!IsSamePos())
	{
		return;
	}
	m_pFlightTypeRestrictionList->SaveData(m_nProjID);
	OnOK();
}

void CDlgTaxiwayFlightTypeRestriction::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

LRESULT CDlgTaxiwayFlightTypeRestriction::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;


	m_pFlightTypeRestriction = (CFlightTypeRestriction*)m_MyList.GetItemData(nCurSel);
	
	switch (nCurSubSel)
	{
	case TAXIWAY: 
		{
			int nCurTaxiwayID = (m_vTaxiwaysIDs.at(nComboxSel));
			m_pFlightTypeRestriction->SetTaxiwayID(nCurTaxiwayID);			
			GetNodeListByTaxiwayID(nCurTaxiwayID);
			m_pFlightTypeRestriction->SetFromPos(m_nCurFirtNodeID); // must assign the from to node id manually
			m_pFlightTypeRestriction->SetToPos(m_nCurFirtNodeID);
			SetListCtrlEx(TRUE);
			CString strPos = m_strListNode.GetCount() > 0 ? m_strListNode.GetHead() : _T("");
			m_MyList.SetItemText(nCurSel, 2, strPos);
			m_MyList.SetItemText(nCurSel, 3, strPos);

		}
		break;
	case FROMPOS: 
		{
			int nCurNodeFromID = (m_vNodeList.at(nComboxSel)).GetID();
			m_pFlightTypeRestriction->SetFromPos(nCurNodeFromID);
		}
		break;
	case TOPOS:
		{
			int nCurNodeToID = (m_vNodeList.at(nComboxSel)).GetID();	
			m_pFlightTypeRestriction->SetToPos(nCurNodeToID);
		}
		break;
	default:
		break;
	}	
	
	return 0;
}

BOOL CDlgTaxiwayFlightTypeRestriction::IsSamePos()
{
	int nCnt = m_MyList.GetItemCount();
	CString strFromPos, strToPos;
	for(int i = 0; i< nCnt; i++)
	{
		strFromPos = m_MyList.GetItemText(i,2);
		strToPos = m_MyList.GetItemText(i,3);
		if(strFromPos == _T(""))
		{
			CString strMsg;
			strMsg.Format(_T("%s%d%s"), "Please select From point in ",i+1, " row!");
			MessageBox(strMsg);
			return FALSE;
		}
		if(strToPos == _T(""))
		{
			CString strMsg;
			strMsg.Format(_T("%s%d%s"), "Please select To point in ",i+1, " row!");
			MessageBox(strMsg);
			return FALSE;
		}
		if(strFromPos == strToPos)
		{
			CString strMsg;
			strMsg.Format(_T("%s%d%s"), "You can not select same point in ",i+1, " row!");
			MessageBox(strMsg);	
			return FALSE;
		}
	}
	return TRUE;
}