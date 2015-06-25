// DlgTemporaryParkingCriteria.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "DlgTemporaryParkingCriteria.h"
#include ".\dlgtemporaryparkingcriteria.h"

#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/taxiway.h"
#include "../InputAirside/InputAirside.h"
#include "../Common/WinMsg.h"
#include <algorithm>


namespace
{
	const UINT ID_NEW_TAXIWAY = 11;
	const UINT ID_DEL_TAXIWAY = 12;
	const UINT ID_NEW_DATA = 21;
	const UINT ID_DEL_DATA = 22;
}
// CDlgTemporaryParkingCriteria dialog

IMPLEMENT_DYNAMIC(CDlgTemporaryParkingCriteria, CXTResizeDialog)
CDlgTemporaryParkingCriteria::CDlgTemporaryParkingCriteria(int nProjID, CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgTemporaryParkingCriteria::IDD, pParent)
{
	m_nProjID = nProjID;
	m_bSameTaxiway = TRUE;
	m_pTempParkingCriteriaList = new CTemporaryParkingCriteriaList();
	m_pTempParkingCriteriaList->setProjID(m_nProjID);
	m_nCurFirtNodeID = -1;
	m_nCurNodeCount = 0;
}

CDlgTemporaryParkingCriteria::~CDlgTemporaryParkingCriteria()
{
	if (m_pTempParkingCriteriaList)
	{
		delete m_pTempParkingCriteriaList;
		m_pTempParkingCriteriaList = NULL;
	}
}

void CDlgTemporaryParkingCriteria::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TAXIWAY, m_listTaxiway);
	DDX_Control(pDX, IDC_LIST_DATA, m_listData);
}


BEGIN_MESSAGE_MAP(CDlgTemporaryParkingCriteria, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_NEW_TAXIWAY, OnNewTaxiway)
	ON_COMMAND(ID_DEL_TAXIWAY, OnDelTaxiway)
	ON_COMMAND(ID_NEW_DATA, OnNewData)
	ON_COMMAND(ID_DEL_DATA, OnDelData)
	ON_LBN_SELCHANGE(IDC_LIST_TAXIWAY, OnLbnSelchangeListTaxiway)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedListData)

	ON_MESSAGE(WM_COMBO_IN_LISTBOX, OnMsgComboKillfocus)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
	ON_MESSAGE(WM_COLLUM_INDEX,OnDbClick)

	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBnClickedButtonSave)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CDlgTemporaryParkingCriteria message handlers

int CDlgTemporaryParkingCriteria::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	CreateToolBar();

	return 0;
}

BOOL CDlgTemporaryParkingCriteria::CreateToolBar()
{
	if(m_taxiwayToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0))&&
		!m_taxiwayToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_taxiwayToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_TAXIWAY);
	m_taxiwayToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_TAXIWAY);

	if(m_dataToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP,CRect(0,0,0,0),m_taxiwayToolBar.GetDlgCtrlID()+1)&&
		!m_dataToolBar.LoadToolBar(IDR_TOOLBAR_ADD_DEL))
	{
		AfxMessageBox("Cann't create toolbar!");
		return FALSE;
	}

	m_dataToolBar.GetToolBarCtrl().SetCmdID(0, ID_NEW_DATA);
	m_dataToolBar.GetToolBarCtrl().SetCmdID(1, ID_DEL_DATA);

	return TRUE;
}


BOOL CDlgTemporaryParkingCriteria::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	InitListCtrlEx();
	SetListCtrlEx();
	GetAllTaxiway();
	LoadData();

	SetResize(m_taxiwayToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_dataToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);

	SetResize(IDC_STATIC_TAXI, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_TAXIWAY, SZ_TOP_LEFT, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_LIST_DATA, SZ_TOP_CENTER, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);

	return TRUE;
}


void CDlgTemporaryParkingCriteria::InitToolBar()
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

void CDlgTemporaryParkingCriteria::InitListCtrlEx()
{
	DWORD dwStyle = m_listData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listData.SetExtendedStyle( dwStyle );

	m_strListOrientation.InsertAfter(m_strListOrientation.GetTailPosition(), "Longitudinal");
	m_strListOrientation.InsertAfter(m_strListOrientation.GetTailPosition(), "Angled");
}

