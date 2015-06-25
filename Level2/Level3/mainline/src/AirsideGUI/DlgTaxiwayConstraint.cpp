// DlgTaxiwayConstraint.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTaxiwayConstraint.h"
#include ".\dlgtaxiwayconstraint.h"


#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
#include "../InputAirside/InputAirside.h"
#include "../Common/WinMsg.h"

#include "DlgWingspanAdjacancyConstraints.h"
#include "../Common/AirportDatabase.h"
#include <algorithm>
namespace
{
	const UINT ID_NEW_TAXIWAY = 11;
	const UINT ID_DEL_TAXIWAY = 12;
	const UINT ID_NEW_DATA = 21;
	const UINT ID_DEL_DATA = 22;
}

// CDlgTaxiwayConstraint dialog

IMPLEMENT_DYNAMIC(CDlgTaxiwayConstraint, CDialog)
CDlgTaxiwayConstraint::CDlgTaxiwayConstraint(int nProjID, TaxiwayConstraintType emType,CAirportDatabase* pAirportDB, 
											 CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTaxiwayConstraint::IDD, pParent)
{
	m_nProjID = nProjID;
	m_emType = emType;
	m_bSameTaxiway = TRUE;
	m_pAirportDB = pAirportDB;
	m_pTaxiwayConstraintList = new CTaxiwayConstraintList();
	m_pTaxiwayConstraintList->setProjID(m_nProjID);
	m_pTaxiwayConstraintList->SetConstraintType(m_emType);
	m_nCurFirtNodeID = -1;
	m_nCurNodeCount = 0;
}

CDlgTaxiwayConstraint::~CDlgTaxiwayConstraint()
{
	if (m_pTaxiwayConstraintList)
	{
		delete m_pTaxiwayConstraintList;
		m_pTaxiwayConstraintList = NULL;
	}
}

void CDlgTaxiwayConstraint::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_listData);
	DDX_Control(pDX, IDC_LIST_TAXIWAY, m_listTaxiway);
}


BEGIN_MESSAGE_MAP(CDlgTaxiwayConstraint, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_TAXIWAY, OnNewTaxiway)
	ON_COMMAND(ID_DEL_TAXIWAY, OnDelTaxiway)
	ON_COMMAND(ID_NEW_DATA, OnNewData)
	ON_COMMAND(ID_DEL_DATA, OnDelData)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BTN_WS_ADJCONS, OnBnClickedButtonAdjcancyConstraints)
	
	ON_LBN_SELCHANGE(IDC_LIST_TAXIWAY, OnLbnSelchangeListTaxiway)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListData)
	ON_MESSAGE(WM_COMBO_IN_LISTBOX, OnMsgComboKillfocus)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListData)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgTaxiwayConstraint message handlers

int CDlgTaxiwayConstraint::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	switch (m_emType)
	{
	case WeightConstraint:
		SetWindowText(_T("Taxiway Weight Constraint"));
		break;
	case WingSpanConstraint:
		SetWindowText(_T("Taxiway Wingspan Constraint"));
		break;
	default:
		break;
	}

	CreateToolBar();

	return 0;
}

BOOL CDlgTaxiwayConstraint::CreateToolBar()
{
	if(m_taxiwayToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_taxiwayToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_taxiwayToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_TAXIWAY);
	m_taxiwayToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_TAXIWAY);

	if(m_dataToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_dataToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	//-----------------------------------------------------------------/
	m_dataToolBar.SetDlgCtrlID(m_taxiwayToolBar.GetDlgCtrlID()+1);
	m_dataToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_DATA);
	m_dataToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_DATA);
	//-----------------------------------------------------------------/
	return TRUE;
}

BOOL CDlgTaxiwayConstraint::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitListCtrlEx();
	SetListCtrlEx();
	GetAllTaxiway();
	LoadData();	
	//-----------------------------------------------------------------/
	SetResize(m_taxiwayToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_dataToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_GROUPBOX_TAXIWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_TAXIWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_GROUPBOX_CONSTRAINT, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BTN_WS_ADJCONS, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	if(m_emType == WingSpanConstraint)
	{
		GetDlgItem(IDC_BTN_WS_ADJCONS)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_BTN_WS_ADJCONS)->ShowWindow(SW_HIDE);

	}
	
	
	//-----------------------------------------------------------------/

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}

