#include "stdafx.h"
#include "Resource.h"
#include "DlgAdjacencyGateConstraints.h"
#include "ProcesserDialog.h"
#include "TermPlanDoc.h"
#include "SelectStandDialog.h"
#include "../InputAirside/InputAirside.h"
#include "../AirsideGUI/DlgStandFamily.h"

static const UINT ID_NEW_ADJACENCYGATECONSTRAINT = 10;
static const UINT ID_DEL_ADJACENCYGATECONSTRAINT = 11;
static const UINT ID_EDIT_ADJACENCYGATECONSTRAINT = 12;

static const char constraintTypes[][32] = {_T("Height(m)"), _T("Len(m)"), _T("Span(m)"), _T("MZFW(lbs)"),
											_T("OEW(lbs)"), _T("MTOW(lbs)"), _T("MLW(lbs)"), _T("Capacity")};

CDlgAdjacencyGateConstraints::CDlgAdjacencyGateConstraints(InputTerminal* pInTerm, AdjacencyGateConstraintList* pAdjGateConstraints,int nProjID,CWnd* pParent /*=NULL*/)
 : CToolTipDialog(IDD, pParent)
 , m_pInputTerminal(pInTerm)
 , m_pAdjGateConstraints(pAdjGateConstraints)
{
	m_nProjID = nProjID;
}

CDlgAdjacencyGateConstraints::~CDlgAdjacencyGateConstraints()
{
}

void CDlgAdjacencyGateConstraints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADJACENCYGATE_CONSTRAINTS, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CDlgAdjacencyGateConstraints, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ADJACENCYGATE_CONSTRAINTS, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ADJACENCYGATE_CONSTRAINTS, OnLvnEndlabeleditListContents)
	ON_COMMAND(ID_NEW_ADJACENCYGATECONSTRAINT, OnCmdNewItem)
	ON_COMMAND(ID_DEL_ADJACENCYGATECONSTRAINT, OnCmdDeleteItem)
	ON_COMMAND(ID_EDIT_ADJACENCYGATECONSTRAINT, OnCmdEditItem)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_MESSAGE(WM_COLLUM_INDEX, OnCollumnIndex)
END_MESSAGE_MAP()


BOOL CDlgAdjacencyGateConstraints::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

	// set list control window style
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	// set the layout of child controls
	RefreshLayout();

	// init the list control
	InitListControl();

	// set the toolbar button state
	UpdateToolBarState();

	return TRUE;
}


int CDlgAdjacencyGateConstraints::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CToolTipDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_NEW_ADJACENCYGATECONSTRAINT);
	toolbar.SetCmdID(1, ID_DEL_ADJACENCYGATECONSTRAINT);
	toolbar.SetCmdID(2, ID_EDIT_ADJACENCYGATECONSTRAINT);

	return 0;
}

void CDlgAdjacencyGateConstraints::UpdateToolBarState()
{
	if (!::IsWindow(m_wndToolBar.m_hWnd) || !::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	BOOL bEnable = TRUE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_ADJACENCYGATECONSTRAINT, bEnable);

	// delete & edit button
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ADJACENCYGATECONSTRAINT, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_ADJACENCYGATECONSTRAINT, bEnable);
}

void CDlgAdjacencyGateConstraints::OnSize(UINT nType, int cx, int cy)
{
	RefreshLayout(cx, cy);
}

void CDlgAdjacencyGateConstraints::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 200;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgAdjacencyGateConstraints::AddConstraintItem(AdjacencyGateConstraint& conItem)
{
	int iNewIndex = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), NULL);
	SetListItemContent(iNewIndex, conItem);
}

void CDlgAdjacencyGateConstraints::SetListItemContent(int nIndex, AdjacencyGateConstraint& conItem)
{
	// If Stand
	GateAssignmentConstraint* pFirsCon = conItem.GetFirstConstraint();
	m_wndListCtrl.SetItemText(nIndex, 0, pFirsCon->GetGate().GetIDString());

	// Constraint
	m_wndListCtrl.SetItemText(nIndex, 1, constraintTypes[pFirsCon->GetConType()]);

	// Min Value
	CString strValue;
	strValue.Format(_T("%.2f"), pFirsCon->GetMinValue());
	m_wndListCtrl.SetItemText(nIndex, 2, strValue);

	// Max Value
	strValue.Format(_T("%.2f"), pFirsCon->GetMaxValue());
	m_wndListCtrl.SetItemText(nIndex, 3, strValue);

	// Then Stand
	GateAssignmentConstraint* pSecondCon = conItem.GetSecondConstraint();
	m_wndListCtrl.SetItemText(nIndex, 4, pSecondCon->GetGate().GetIDString());

	// Constraint
	m_wndListCtrl.SetItemText(nIndex, 5, constraintTypes[pSecondCon->GetConType()]);

	// Min Value
	strValue.Format(_T("%.2f"), pSecondCon->GetMinValue());
	m_wndListCtrl.SetItemText(nIndex, 6, strValue);

	// Max Value
	strValue.Format(_T("%.2f"), pSecondCon->GetMaxValue());
	m_wndListCtrl.SetItemText(nIndex, 7, strValue);
}

