#include "stdafx.h"
#include "Resource.h"
#include "DlgGateAssignConstraints.h"
#include "ProcesserDialog.h"
#include "TermPlanDoc.h"
#include "GateAssignDlg.h"
#include "DlgAdjacencyGateConstraints.h"
#include "../Inputs/GateAssignmentConstraints.h"
#include "./SelectALTObjectDialog.h"
#include "../InputAirside/InputAirside.h"
#include "DlgACTypeStandConstraint.h"
#include "DlgAdjacencyConstraintSpec.h"
#include "DlgAssignmentConstraintSpec.h"
#include "../AirsideGUI/DlgStandFamily.h"

static const UINT ID_NEW_GATECONSTRAINT = 10;
static const UINT ID_DEL_GATECONSTRAINT = 11;
static const UINT ID_EDIT_GATECONSTRAINT = 12;

static const char constraintTypes[][32] = {_T("Height(m)"), _T("Len(m)"), _T("Span(m)"), _T("MZFW(lbs)"),
											_T("OEW(lbs)"), _T("MTOW(lbs)"), _T("MLW(lbs)"), _T("Capacity")};

CDlgGateAssignConstraints::CDlgGateAssignConstraints(InputTerminal* pInTerm,int nAirportID,int nProjectID, GateAssignmentConstraintList* pConstraints, CWnd* pParent /*=NULL*/)
 : CToolTipDialog(CDlgGateAssignConstraints::IDD, pParent)
 , m_pInputTerminal(pInTerm)
 , m_pConstraints(pConstraints)
{
	m_nProjectID = nProjectID;
	m_nAirportID = nAirportID;
}

CDlgGateAssignConstraints::~CDlgGateAssignConstraints()
{
}

void CDlgGateAssignConstraints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GATEASSIGN_CONSTRAINTS, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(CDlgGateAssignConstraints, CDialog)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_GATEASSIGN_CONSTRAINTS, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_GATEASSIGN_CONSTRAINTS, OnLvnEndlabeleditListContents)
	ON_COMMAND(ID_NEW_GATECONSTRAINT, OnCmdNewItem)
	ON_COMMAND(ID_DEL_GATECONSTRAINT, OnCmdDeleteItem)
	ON_COMMAND(ID_EDIT_GATECONSTRAINT, OnCmdEditItem)
	ON_BN_CLICKED(IDC_BUTTON_ADJACENCYCON, OnAdjacencyCon)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_BUTTON_ACTYPECONSTRAINT, OnBnClickedButtonActypeconstraint)
END_MESSAGE_MAP()

BOOL CDlgGateAssignConstraints::OnInitDialog()
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


int CDlgGateAssignConstraints::OnCreate(LPCREATESTRUCT lpCreateStruct)
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
	toolbar.SetCmdID(0, ID_NEW_GATECONSTRAINT);
	toolbar.SetCmdID(1, ID_DEL_GATECONSTRAINT);
	toolbar.SetCmdID(2, ID_EDIT_GATECONSTRAINT);

	return 0;
}

void CDlgGateAssignConstraints::OnSize(UINT nType, int cx, int cy)
{
	RefreshLayout(cx, cy);
}

void CDlgGateAssignConstraints::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = 400;
	lpMMI->ptMinTrackSize.y = 200;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgGateAssignConstraints::UpdateToolBarState()
{
	if (!::IsWindow(m_wndToolBar.m_hWnd) || !::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	BOOL bEnable = TRUE;
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_NEW_GATECONSTRAINT, bEnable);

	// delete & edit button
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos == NULL)
		bEnable = FALSE;
	else
		bEnable = TRUE;

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_GATECONSTRAINT, bEnable);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_GATECONSTRAINT, bEnable);
}

// refresh the layout of child controls
void CDlgGateAssignConstraints::RefreshLayout(int cx, int cy)
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
	CWnd* pWndAdjacency = GetDlgItem(IDC_BUTTON_ADJACENCYCON);
	CWnd* pWndActype = GetDlgItem(IDC_BUTTON_ACTYPECONSTRAINT) ;
	if (pWndSave == NULL || pWndOK == NULL || pWndCancel == NULL || pWndAdjacency == NULL || pWndActype == NULL)
		return;

	CRect rcAdjacency;
	pWndAdjacency->GetClientRect(&rcAdjacency);
	pWndAdjacency->MoveWindow(10, cy - rcAdjacency.Height() - 10, rcAdjacency.Width(), rcAdjacency.Height());

	CRect rcActype ;
	pWndActype->GetClientRect(&rcActype) ;
	pWndActype->MoveWindow(10 + rcAdjacency.Width() + 10 ,cy - rcActype.Height() -10 , rcActype.Width(),rcActype.Height()) ;

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