void CDlgTaxiwayConstraint::InitToolBar()
{
	CRect rcToolbar;

	// set the position of the arrival delay trigger list toolbar	
	m_listTaxiway.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_taxiwayToolBar.MoveWindow(rcToolbar);

	m_taxiwayToolBar.GetToolBarCtrl().EnableButton(ID_NEW_TAXIWAY);
	m_taxiwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAY, FALSE);

	// set the position of the  arrival delay trigger toolbar
	m_listData.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	rcToolbar.left += 4;
	m_dataToolBar.MoveWindow(&rcToolbar);

	m_dataToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA, FALSE);
	m_dataToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA, FALSE);
}

void CDlgTaxiwayConstraint::InitListCtrlEx()
{
	DWORD dwStyle = m_listData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listData.SetExtendedStyle( dwStyle );
}

void CDlgTaxiwayConstraint::SetListCtrlEx()
{
	const int nColCount = 3;

	// delete all data on control
	if (m_listData.GetItemCount() > 0)
	{
		m_listData.DeleteAllItems();
	}	
	for (int i = nColCount-1; i >= 0; i--)
	{
		m_listData.DeleteColumn(i);
	}

	// set data on control
	char* columnLabel[] = {	"From", "To", "" };
	switch (m_emType)
	{
	case WeightConstraint:
		columnLabel[2] = "Max weight(lb)";
		break;
	case WingSpanConstraint:
		columnLabel[2] = "Max Wing Span(m)";
		break;
	default:
		break;
	}

	int DefaultColumnWidth[] = { 140, 140, 100 };
	int nColFormat[] = { LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_NUMBER };

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = &m_strListNode;	

	for (int i = 0; i < nColCount; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		m_listData.InsertColumn(i, &lvc);
	}
}


void CDlgTaxiwayConstraint::GetAllTaxiway()
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

void CDlgTaxiwayConstraint::LoadData()
{
	m_pTaxiwayConstraintList->ReadData(m_nProjID);

	// delete the repeated data
	int nCount = m_pTaxiwayConstraintList->GetItemCount();	
	for (int i = 0; i < nCount; i++)
	{
		CTaxiwayConstraint* pCurTaxiway = m_pTaxiwayConstraintList->GetItem(i);		
		int nTaxiwayID = pCurTaxiway->GetTaxiwayID();
		
		std::vector<int>::iterator iter = std::find(m_vTaxiwaysIDs.begin(), m_vTaxiwaysIDs.end(), nTaxiwayID);
		if(iter == m_vTaxiwaysIDs.end())
		{
            m_pTaxiwayConstraintList->DelItem(pCurTaxiway);
			i--;
			nCount--;
			continue;
		}
		else
		{
			Taxiway taxiway;
			taxiway.ReadObject(nTaxiwayID);

			CString strTaxiway;
			strTaxiway.Format("%s", taxiway.GetObjNameString());			
			int nSel = m_listTaxiway.AddString(strTaxiway);            
			m_listTaxiway.SetItemData(nSel, nTaxiwayID);
		}

		int nCurCount = pCurTaxiway->GetItemCount();
		GetNodeListByTaxiwayID(nTaxiwayID);
		for (int j = 0; j < nCurCount; j++)
		{
			CTaxiwayConstraintData* pCurData = pCurTaxiway->GetItem(j);
			BOOL bFormExist = FALSE;
			BOOL bToExist = FALSE;

			std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
			for (; iter != m_vNodeList.end(); ++iter)
			{
				//CString strObjName = ((*iter).m_pOtherObj)->GetObjNameString();

				if (pCurData->GetFromID() == (*iter).GetID())
				{
					bFormExist = TRUE;					
				}
				if (pCurData->GetToID() == (*iter).GetID())
				{
					bToExist = TRUE;
				}			
			}
			if (!bFormExist || !bToExist)
			{
				pCurTaxiway->DelItem(pCurData);
				j--;
				nCurCount--;
				continue;
			}
		}
	}

    m_pTaxiwayConstraintList->SaveData(m_nProjID);
	m_listTaxiway.SetCurSel(0);
	OnLbnSelchangeListTaxiway();
}

