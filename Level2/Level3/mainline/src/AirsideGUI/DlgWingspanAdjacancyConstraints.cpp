// DlgWingspanAdjacancyConstraints.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgWingspanAdjacancyConstraints.h"
#include ".\dlgwingspanadjacancyconstraints.h"
#include "DlgTaxiwaySelect.h"
#include "InputAirside/InputAirside.h"
#include "InputAirside/ALTAirport.h"
#include "InputAirside/Taxiway.h"
#include "../Common/WinMsg.h"


// CDlgWingspanAdjacancyConstraints dialog

IMPLEMENT_DYNAMIC(CDlgWingspanAdjacancyConstraints, CDialog)
CDlgWingspanAdjacancyConstraints::CDlgWingspanAdjacancyConstraints(int nProjectID,CAirportDatabase* pAirportDB,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_WINGSPAN_ADJCONS, pParent)
	,m_nProjID(nProjectID)
	,m_pAirportDB(pAirportDB)
	,m_vData(pAirportDB)
{
	
}

CDlgWingspanAdjacancyConstraints::~CDlgWingspanAdjacancyConstraints()
{
	std::vector<Taxiway *>::iterator iter = m_vTaxiway.begin();
	for (; iter != m_vTaxiway.end(); ++iter)
	{
		delete *iter;
	}
	m_vTaxiway.clear();
}

void CDlgWingspanAdjacancyConstraints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_wndListCtrl);

}


BEGIN_MESSAGE_MAP(CDlgWingspanAdjacancyConstraints, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBAR_WINGSPAN_ADJCONS_ADD, OnCmdNewItem)
	ON_COMMAND(ID_TOOLBAR_WINGSPAN_ADJCONS_DEL, OnCmdDeleteItem)
	ON_COMMAND(ID_TOOLBAR_WINGSPAN_ADJCONS_EDIT, OnCmdEditItem)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListContents)

	ON_MESSAGE(WM_COLLUM_INDEX, OnCollumnIndex)
	ON_MESSAGE(WM_INPLACE_COMBO_KILLFOUCUS, OnMsgComboChange)
END_MESSAGE_MAP()


// CDlgWingspanAdjacancyConstraints message handlers

int CDlgWingspanAdjacancyConstraints::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_WINGSPAN_ADJCONS))
	{
		return -1;
	}
	return 0;
}

BOOL CDlgWingspanAdjacancyConstraints::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(_T("Wingspan Adjacency Constraints"));

	// TODO:  Add extra initialization here
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow(FALSE);
	//list
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	//load data for taxiway adjacency
	m_vData.LoadData(m_nProjID);
	// set the layout of child controls
	RefreshLayout();

	// init the list control
	InitListControl();

	// set the toolbar button state
	UpdateToolBarState();

	GetAllTaxiway();

	FillListControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgWingspanAdjacancyConstraints::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	RefreshLayout(cx, cy);
}

void CDlgWingspanAdjacancyConstraints::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	OnSave();
	CDialog::OnOK();
}

void CDlgWingspanAdjacancyConstraints::UpdateToolBarState()
{
	if (!::IsWindow(m_wndToolBar.m_hWnd) || !::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	BOOL bEnable = TRUE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_WINGSPAN_ADJCONS_ADD, bEnable);

	// delete & edit button
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_WINGSPAN_ADJCONS_DEL, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_WINGSPAN_ADJCONS_EDIT, bEnable);
}
void CDlgWingspanAdjacancyConstraints::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	UpdateToolBarState();

	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;
	CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nSelItem);
	if (pConstraint ==  NULL)
		return;




	{
		CStringList lstFirstNode;
		GetNodeListByTaxiway(pConstraint->m_firstaxiway.m_pTaxiway,lstFirstNode);

		LVCOLDROPLIST* pDroplistFrom =  m_wndListCtrl.GetColumnStyle(1);
		LVCOLDROPLIST* pDroplistTo =  m_wndListCtrl.GetColumnStyle(2);
		if(pDroplistFrom && pDroplistTo)
		{
			pDroplistFrom->List.RemoveAll();
			pDroplistTo->List.RemoveAll();
			POSITION pos;
			for( pos = lstFirstNode.GetHeadPosition(); pos;)
			{
				pDroplistFrom->List.AddTail(lstFirstNode.GetAt(pos));
				pDroplistTo->List.AddTail(lstFirstNode.GetAt(pos));
				lstFirstNode.GetNext( pos );
			}
		}
	}


	{	
		CStringList lstSecondNode;	
		GetNodeListByTaxiway(pConstraint->m_secondTaxiway.m_pTaxiway,lstSecondNode);
		LVCOLDROPLIST* pDroplistFrom =  m_wndListCtrl.GetColumnStyle(7);
		LVCOLDROPLIST* pDroplistTo =  m_wndListCtrl.GetColumnStyle(8);
		if(pDroplistFrom && pDroplistTo)
		{
			pDroplistFrom->List.RemoveAll();
			pDroplistTo->List.RemoveAll();
			POSITION pos;
			for( pos = lstSecondNode.GetHeadPosition(); pos;)
			{
				pDroplistFrom->List.AddTail(lstSecondNode.GetAt(pos));
				pDroplistTo->List.AddTail(lstSecondNode.GetAt(pos));
				lstSecondNode.GetNext( pos );
			}
		}
	}


}