void CDlgGateAssignConstraints::InitListControl()
{
	if (!::IsWindow(m_wndListCtrl.m_hWnd))
		return;

	// set the list header contents;
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Gate Group");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Constraint");
	for (int i = 0; i < 8; i++)
		strList.AddTail(constraintTypes[i]);
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(1, &lvc);

	strList.RemoveAll();
	lvc.pszText = _T("Min Value");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Max Value");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NUMBER;
	m_wndListCtrl.InsertColumn(3, &lvc);

	int nConstraintCount = m_pConstraints->GetCount();
	for (int i = 0; i < nConstraintCount; i++)
	{
		GateAssignmentConstraint& conItem = m_pConstraints->GetItem(i);
		AddConstraintItem(conItem);
	}
}

void CDlgGateAssignConstraints::AddConstraintItem(const GateAssignmentConstraint& conItem)
{
	int iNewIndex = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), NULL);
	SetListItemContent(iNewIndex, conItem);
}

void CDlgGateAssignConstraints::SetListItemContent(int nIndex, const GateAssignmentConstraint& conItem)
{
	// Gate group
	m_wndListCtrl.SetItemText(nIndex, 0, conItem.GetGate().GetIDString());

	// Constraint
	m_wndListCtrl.SetItemText(nIndex, 1, constraintTypes[conItem.GetConType()]);

	// Min Value
	CString strValue;
	strValue.Format(_T("%.2f"), conItem.GetMinValue());
	m_wndListCtrl.SetItemText(nIndex, 2, strValue);

	// Max Value
	strValue.Format(_T("%.2f"), conItem.GetMaxValue());
	m_wndListCtrl.SetItemText(nIndex, 3, strValue);
}

CString CDlgGateAssignConstraints::GetProjPath()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	return pDoc->m_ProjInfo.path;
}

void CDlgGateAssignConstraints::OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult)
{
	UpdateToolBarState();
}

void CDlgGateAssignConstraints::OnCmdNewItem()
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

	//CString strNewProc = procID.GetIDString();
	// check if the process id has exist in the list ctrl


	/*CSelectALTObjectDialog objDlg(0,m_nAirportID);
	objDlg.SetType(ALT_STAND);*/
	CDlgStandFamily objDlg(m_nProjectID);
	if(objDlg.DoModal()!=IDOK) return ;

	std::vector<CString> vConstraints;
	CString objIDstr;
	objIDstr = objDlg.GetSelStandFamilyName();
	if (objIDstr.IsEmpty())
		return;