void CDlgTemporaryParkingCriteria::SetListCtrlEx()
{
	const int nColCount = 4;

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
	char* columnLabel[] = {	"Start Pos", "End Pos", "Entry via", "Orientation" };

	int DefaultColumnWidth[] = { 135, 135, 135, 85 };
	int nColFormat[] = { LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN };

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT;
	lvc.csList = &m_strListNode;

	for (int i = 0; i < nColCount; i++)
	{
		lvc.fmt = nColFormat[i];
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if (i == 2)
		{
			lvc.csList = &m_strListEntryNode;
		}
		if (i == 3)
		{
			lvc.csList = &m_strListOrientation;
		}
		m_listData.InsertColumn(i, &lvc);
	}
}

void CDlgTemporaryParkingCriteria::GetAllTaxiway()
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
		//	strTmpTaxiway.Format("%s", taxiway.GetObjNameString());
			strTmpTaxiway.Format("%s", taxiway.GetMarking().c_str());
			m_strListTaxiway.InsertAfter(m_strListTaxiway.GetTailPosition(), strTmpTaxiway);
		}
	}

	m_nTaxiwayCount = m_vTaxiwaysIDs.size();
}


void CDlgTemporaryParkingCriteria::LoadData()
{
	m_pTempParkingCriteriaList->ReadData(m_nProjID);

	// delete the invalid data
	int nCount = m_pTempParkingCriteriaList->GetItemCount();	
	for (int i = 0; i < nCount; i++)
	{
		CTemporaryParkingCriteria* pCurTaxiway = m_pTempParkingCriteriaList->GetItem(i);		
		int nTaxiwayID = pCurTaxiway->GetTaxiwayID();

		std::vector<int>::iterator iter = std::find(m_vTaxiwaysIDs.begin(), m_vTaxiwaysIDs.end(), nTaxiwayID);
		if(iter == m_vTaxiwaysIDs.end())
		{
			m_pTempParkingCriteriaList->DelItem(pCurTaxiway);
			i--;
			nCount--;
			continue;
		}
		else
		{
			Taxiway taxiway;
			taxiway.ReadObject(nTaxiwayID);

			CString strTaxiway;
			strTaxiway.Format("%s", taxiway.GetMarking().c_str());			
			int nSel = m_listTaxiway.AddString(strTaxiway);            
			m_listTaxiway.SetItemData(nSel, nTaxiwayID);
		}

		int nCurCount = pCurTaxiway->GetItemCount();
		GetNodeListByTaxiwayID(nTaxiwayID);
		GetEntryNodeListByTaxiwayID(nTaxiwayID);
		for (int j = 0; j < nCurCount; j++)
		{
			CTemporaryParkingCriteriaData* pCurData = pCurTaxiway->GetItem(j);
			BOOL bStartExist = FALSE;
			BOOL bEndExist = FALSE;
			BOOL bEntryExist = FALSE;

			std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
			for (; iter != m_vNodeList.end(); ++iter)
			{

				if (pCurData->GetStartID() == (*iter).GetID())
				{
					bStartExist = TRUE;					
				}
				if (pCurData->GetEndID() == (*iter).GetID())
				{
					bEndExist = TRUE;
				}	
				if (pCurData->GetEntryID() == (*iter).GetID())
				{
					bEntryExist = TRUE;
				}
			}
			if (!bStartExist || !bEndExist || !bEntryExist)
			{
				pCurTaxiway->DelItem(pCurData);
				j--;
				nCurCount--;
				continue;
			}
		}
	}

	m_pTempParkingCriteriaList->SaveData(m_nProjID);
	m_listTaxiway.SetCurSel(0);
	OnLbnSelchangeListTaxiway();
}

void CDlgTemporaryParkingCriteria::UpdateToolBar()
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

void CDlgTemporaryParkingCriteria::GetNodeListByTaxiwayID(int nTaxiwayID)
{
	if (nTaxiwayID == -1)	// have no taxiway
	{
		m_vNodeList.clear();
		return;
	}

	// get taxiway name
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);


	IntersectionNode startNode;
	IntersectionNode endNode;
	m_vNodeListAll = taxiway.GetIntersectNodes();

	m_vNodeList.clear();
	std::vector<IntersectionNode>::iterator iter = m_vNodeListAll.begin();
	for (; iter != m_vNodeListAll.end(); ++iter)
	{

		if( iter->HasObject(nTaxiwayID) && iter->GetTaxiwayIntersectItemList().size() > 1 )
		{
			m_vNodeList.push_back((*iter));
		}	
	}

	m_nCurNodeCount = m_vNodeList.size();
	if (m_nCurNodeCount <= 0)
		return;

	m_strListNode.RemoveAll();
	iter = m_vNodeList.begin();
	m_nCurFirtNodeID = (*iter).GetID();

	for (; iter != m_vNodeList.end(); ++iter)
	{		
		std::vector<TaxiwayIntersectItem*> vTaxiItems = iter->GetTaxiwayIntersectItemList();
		for(int i=0;i< (int)vTaxiItems.size();i++)
		{
			TaxiwayIntersectItem* taxiItem = vTaxiItems[i];
			Taxiway * pOtherTaxiway = taxiItem->GetTaxiway();
			if( pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiwayID)
			{
				CString strObjName = pOtherTaxiway->GetMarking().c_str();
				CString strNode;
				if ((*iter).GetIndex() == 0)
				{
					strNode.Format("(%s)", strObjName);
				}
				else
				{
					strNode.Format("(%s) -%d", strObjName, (*iter).GetIndex());
				}
				m_strListNode.InsertAfter(m_strListNode.GetTailPosition(), strNode);	
			}
		}			
	}
}