void CDlgWingspanAdjacancyConstraints::GetNodeListByTaxiway(Taxiway& pTaxiway, CStringList& lstNode)
{
	lstNode.RemoveAll();

	IntersectionNodeList vNodeList = pTaxiway.GetIntersectNodes();
	std::vector<IntersectionNode>::iterator iterNode = vNodeList.begin();
	for (; iterNode != vNodeList.end(); ++iterNode)
	{
		CString strNodeName = iterNode->GetName();
		lstNode.InsertAfter( lstNode.GetTailPosition(),  strNodeName);
	}
}

void CDlgWingspanAdjacancyConstraints::RefreshLayout( int cx /*= -1*/, int cy /*= -1*/ )
{
	if (cx == -1 && cy == -1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		cx = rectClient.Width();
		cy = rectClient.Height();
	}

	CWnd* pWndSave = GetDlgItem(IDC_BUTTON_SAVE);
	CWnd* pWndOK = GetDlgItem(IDOK);
	CWnd* pWndCancel = GetDlgItem(IDCANCEL);
	if (pWndSave == NULL || pWndOK == NULL || pWndCancel == NULL)
		return;

	CRect rcCancel;
	pWndCancel->GetClientRect(&rcCancel);
	pWndCancel->MoveWindow(cx - rcCancel.Width() - 10,
		cy - rcCancel.Height() - 10, 
		rcCancel.Width(), 
		rcCancel.Height());

	CRect rcOK;
	pWndOK->GetClientRect(&rcOK);
	pWndOK->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - 20,
		cy - rcOK.Height() - 10, 
		rcOK.Width(),
		rcOK.Height());

	CRect rcSave;
	pWndSave->GetClientRect(&rcSave);
	pWndSave->MoveWindow(cx - rcCancel.Width() - rcOK.Width() - rcSave.Width() - 30,
		cy - rcSave.Height() - 10, 
		rcSave.Width(),
		rcSave.Height());

	CWnd* pGroupBox = GetDlgItem(IDC_STATIC_FRAME);
	if (pGroupBox == NULL)
		return;

	pGroupBox->MoveWindow(10, 5, cx - 20, cy - rcOK.Height() - 27);

	m_wndListCtrl.MoveWindow(10, 35, cx - 20, cy - rcOK.Height() - 57);

	if (!::IsWindow(m_wndToolBar.m_hWnd))
		return;
	m_wndToolBar.MoveWindow(11, 11, cx - 25, 22);

	Invalidate();
}