void CDlgTaxiwayConstraint::UpdateToolBar()
{
	if (m_listTaxiway.GetCount() > 0 && m_listTaxiway.GetCurSel() != -1)
	{
		m_taxiwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAY, TRUE);
		m_dataToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA, TRUE);
		
		if (m_listData.GetItemCount() > 0 && m_listData.GetCurSel() != -1)
		{
			m_dataToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA, TRUE);
		}
		else
		{
			m_dataToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA, FALSE);
		}
	}
	else
	{
		m_taxiwayToolBar.GetToolBarCtrl().EnableButton(ID_DEL_TAXIWAY, FALSE);
		m_dataToolBar.GetToolBarCtrl().EnableButton(ID_NEW_DATA, FALSE);
		m_dataToolBar.GetToolBarCtrl().EnableButton(ID_DEL_DATA, FALSE);
	}
}

void CDlgTaxiwayConstraint::GetNodeListByTaxiwayID(int nTaxiwayID)
{
	if (nTaxiwayID == -1)	// have no taxiway
	{
		m_vNodeList.clear();
		return;
	}

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
		/*CString strObjName;
		ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
		if(pObj)
			strObjName = pObj->GetObjNameString();

		CString strNode;
		strNode.Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);*/
		m_strListNode.InsertAfter( m_strListNode.GetTailPosition(), iter->GetName() );
	}
}

void CDlgTaxiwayConstraint::OnNewTaxiway()
{
	// data handling
	if (m_nTaxiwayCount <= 0)
	{
		MessageBox("You have not established the taxiway!");
		return;
	}

	pCurTaxiwayItem = new CTaxiwayConstraint();
	int nCurTaxiwayID = m_vTaxiwaysIDs.at(0);	

	pCurTaxiwayItem->SetTaxiwayID(nCurTaxiwayID);	
	m_pTaxiwayConstraintList->AddItem(pCurTaxiwayItem);

	// control handling
	m_listTaxiway.AddItem(&m_strListTaxiway);
	int nSel = m_listTaxiway.GetCurSel();
	m_listTaxiway.SetItemData(nSel, (DWORD_PTR)nCurTaxiwayID);
    SetCurNodList();
	m_bSameTaxiway = FALSE;

	UpdateToolBar();
}

void CDlgTaxiwayConstraint::SetCurNodList()
{
	int nSel = m_listTaxiway.GetCurSel();
	if (nSel == LB_ERR)
		return;
	int nCurTaxiwayID = m_listTaxiway.GetItemData(nSel);
	if (nCurTaxiwayID == -1)
	{
		return;
	}
	GetNodeListByTaxiwayID(nCurTaxiwayID);
	SetListCtrlEx();	
}

void CDlgTaxiwayConstraint::OnDelTaxiway()
{
	int nCurSel = m_listTaxiway.GetCurSel();
	if (nCurSel == 	LB_ERR)
		return;

	// data handling
	m_pTaxiwayConstraintList->DelItem(nCurSel);

	// control handling
	m_listTaxiway.DelItem(nCurSel);
	int nCount = m_listTaxiway.GetCount();
	int nSel = (nCount > nCurSel) ? nCurSel : (nCount - 1);
	m_listTaxiway.SetCurSel(nSel);

	OnLbnSelchangeListTaxiway();
}

void CDlgTaxiwayConstraint::OnNewData()
{
	if (m_nCurNodeCount <= 0)
	{
		MessageBox("The taxiway have not nodes!");
		return;
	}
	if (!m_bSameTaxiway)
	{
		m_listTaxiway.KillCombofocus();
        SetCurNodList();
        m_bSameTaxiway = TRUE;
	}

	// data handling
	CTaxiwayConstraintData* pNewData = new CTaxiwayConstraintData();
	pNewData->SetFromID(m_nCurFirtNodeID);
	pNewData->SetToID(m_nCurFirtNodeID);
	pNewData->SetMaxNumber(0);
	pCurTaxiwayItem->AddItem(pNewData);

	// control handling
	int nId = m_listData.GetItemCount();
	CString strTemp;
	strTemp = m_strListNode.GetCount() > 0 ? m_strListNode.GetHead() : "No Node";
	m_listData.InsertItem(nId, strTemp);
	m_listData.SetItemText(nId, 1, strTemp);
	m_listData.SetItemText(nId, 2, "0");
	m_listData.SetItemData(nId, (DWORD_PTR)pNewData);

	UpdateToolBar();
}

void CDlgTaxiwayConstraint::OnDelData()
{
	POSITION pos = m_listData.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_listData.GetNextSelectedItem(pos);
			CTaxiwayConstraintData* pDelData = (CTaxiwayConstraintData*)(m_listData.GetItemData(nItem));
			pCurTaxiwayItem->DelItem(pDelData);
			m_listData.DeleteItemEx(nItem);
		}
	}

	UpdateToolBar();
}