LRESULT CDlgTemporaryParkingCriteria::OnDbClick(WPARAM wParam, LPARAM lParam)
{
	int nSel = m_listTaxiway.GetCurSel();
	int nCol = (int)lParam;
	if (nSel != LB_ERR && nCol == 2)
	{
		int nCurTaxiID = m_listTaxiway.GetItemData(nSel);
		LVCOLDROPLIST* pItem = m_listData.GetColumnStyle(nCol);
		GetEntryNodeListByTaxiwayID(nCurTaxiID);
		pItem->List.RemoveAll();
		POSITION pos = m_strListEntryNode.GetHeadPosition();
		while(pos)
		{
			CString csStr = m_strListEntryNode.GetAt( pos );
			pItem->List.InsertAfter(pItem->List.GetHeadPosition(),csStr);
			m_strListEntryNode.GetNext(pos);
		}
	}
	return TRUE;
}

void CDlgTemporaryParkingCriteria::GetEntryNodeListByTaxiwayID(int nTaxiwayID)
{
	if (nTaxiwayID == -1)	// have no taxiway
	{
		m_vNodeList.clear();
		return;
	}

	// get taxiway name
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);


	IntersectionNode* startNode = NULL;
	IntersectionNode* endNode = NULL;
	m_vNodeListAll = taxiway.GetIntersectNodes();

	m_vNodeList.clear();
	std::vector<IntersectionNode>::iterator iter = m_vNodeListAll.begin();
	for (; iter != m_vNodeListAll.end(); ++iter)
	{

		if( iter->HasObject(nTaxiwayID) && iter->GetTaxiwayIntersectItemList().size() > 1 )
		{
			m_vNodeList.push_back((*iter));
		}	
	}

	m_nCurNodeCount = m_vNodeList.size();
	if (m_nCurNodeCount <= 0)
		return;

	m_strListEntryNode.RemoveAll();
	int nSel = m_listData.GetCurSel();
	if (nSel != LB_ERR)
	{
		CTemporaryParkingCriteriaData* pCurData = (CTemporaryParkingCriteriaData*)m_listData.GetItemData(nSel);
		for (iter = m_vNodeList.begin(); iter != m_vNodeList.end(); ++iter)
		{
			if ((*iter).GetID() == pCurData->GetStartID())
			{
				startNode = &(*iter);
			}
			if ((*iter).GetID() == pCurData->GetEndID())
			{
				endNode = &(*iter);
			}
		}
	}
	else
	{
		iter = m_vNodeList.begin();
		m_nCurFirtNodeID = (*iter).GetID();
		startNode = &(*iter);
		endNode = &(*iter);
	}
	double dStart = startNode->GetDistance(nTaxiwayID);
	double dEnd = endNode->GetDistance(nTaxiwayID);
	for (iter = m_vNodeList.begin(); iter != m_vNodeList.end(); ++iter)
	{	
		double dMid = (*iter).GetDistance(nTaxiwayID);
		std::vector<TaxiwayIntersectItem*> vTaxiItems = iter->GetTaxiwayIntersectItemList();
		for(int i=0;i< (int)vTaxiItems.size();i++)
		{
			TaxiwayIntersectItem* taxiItem = vTaxiItems[i];
			Taxiway * pOtherTaxiway = taxiItem->GetTaxiway();
			if( pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiwayID)
			{
				if ((dStart <= dMid && dEnd >= dMid)\
					||(dStart >= dMid && dEnd <= dMid))
				{
					CString strObjName = pOtherTaxiway->GetMarking().c_str();
					CString strNode;
					if ((*iter).GetIndex() == 0)
					{
						strNode.Format("(%s)", strObjName);
					}
					else
					{
						strNode.Format("(%s) -%d", strObjName, (*iter).GetIndex());
					}
					m_strListEntryNode.InsertAfter(m_strListEntryNode.GetTailPosition(), strNode);	
				}
			}
		}			
	}
}