CString CDlgAdjacencyGateConstraints::GetProjPath()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	return pDoc->m_ProjInfo.path;
}

void CDlgAdjacencyGateConstraints::OnOK()
{
	OnSave();
	CToolTipDialog::OnOK();
}

void CDlgAdjacencyGateConstraints::OnCancel()
{
	CToolTipDialog::OnCancel();
}

void CDlgAdjacencyGateConstraints::OnSave()
{
	m_pAdjGateConstraints->saveDataSet(GetProjPath(), true);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgAdjacencyGateConstraints::OnCmdNewItem()
{
	/*CProcesserDialog procDlg(m_pInputTerminal);
	procDlg.SetType(GateProc);
	procDlg.SetType2(StandProcessor);
	procDlg.SetOnlyShowACStandGate(true);
	if (procDlg.DoModal() != IDOK)
		return;

	ProcessorID procID;
	if (!procDlg.GetProcessorID(procID))
		return;*/

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return ;
	

	//CSelectStandDialog objDlg(0,vAirport.at(0));
	//if(objDlg.DoModal() != IDOK ) return;
	CDlgStandFamily objDlg(m_nProjID);
	if(objDlg.DoModal()!=IDOK) return ;

	CString pnewIDstr;
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;

	GateAssignmentConstraint* pCon = new GateAssignmentConstraint;
	pCon->SetGate( ALTObjectID((const char*)pnewIDstr));
	GateAssignmentConstraint* pCon2 = new GateAssignmentConstraint;
	pCon2->SetGate(ALTObjectID((const char*)pnewIDstr));

	
	AdjacencyGateConstraint conItem(pCon, pCon2);
	m_pAdjGateConstraints->AddItem(conItem);
	AddConstraintItem(conItem);

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CDlgAdjacencyGateConstraints::OnCmdDeleteItem()
{
	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;

	m_wndListCtrl.DeleteItemEx(nSelItem);
	m_pAdjGateConstraints->DeleteItem(nSelItem);

	UpdateToolBarState();
	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CDlgAdjacencyGateConstraints::OnCmdEditItem()
{
	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return ;

	//CSelectStandDialog objDlg(0,vAirport.at(0));
	//if(objDlg.DoModal() != IDOK ) return;

	CDlgStandFamily objDlg(m_nProjID);
	if(objDlg.DoModal()!=IDOK) return ;

	CString pnewIDstr;
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;

	
	AdjacencyGateConstraint& conItem = m_pAdjGateConstraints->GetItem(nSelItem);
	conItem.GetFirstConstraint()->SetGate(ALTObjectID((const char*)pnewIDstr) );
	SetListItemContent(nSelItem, conItem);

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

// refresh the layout of child controls
void CDlgAdjacencyGateConstraints::RefreshLayout(int cx, int cy)
{
	if (cx == -1 && cy == -1)
	{
		CRect rectClient;
		GetClientRect(&rectClient);
		cx = rectClient.Width();
		cy = rectClient.Height();
	}

	CWnd* pWndSave = GetDlgItem(IDC_SAVE);
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

	CWnd* pGroupBox = GetDlgItem(IDC_GROUPBOX);
	if (pGroupBox == NULL)
		return;

	pGroupBox->MoveWindow(10, 5, cx - 20, cy - rcOK.Height() - 27);

	m_wndListCtrl.MoveWindow(10, 35, cx - 20, cy - rcOK.Height() - 57);

	if (!::IsWindow(m_wndToolBar.m_hWnd))
		return;
	m_wndToolBar.MoveWindow(11, 11, cx - 25, 22);

	Invalidate();
}


void CDlgAdjacencyGateConstraints::InitListControl()
{
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	// set the list header contents;
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("If Stand");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Criteria");
	for (int i = 0; i < 8; i++)
		strList.AddTail(constraintTypes[i]);
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1, &lvc);

	strList.RemoveAll();
	lvc.pszText = _T("Min Value");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Max Value");
	lvc.cx = 65;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.pszText = _T("Then Stand");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(4, &lvc);

	lvc.pszText = _T("Criteria");
	for (int i = 0; i < 8; i++)
		strList.AddTail(constraintTypes[i]);
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(5, &lvc);

	strList.RemoveAll();
	lvc.pszText = _T("Min Value");
	lvc.cx = 60;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(6, &lvc);

	lvc.pszText = _T("Max Value");
	lvc.cx = 65;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(7, &lvc);

	int nConstraintCount = m_pAdjGateConstraints->GetCount();
	for (int i = 0; i < nConstraintCount; i++)
	{
		AddConstraintItem(m_pAdjGateConstraints->GetItem(i));
	}
}

void CDlgAdjacencyGateConstraints::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBarState();
}

void CDlgAdjacencyGateConstraints::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	if (nItem < 0)
		return;

	AdjacencyGateConstraint& conItem = m_pAdjGateConstraints->GetItem(nItem);
	CString strValue = m_wndListCtrl.GetItemText(nItem, plvItem->iSubItem);
	switch(plvItem->iSubItem)
	{
	case 1:
		for (int i  = 0; i < 8; i++)
		{
			if (strValue.Compare(constraintTypes[i]) == 0)
			{
				conItem.GetFirstConstraint()->SetConType(GateAssignConType(i));
				break;
			}
		}
		break;

	case 2:
		{
			float fMinValue = (float)::atof(strValue);
			conItem.GetFirstConstraint()->SetMinValue(fMinValue);
		}
		break;

	case 3:
		{
			float fMaxValue = (float)::atof(strValue);
			conItem.GetFirstConstraint()->SetMaxValue(fMaxValue);
		}
		break;

	case 5:
		for (int i  = 0; i < 8; i++)
		{
			if (strValue.Compare(constraintTypes[i]) == 0)
			{
				conItem.GetSecondConstraint()->SetConType(GateAssignConType(i));
				break;
			}
		}
		break;

	case 6:
		{
			float fMinValue = (float)::atof(strValue);
			conItem.GetSecondConstraint()->SetMinValue(fMinValue);
		}
		break;

	case 7:
		{
			float fMaxValue = (float)::atof(strValue);
			conItem.GetSecondConstraint()->SetMaxValue(fMaxValue);
		}
		break;

	default:
		break;
	}

	SetListItemContent(nItem, conItem);
	GetDlgItem(IDC_SAVE)->EnableWindow();

	*pResult = 0;
}


int CDlgAdjacencyGateConstraints::GetSelectedListItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}