void CDlgWingspanAdjacancyConstraints::InitListControl()
{
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	// set the list header contents;
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("If Taxiway");
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	//from
	lvc.pszText = _T("From");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1, &lvc);

	//to
	lvc.pszText = _T("To");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Criteria");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(3, &lvc);

	strList.RemoveAll();
	lvc.pszText = _T("Min Value");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.pszText = _T("Max Value");
	lvc.cx = 65;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(5, &lvc);

	lvc.pszText = _T("Then Taxiway");
	lvc.cx = 120;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(6, &lvc);


	//from
	lvc.pszText = _T("From");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(7, &lvc);

	//to
	lvc.pszText = _T("To");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(8, &lvc);

	lvc.pszText = _T("Criteria");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(9, &lvc);

	strList.RemoveAll();
	lvc.pszText = _T("Min Value");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(10, &lvc);

	lvc.pszText = _T("Max Value");
	lvc.cx = 65;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(11, &lvc);

}
void CDlgWingspanAdjacancyConstraints::OnCancel()
{
	CDialog::OnCancel();
}
void CDlgWingspanAdjacancyConstraints::GetAllTaxiway()
{
	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(m_nProjID, vAirportIds);
	std::vector<int>::iterator iterAirportID = vAirportIds.begin();
	for (; iterAirportID != vAirportIds.end(); ++iterAirportID)
	{
		std::vector<int> vTaxiwayID;
		ALTAirport airport;
		airport.ReadAirport(*iterAirportID);
		ALTAirport::GetTaxiwaysIDs(*iterAirportID, vTaxiwayID);

		std::vector<int>::iterator iterTaxiwayID = vTaxiwayID.begin();
		for (; iterTaxiwayID != vTaxiwayID.end(); ++iterTaxiwayID)
		{
			Taxiway *pTaxiway = new Taxiway ;
			pTaxiway->ReadObject(*iterTaxiwayID);
			m_vTaxiway.push_back(pTaxiway);

			//CString strTmpTaxiway;
			//strTmpTaxiway.Format("%s", taxiway.GetObjNameString());
		}
	}
}

void CDlgWingspanAdjacancyConstraints::FillListControl()
{
	for (int i = 0; i < m_vData.GetCount(); i++)
	{
		CWingspanAdjacencyConstraints *pConstraint = m_vData.GetConstraint(i);
		AddConstraintItem(pConstraint);
	}
}

void CDlgWingspanAdjacancyConstraints::OnCmdNewItem()
{
	CDlgTaxiwaySelect dlg(m_nProjID);
	if(dlg.DoModal() == IDOK)
	{
		int nTaxiwayID = dlg.GetSelTaxiwayID();
		std::vector<Taxiway *>::iterator iter = m_vTaxiway.begin();
		for (; iter != m_vTaxiway.end(); ++iter)
		{
			if(*iter && (*iter)->getID() == nTaxiwayID)
			{
				Taxiway *pTaxiway = *iter;
				IntersectionNodeList vIntersectNode = pTaxiway->GetIntersectNodes();

				//create new item
				CWingspanAdjacencyConstraints *pConstraint = new CWingspanAdjacencyConstraints;
				pConstraint->m_firstaxiway.m_pTaxiway = *pTaxiway;
				if(vIntersectNode.size() > 0)
				{
					pConstraint->m_firstaxiway.m_startNode = vIntersectNode[0];
					pConstraint->m_firstaxiway.m_endNode = vIntersectNode.at(vIntersectNode.size() - 1);
					pConstraint->m_firstaxiway.m_pAirportDB = m_pAirportDB;
				}
//				pConstraint->m_firstaxiway.m_dMinValue = 

				//second taxiway

				pConstraint->m_secondTaxiway.m_pTaxiway = *pTaxiway;
				if(vIntersectNode.size() > 0)
				{
					pConstraint->m_secondTaxiway.m_startNode = vIntersectNode[0];
					pConstraint->m_secondTaxiway.m_endNode = vIntersectNode.at(vIntersectNode.size() - 1);
					pConstraint->m_secondTaxiway.m_pAirportDB = m_pAirportDB;
				}

				m_vData.AddItem(pConstraint);
				AddConstraintItem(pConstraint);

				GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
				break;				
			}
		}
	}
}
int CDlgWingspanAdjacancyConstraints::GetSelectedListItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}
void CDlgWingspanAdjacancyConstraints::OnCmdDeleteItem()
{
	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;
	CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nSelItem);
	if(pConstraint)
	{
		m_vData.DelItem(pConstraint);
	}
	m_wndListCtrl.DeleteItemEx(nSelItem);

	UpdateToolBarState();
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();
}
void CDlgWingspanAdjacancyConstraints::OnCmdEditItem()
{
	MessageBox(_T("Double click the item to modify the content."), _T("Tips"), MB_OK);

}
void CDlgWingspanAdjacancyConstraints::OnSave()
{
	try
	{
		CADODatabase::BeginTransaction();	
		m_vData.SaveData(m_nProjID);
		CADODatabase::CommitTransaction();	
	}
	catch (CADOException &e)
	{
		CADODatabase::RollBackTransation();
		e.ErrorMessage();
		MessageBox(_T("Cann't save the Object."));
		return;
	}
}