void CDlgTemporaryParkingCriteria::OnNewTaxiway()
{
	// data handling
	if (m_nTaxiwayCount <= 0)
	{
		MessageBox("You have not established the taxiway!");
		return;
	}

	pCurTaxiwayItem = new CTemporaryParkingCriteria();
	int nCurTaxiwayID = m_vTaxiwaysIDs.at(0);	

	pCurTaxiwayItem->SetTaxiwayID(nCurTaxiwayID);	
	m_pTempParkingCriteriaList->AddItem(pCurTaxiwayItem);

	// control handling
	m_listTaxiway.AddItem(&m_strListTaxiway);
	int nSel = m_listTaxiway.GetCurSel();
	m_listTaxiway.SetItemData(nSel, (DWORD_PTR)nCurTaxiwayID);
	SetCurNodList();
	m_bSameTaxiway = FALSE;

	UpdateToolBar();
}

void CDlgTemporaryParkingCriteria::SetCurNodList()
{
	int nSel = m_listTaxiway.GetCurSel();
	if (nSel == LB_ERR)
		return;
	int nCurTaxiwayID = m_listTaxiway.GetItemData(nSel);
	if (nCurTaxiwayID == -1)
	{
		return;
	}
	GetEntryNodeListByTaxiwayID(nCurTaxiwayID);
	GetNodeListByTaxiwayID(nCurTaxiwayID);
	SetListCtrlEx();
}

void CDlgTemporaryParkingCriteria::OnDelTaxiway()
{
	int nCurSel = m_listTaxiway.GetCurSel();
	if (nCurSel == 	LB_ERR)
		return;

	// data handling
	m_pTempParkingCriteriaList->DelItem(nCurSel);

	// control handling
	m_listTaxiway.DelItem(nCurSel);
	int nCount = m_listTaxiway.GetCount();
	int nSel = (nCount > nCurSel) ? nCurSel : (nCount - 1);
	m_listTaxiway.SetCurSel(nSel);

	OnLbnSelchangeListTaxiway();
}

void CDlgTemporaryParkingCriteria::OnNewData()
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
	CTemporaryParkingCriteriaData* pNewData = new CTemporaryParkingCriteriaData();
	pNewData->SetStartID(m_nCurFirtNodeID);
	pNewData->SetEndID(m_nCurFirtNodeID);
	pNewData->SetEntryID(m_nCurFirtNodeID);
	pNewData->SetOrientationType(Longitudinal);

	pCurTaxiwayItem->AddItem(pNewData);

	// control handling
	int nId = m_listData.GetItemCount();
	CString strTemp;
	CString strEntry;
	strTemp = m_strListNode.GetCount() > 0 ? m_strListNode.GetHead() : "No Node";
	strEntry = m_strListEntryNode.GetCount() > 0 ?m_strListEntryNode.GetHead():"No Node";
	m_listData.InsertItem(nId, strTemp);
	m_listData.SetItemText(nId, 1, strTemp);
	m_listData.SetItemText(nId, 2, strEntry);
	m_listData.SetItemText(nId, 3, m_strListOrientation.GetHead());

	m_listData.SetItemData(nId, (DWORD_PTR)pNewData);

	UpdateToolBar();
}

void CDlgTemporaryParkingCriteria::OnDelData()
{
	POSITION pos = m_listData.GetFirstSelectedItemPosition();
	if (pos != NULL)
	{
		while (pos)
		{
			int nItem = m_listData.GetNextSelectedItem(pos);
			CTemporaryParkingCriteriaData* pDelData = (CTemporaryParkingCriteriaData*)(m_listData.GetItemData(nItem));
			pCurTaxiwayItem->DelItem(pDelData);

			m_listData.DeleteItemEx(nItem);
		}
	}

	UpdateToolBar();
}