void CDlgTaxiwayConstraint::OnLbnSelchangeListTaxiway()
{
	UpdateToolBar();
	int nSel = m_listTaxiway.GetCurSel();
	if (nSel == LB_ERR)
	{
		m_listData.DeleteAllItems();
		return;
	}
	SetCurNodList();
	m_bSameTaxiway = TRUE;

	pCurTaxiwayItem = m_pTaxiwayConstraintList->GetItem(nSel);

	// get taxiway name
	int nTaxiwayID = pCurTaxiwayItem->GetTaxiwayID();
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);

	CString strTaxiway;
	strTaxiway.Format("%s", taxiway.GetObjNameString());

	for (int i = 0; i < pCurTaxiwayItem->GetItemCount(); i++)
	{
		CTaxiwayConstraintData* pCurData = pCurTaxiwayItem->GetItem(i);

		CString strFrom;
		CString strTo;
		CString strMaxNumber;

		std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
		for (; iter != m_vNodeList.end(); ++iter)
		{
			/*CString strObjName;
			ALTObject * pObj = (*iter).GetOtherObject(nTaxiwayID);
			if(pObj)
				strObjName = pObj->GetObjNameString();*/

			if (pCurData->GetFromID() == (*iter).GetID())
			{
				strFrom = iter->GetName(); //Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
			}
			if (pCurData->GetToID() == (*iter).GetID())
			{
				strTo = iter->GetName(); //Format("(%s) & (%s)-%d", strTaxiway, strObjName, ((*iter).GetIndex())+1);
			}			
		}
		strMaxNumber.Format("%d", pCurData->GetMaxNumber());

		m_listData.InsertItem(i, strFrom);
		m_listData.SetItemText(i, 1, strTo);
		m_listData.SetItemText(i, 2, strMaxNumber);
		m_listData.SetItemData(i, (DWORD_PTR)pCurData);
	}
}


void CDlgTaxiwayConstraint::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    UpdateToolBar();
	*pResult = 0;
}

LRESULT CDlgTaxiwayConstraint::OnMsgComboKillfocus(WPARAM wParam, LPARAM lParam)
{
	int nCurTaxiwayID = m_vTaxiwaysIDs.at((int)lParam);
	pCurTaxiwayItem->SetTaxiwayID(nCurTaxiwayID);

	int nSel = m_listTaxiway.GetCurSel();
	m_listTaxiway.SetItemData(nSel, (DWORD_PTR)nCurTaxiwayID);
	return 0;
}

LRESULT CDlgTaxiwayConstraint::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
	return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;	

	CTaxiwayConstraintData* pCurData = (CTaxiwayConstraintData*)m_listData.GetItemData(nCurSel);

	switch (nCurSubSel)
	{
	case 0: // From
		{
			int nCurNodeFromID = (m_vNodeList.at(nComboxSel)).GetID();
			pCurData->SetFromID(nCurNodeFromID);
		}
		break;
	case 1: // To
		{
			int nCurNodeToID = (m_vNodeList.at(nComboxSel)).GetID();
			pCurData->SetToID(nCurNodeToID);
		}
		break;
	default:
		break;
	}	

	return 0;
}

void CDlgTaxiwayConstraint::OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);

	int nCurSel = pDispInfo->item.iItem;
	CTaxiwayConstraintData* pCurData = (CTaxiwayConstraintData*)m_listData.GetItemData(nCurSel);
	if (pDispInfo->item.iSubItem == 2)
	{
		LONG lMaxNumber = atol(pDispInfo->item.pszText);
		pCurData->SetMaxNumber(lMaxNumber);
	}

	*pResult = 0;
}

void CDlgTaxiwayConstraint::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pTaxiwayConstraintList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
}

void CDlgTaxiwayConstraint::OnBnClickedOk()
{
	OnBnClickedButtonSave();
	OnOK();
}

void CDlgTaxiwayConstraint::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}

void CDlgTaxiwayConstraint::OnBnClickedButtonAdjcancyConstraints()
{
	if(m_emType == WingSpanConstraint)
	{
		CDlgWingspanAdjacancyConstraints dlg(m_nProjID,m_pAirportDB);
		dlg.DoModal();
	}
	
}