void CDlgWingspanAdjacancyConstraints::AddConstraintItem( CWingspanAdjacencyConstraints* pConItem )
{
	int iNewIndex = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), NULL);
	SetListItemContent(iNewIndex, *pConItem);
	m_wndListCtrl.SetItemData(iNewIndex,(DWORD_PTR)pConItem);
}

void CDlgWingspanAdjacancyConstraints::SetListItemContent( int nIndex, CWingspanAdjacencyConstraints& conItem )
{
	// If taxiway
	m_wndListCtrl.SetItemText(nIndex, 0, conItem.m_firstaxiway.getTaxiwayName());

	//from node
	m_wndListCtrl.SetItemText(nIndex, 1, conItem.m_firstaxiway.getStartNodeName());

	//end node
	m_wndListCtrl.SetItemText(nIndex, 2, conItem.m_firstaxiway.getEndNodeName());


	// Constraint
	m_wndListCtrl.SetItemText(nIndex, 3, _T("Span(m)"));

	// Min Value
	CString strValue;
	strValue.Format(_T("%.2f"), conItem.m_firstaxiway.m_dMinValue);
	m_wndListCtrl.SetItemText(nIndex, 4, strValue);

	// Max Value
	strValue.Format(_T("%.2f"), conItem.m_firstaxiway.m_dMaxValue);
	m_wndListCtrl.SetItemText(nIndex, 5, strValue);

	// Then Taxiway
	m_wndListCtrl.SetItemText(nIndex, 6, conItem.m_secondTaxiway.getTaxiwayName() );

	//from node
	m_wndListCtrl.SetItemText(nIndex, 7, conItem.m_secondTaxiway.getStartNodeName());

	//end node
	m_wndListCtrl.SetItemText(nIndex, 8, conItem.m_secondTaxiway.getEndNodeName());

	// Constraint
	m_wndListCtrl.SetItemText(nIndex, 9, _T("Span(m)"));

	// Min Value
	strValue.Format(_T("%.2f"), conItem.m_secondTaxiway.m_dMinValue);
	m_wndListCtrl.SetItemText(nIndex, 10, strValue);

	// Max Value
	strValue.Format(_T("%.2f"),conItem.m_secondTaxiway.m_dMaxValue);
	m_wndListCtrl.SetItemText(nIndex, 11, strValue);

}

void CDlgWingspanAdjacancyConstraints::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	if (nItem < 0)
		return;

	CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nItem);
	if(pConstraint == NULL)
		return;

	CString strValue = m_wndListCtrl.GetItemText(nItem, plvItem->iSubItem);
	switch(plvItem->iSubItem)
	{
	case 0://taxiway
		break;
	case 1://from 
		break;
	case 2://from 
		break;
	case 3:
		break;

	case 4:
		{
			double dMinValue = ::atof(strValue);
			pConstraint->m_firstaxiway.m_dMinValue = dMinValue;
		}
		break;

	case 5:
		{
			double dMinValue = ::atof(strValue);
			pConstraint->m_firstaxiway.m_dMaxValue = dMinValue;
		}
		break;

	case 6:
		break;

	case 7://from 
		break;
	case 8://from 
		break;

	case 9:
		break;

	case 10:
		{
			double dMinValue = ::atof(strValue);
			pConstraint->m_secondTaxiway.m_dMinValue = dMinValue;
		}
		break;

	case 11:
		{
			double dMinValue = ::atof(strValue);
			pConstraint->m_secondTaxiway.m_dMaxValue = dMinValue;
		}
		break;

	default:
		break;
	}

	//SetListItemContent(nItem, pConstraint);
	GetDlgItem(IDC_BUTTON_SAVE)->EnableWindow();

	*pResult = 0;
}