void CDlgTemporaryParkingCriteria::OnLbnSelchangeListTaxiway()
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

	pCurTaxiwayItem = m_pTempParkingCriteriaList->GetItem(nSel);

	// get taxiway name
	int nTaxiwayID = pCurTaxiwayItem->GetTaxiwayID();
	Taxiway taxiway;
	taxiway.ReadObject(nTaxiwayID);

	//CString strTaxiway;
	//strTaxiway.Format("%s", taxiway.GetObjNameString());

	for (int i = 0; i < pCurTaxiwayItem->GetItemCount(); i++)
	{
		CTemporaryParkingCriteriaData* pCurData = pCurTaxiwayItem->GetItem(i);

		CString strStart;
		CString strEnd;
		CString strEntry;
		CString strOrientation;

		std::vector<IntersectionNode>::iterator iter = m_vNodeList.begin();
		for (; iter != m_vNodeList.end(); ++iter)
		{
			CString strObjName;
			
			std::vector<TaxiwayIntersectItem*> vTaxiwayItems = iter->GetTaxiwayIntersectItemList();
			for(int itaxiIdx=0;itaxiIdx < (int)vTaxiwayItems.size();itaxiIdx++)
			{
				TaxiwayIntersectItem* taxiItem = vTaxiwayItems[itaxiIdx];
				Taxiway * pOtherTaxiway = taxiItem->GetTaxiway();
				if( pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiwayID )
				{
					strObjName = pOtherTaxiway->GetMarking().c_str();
				}
			}
			
			if (pCurData->GetStartID() == (*iter).GetID())
			{
				if ((*iter).GetIndex() == 0)
				{
					strStart.Format("(%s)", strObjName);
				}
				else
				{
					strStart.Format("(%s) -%d", strObjName, (*iter).GetIndex());
				}
			}
			if (pCurData->GetEndID() == (*iter).GetID())
			{
				if ((*iter).GetIndex() == 0)
				{
					strEnd.Format("(%s)", strObjName);
				}
				else
				{
					strEnd.Format("(%s) -%d", strObjName, (*iter).GetIndex());
				}
			}
			if (pCurData->GetEntryID() == (*iter).GetID())
			{
				if ((*iter).GetIndex() == 0)
				{
					strEntry.Format("(%s)", strObjName);
				}
				else
				{
					strEntry.Format("(%s) -%d", strObjName, (*iter).GetIndex());
				}
			}	

		}
		OrientationType emType = pCurData->GetOrientationType();
		int nIndex = (int)emType - 1;
		POSITION pos = m_strListOrientation.FindIndex(nIndex);
		strOrientation = m_strListOrientation.GetAt(pos);

		m_listData.InsertItem(i, strStart);
		m_listData.SetItemText(i, 1, strEnd);
		m_listData.SetItemText(i, 2, strEntry);
		m_listData.SetItemText(i, 3, strOrientation);
		m_listData.SetItemData(i, (DWORD_PTR)pCurData);
	}
}

void CDlgTemporaryParkingCriteria::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBar();
	*pResult = 0;
}

LRESULT CDlgTemporaryParkingCriteria::OnMsgComboKillfocus(WPARAM wParam, LPARAM lParam)
{
	int nCurTaxiwayID = m_vTaxiwaysIDs.at((int)lParam);
	pCurTaxiwayItem->SetTaxiwayID(nCurTaxiwayID);

	int nSel = m_listTaxiway.GetCurSel();
	m_listTaxiway.SetItemData(nSel, (DWORD_PTR)nCurTaxiwayID);
	return 0;
}

LRESULT CDlgTemporaryParkingCriteria::OnMsgComboChange(WPARAM wParam, LPARAM lParam)
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;	

	CTemporaryParkingCriteriaData* pCurData = (CTemporaryParkingCriteriaData*)m_listData.GetItemData(nCurSel);

	switch (nCurSubSel)
	{
	case 0: // Start
		{
			int nCurNodeStartID = (m_vNodeList.at(nComboxSel)).GetID();
			pCurData->SetStartID(nCurNodeStartID);
			CString strStart = m_listData.GetItemText(nCurSel,nCurSubSel);
			pCurData->SetEntryID(nCurNodeStartID);
			m_listData.SetItemText(nCurSel,2,strStart);
		}
		break;
	case 1: // End
		{
			int nCurNodeEndID = (m_vNodeList.at(nComboxSel)).GetID();
			pCurData->SetEndID(nCurNodeEndID);
		}
		break;
	case 2: // Entry
		{
			int nCurNodeEntryID = (m_vNodeList.at(nComboxSel)).GetID();
			pCurData->SetEntryID(nCurNodeEntryID);
		}
		break;
	case 3: // Orientation
		{
			OrientationType emType;
			if (nComboxSel == 0)
			{
				emType = Longitudinal;
			}
			else if (nComboxSel == 1)
			{
				emType = Angled;
			}
			pCurData->SetOrientationType(emType);
		}
		break;
	default:
		break;
	}	

	return 0;
}

void CDlgTemporaryParkingCriteria::OnBnClickedButtonSave()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pTempParkingCriteriaList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	OnOK();
}

void CDlgTemporaryParkingCriteria::OnBnClickedOk()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_pTempParkingCriteriaList->SaveData(m_nProjID);
		CADODatabase::CommitTransaction() ;

	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}
	
	OnOK();
}

void CDlgTemporaryParkingCriteria::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	OnCancel();
}