LRESULT CDlgAdjacencyGateConstraints::OnCollumnIndex(WPARAM wParam,  LPARAM lParam)
{
	int nCollumn = lParam;

	if (nCollumn == 0) // If Stand
	{
		CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
		ASSERT( pMDIActive != NULL );

		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT( pDoc!= NULL );

		std::vector<int> vAirport;
		InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
		if(vAirport.size()<1) return 0 ;

		//CSelectStandDialog objDlg(0,vAirport.at(0));
		//if(objDlg.DoModal() != IDOK ) return 0;
		CDlgStandFamily objDlg(m_nProjID);
		if(objDlg.DoModal()!=IDOK) return 0;

		CString pnewIDstr;
	//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return 0;
		pnewIDstr = objDlg.GetSelStandFamilyName();
		if (pnewIDstr.IsEmpty())
			return 0;

		int nItem = GetSelectedListItem();
		if (nItem < 0)
			return 0;

		AdjacencyGateConstraint& conItem = m_pAdjGateConstraints->GetItem(nItem);
		conItem.GetFirstConstraint()->SetGate( ALTObjectID((const char*)pnewIDstr));
		SetListItemContent(nItem, conItem);
	}
	else if (nCollumn == 4) // Then Stand
	{
		CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
		ASSERT( pMDIActive != NULL );

		CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
		ASSERT( pDoc!= NULL );

		std::vector<int> vAirport;
		InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
		if(vAirport.size()<1) return 0;

		/*CSelectStandDialog objDlg(0,vAirport.at(0));
		if(objDlg.DoModal() != IDOK ) return 0;*/
		CDlgStandFamily objDlg(m_nProjID);
		if(objDlg.DoModal()!=IDOK) return 0;

		CString pnewIDstr;
		//if( !objDlg.GetObjectIDString(pnewIDstr) )  return 0;
		pnewIDstr = objDlg.GetSelStandFamilyName();
		if (pnewIDstr.IsEmpty())
			return 0;
		
		CString strNewProc = pnewIDstr;
		int nItem = GetSelectedListItem();
		if (nItem < 0)
			return 0;

		AdjacencyGateConstraint& conItem = m_pAdjGateConstraints->GetItem(nItem);
		conItem.GetSecondConstraint()->SetGate( ALTObjectID((const char*)pnewIDstr) );
		SetListItemContent(nItem, conItem);
	}

	return 0;
}