LRESULT CDlgWingspanAdjacancyConstraints::OnCollumnIndex( WPARAM wParam, LPARAM lParam )
{
	int nCollumn = lParam;

	if (nCollumn == 0) // If taxiway
	{

		CDlgTaxiwaySelect objDlg(m_nProjID);
		if(objDlg.DoModal() != IDOK )
			return 0;


		int nSelItem = GetSelectedListItem();
		if (nSelItem < 0)
			return 0;
		CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nSelItem);
		if(pConstraint == NULL)
			return 0;

		int nTaxiwayID = objDlg.GetSelTaxiwayID();
		std::vector<Taxiway *>::iterator iter = m_vTaxiway.begin();
		for (; iter != m_vTaxiway.end(); ++iter)
		{
			if(*iter && (*iter)->getID() == nTaxiwayID)
			{
				Taxiway *pTaxiway = *iter;
				IntersectionNodeList vIntersectNode = pTaxiway->GetIntersectNodes();

				//create new item
				pConstraint->m_firstaxiway.m_pTaxiway = *pTaxiway;
				if(vIntersectNode.size() > 0)
				{
					pConstraint->m_firstaxiway.m_startNode = vIntersectNode[0];
					pConstraint->m_firstaxiway.m_endNode = vIntersectNode.at(vIntersectNode.size() - 1);
				}

				SetListItemContent(nSelItem, *pConstraint);
			}
		}

	}
	else if (nCollumn == 6) // Then Taxiway
	{

		CDlgTaxiwaySelect objDlg(m_nProjID);
		if(objDlg.DoModal() != IDOK )
			return 0;


		int nSelItem = GetSelectedListItem();
		if (nSelItem < 0)
			return 0;
		CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nSelItem);
		if(pConstraint == NULL)
			return 0;

		int nTaxiwayID = objDlg.GetSelTaxiwayID();
		std::vector<Taxiway *>::iterator iter = m_vTaxiway.begin();
		for (; iter != m_vTaxiway.end(); ++iter)
		{
			if(*iter && (*iter)->getID() == nTaxiwayID)
			{
				Taxiway *pTaxiway = *iter;
				IntersectionNodeList vIntersectNode = pTaxiway->GetIntersectNodes();

				//create new item
				pConstraint->m_secondTaxiway.m_pTaxiway = *pTaxiway;
				if(vIntersectNode.size() > 0)
				{
					pConstraint->m_secondTaxiway.m_startNode = vIntersectNode[0];
					pConstraint->m_secondTaxiway.m_endNode = vIntersectNode.at(vIntersectNode.size() - 1);
				}

				SetListItemContent(nSelItem, *pConstraint);
			}
		}
	}
	return 1;

}

LRESULT CDlgWingspanAdjacancyConstraints::OnMsgComboChange( WPARAM wParam, LPARAM lParam )
{
	int nComboxSel = (int)wParam;
	if (nComboxSel == LB_ERR)
		return 0;

	LV_DISPINFO* dispinfo = (LV_DISPINFO*)lParam;
	int nCurSel = dispinfo->item.iItem;
	int nCurSubSel = dispinfo->item.iSubItem;	

	CWingspanAdjacencyConstraints *pConstraint = (CWingspanAdjacencyConstraints *)m_wndListCtrl.GetItemData(nCurSel);
	if(pConstraint == NULL)
		return 0;

	switch (nCurSubSel)
	{
	case 1: // From
		{
			//if(pConstraint->m_firstaxiway.m_pTaxiway)
			{
				IntersectionNodeList vNodeList = pConstraint->m_firstaxiway.m_pTaxiway.GetIntersectNodes();
				if(static_cast<int>(vNodeList.size()) > nComboxSel)
					pConstraint->m_firstaxiway.m_startNode = vNodeList.at(nComboxSel);
			}
		}
		break;
	case 2: // To
		{
		//	if(pConstraint->m_firstaxiway.m_pTaxiway)
			{
				IntersectionNodeList vNodeList = pConstraint->m_firstaxiway.m_pTaxiway.GetIntersectNodes();
				if(static_cast<int>(vNodeList.size()) > nComboxSel)
					pConstraint->m_firstaxiway.m_endNode = vNodeList.at(nComboxSel);
			}
		}
		break;

	case 7: // From
		{
		//	if(pConstraint->m_secondTaxiway.m_pTaxiway)
			{
				IntersectionNodeList vNodeList = pConstraint->m_secondTaxiway.m_pTaxiway.GetIntersectNodes();
				if(static_cast<int>(vNodeList.size()) > nComboxSel)
					pConstraint->m_secondTaxiway.m_startNode = vNodeList.at(nComboxSel);
			}
		}
		break;
	case 8: // To
		{
	//		if(pConstraint->m_secondTaxiway.m_pTaxiway)
			{
				IntersectionNodeList vNodeList = pConstraint->m_secondTaxiway.m_pTaxiway.GetIntersectNodes();
				if(static_cast<int>(vNodeList.size()) > nComboxSel)
					pConstraint->m_secondTaxiway.m_endNode = vNodeList.at(nComboxSel);
			}
		}
		break;
	default:
		break;
	}	

	return 0;
}