//	if( !objDlg.GetObjectIDString(objIDstr) )return;

	for (int i=0; i < m_wndListCtrl.GetItemCount(); i++ )
	{
		CString strItemText = m_wndListCtrl.GetItemText(i, 0);
		CString strConstrainText = m_wndListCtrl.GetItemText(i,1);
 		if (objIDstr.CompareNoCase(strItemText) == 0)		// has exist in the list 
 		{
			vConstraints.push_back(strConstrainText);
 		}
	}

	int nCount = (int)vConstraints.size();
	if (nCount == 8)
	{
		return;
	}

	int nIndex = 0;

	for (int i = 0; i < 8; i++)
	{
		if (!FindSameConstraints(vConstraints,constraintTypes[i]))
		{
			nIndex = i;
			break;
		}
	}
	GateAssignmentConstraint conItem;
	conItem.SetConType((GateAssignConType)nIndex);
	conItem.SetGate(ALTObjectID((const char *)objIDstr));
	m_pConstraints->AddItem(conItem);
	AddConstraintItem(conItem);

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CDlgGateAssignConstraints::OnCmdDeleteItem()
{
	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;

	m_pConstraints->DeleteItem(nSelItem);
	m_wndListCtrl.DeleteItemEx(nSelItem);

	UpdateToolBarState();
	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CDlgGateAssignConstraints::OnCmdEditItem()
{
	int nSelItem = GetSelectedListItem();
	if (nSelItem < 0)
		return;


	/*CSelectALTObjectDialog objDlg(0,m_nAirportID);
	objDlg.SetType(ALT_STAND);*/
	CDlgStandFamily objDlg(m_nProjectID);
	if(objDlg.DoModal()!=IDOK) return ;

	std::vector<CString> vConstraints;
	CString objIDstr;
	objIDstr = objDlg.GetSelStandFamilyName();
	if (objIDstr.IsEmpty())
		return;
	
//	if( !objDlg.GetObjectIDString(objIDstr) )return;

	for (int i=0; i < m_wndListCtrl.GetItemCount(); i++ )
	{
		CString strItemText = m_wndListCtrl.GetItemText(i, 0);
		CString strConstrainText = m_wndListCtrl.GetItemText(i,1);
		if (objIDstr.CompareNoCase(strItemText) == 0)		// has exist in the list 
		{
			vConstraints.push_back(strConstrainText);
		}
	}

	int nCount = (int)vConstraints.size();
	int nIndex = 0;

	for (int i = 0; i < 8; i++)
	{
		if (!FindSameConstraints(vConstraints,constraintTypes[i]))
		{
			nIndex = i;
			break;
		}
	}
	
	GateAssignmentConstraint& conItem = m_pConstraints->GetItem(nSelItem);
	conItem.SetGate(ALTObjectID((const char *)objIDstr));
	conItem.SetConType((GateAssignConType)nIndex);
	SetListItemContent(nSelItem, conItem);

	GetDlgItem(IDC_SAVE)->EnableWindow();
}

void CDlgGateAssignConstraints::OnOK()
{
	OnSave();
	//((CGateAssignDlg *)GetParent())->GatePriorityAssign();
	CToolTipDialog::OnOK();
}

void CDlgGateAssignConstraints::OnCancel()
{
	m_pConstraints->loadDataSet(GetProjPath());
	CToolTipDialog::OnCancel();
}

void CDlgGateAssignConstraints::OnAdjacencyCon()
{
 	DlgAssignmentConstraintSpec dlg(&m_pConstraints->GetStandAdjConstraint(),this);
 	if (dlg.DoModal()== IDOK)
 	{
 		if (m_pConstraints->GetStandAdjConstraint().GetUseFlag() == 1)
 		{
			CDlgAdjacencyGateConstraints dlg(m_pInputTerminal,&m_pConstraints->GetStandAdjConstraint().GetAdjConstraints(),m_nProjectID,this);
			dlg.DoModal();
 		}
 		else
		{
			DlgAdjacencyConstraintSpec dlg(m_pInputTerminal,&m_pConstraints->GetStandAdjConstraint().GetAdjConstraintSpec(),m_nProjectID, this);
			dlg.DoModal();
 			
 		}		
 	}
}

void CDlgGateAssignConstraints::OnSave()
{
	m_pConstraints->saveDataSet(GetProjPath(), true);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
}

void CDlgGateAssignConstraints::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	if (nItem < 0)
		return;

	GateAssignmentConstraint& conItem = m_pConstraints->GetItem(nItem);
	CString strValue = m_wndListCtrl.GetItemText(nItem, plvItem->iSubItem);
	CString strTemp = m_wndListCtrl.GetItemText(nItem,0);
	std::vector<CString> vConstraints;
	switch(plvItem->iSubItem)
	{
	case 1:
		{
			for (int i=0; i < m_wndListCtrl.GetItemCount(); i++ )
			{
				CString strItemText = m_wndListCtrl.GetItemText(i, 0);
				CString strConstrainText = m_wndListCtrl.GetItemText(i,1);
				if (strTemp.CompareNoCase(strItemText) == 0 && i != plvItem->iItem)		// has exist in the list 
				{
					vConstraints.push_back(strConstrainText);
				}
			}
		
			if (!FindSameConstraints(vConstraints,strValue.GetBuffer(0)))
			{
				for (int j = 0; j < 8; j++)
				{
					if (strValue.Compare(constraintTypes[j]) == 0)
					{
						conItem.SetConType((GateAssignConType)j);
					}
				}
			}
		}
		break;

	case 2:
		{
			float fMinValue = (float)::atof(strValue);
			conItem.SetMinValue(fMinValue);
		}
		break;

	case 3:
		{
			float fMaxValue = (float)::atof(strValue);
			conItem.SetMaxValue(fMaxValue);
		}
	    break;

	default:
	    break;
	}

	SetListItemContent(nItem, conItem);
	GetDlgItem(IDC_SAVE)->EnableWindow();
	*pResult = 0;
}

int CDlgGateAssignConstraints::GetSelectedListItem()
{
	POSITION pos = m_wndListCtrl.GetFirstSelectedItemPosition();
	if (pos)
		return m_wndListCtrl.GetNextSelectedItem(pos);

	return -1;
}

BOOL CDlgGateAssignConstraints::FindSameConstraints(std::vector<CString>&vConstraints,const char* strConstraints)
{
	int nCount = (int)vConstraints.size();

	for (int i = 0; i < nCount; i++)
	{
		if (strcmp(vConstraints[i],strConstraints) == 0)
		{
			return TRUE;
		}
	}
	return FALSE;
}
void CDlgGateAssignConstraints::OnBnClickedButtonActypeconstraint()
{
	// TODO: Add your control notification handler code here
	DlgACTypeStandConstraint dlg(m_pInputTerminal, m_nAirportID, m_nProjectID,m_pConstraints->GetACTypeStandConstraints(), this);
	dlg.DoModal();
	m_pConstraints->GetACTypeStandConstraints()->RemoveAll();
	m_pConstraints->GetACTypeStandConstraints()->ReadData();
}
