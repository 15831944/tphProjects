// DlgAdjacencyConstraintSpec.cpp : implementation file
//
#include "stdafx.h"
#include "Resource.h"
#include "DlgAdjacencyConstraintSpec.h"
#include "TermPlanDoc.h"
#include "SelectStandDialog.h"
#include "../InputAirside/InputAirside.h"
#include "Common/UnitPiece.h"
#include "../AirsideGUI/DlgStandFamily.h"

static const char consSpeTypes[][32] = {_T("OneStandOnly"), _T("ConcurrentUse"), _T("MidOrSides")};

IMPLEMENT_DYNAMIC(DlgAdjacencyConstraintSpec, CXTResizeDialog)

DlgAdjacencyConstraintSpec::DlgAdjacencyConstraintSpec(InputTerminal* pInTerm, AdjacencyConstraintSpecList* pAdjGateConstraints,int nProjectID, CWnd* pParent)
: CXTResizeDialog(IDD_DIALOG_ADJACENCYCONSTRAINTASPEC, pParent)
, m_pInputTerminal(pInTerm)
, m_pAdjConstraintSpec(pAdjGateConstraints)
{
	m_nProjectID = nProjectID;
}

DlgAdjacencyConstraintSpec::~DlgAdjacencyConstraintSpec()
{
	
}

void DlgAdjacencyConstraintSpec::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ADJACENCYCONSTRAINTSPECDATA, m_wndListCtrl);
}

BEGIN_MESSAGE_MAP(DlgAdjacencyConstraintSpec, CXTResizeDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ADJACENCYCONSTRAINTSPECDATA, OnLvnItemchangedList)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ADJACENCYCONSTRAINTSPECDATA, OnLvnEndlabeleditListContents)
	ON_COMMAND(ID_ADD_ADJACENCYCONSTRAINTSPEC, OnNewItem)
	ON_COMMAND(ID_DEL_ADJACENCYCONSTRAINTSPEC, OnDeleteItem)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnOK)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnCancel)
	ON_MESSAGE(WM_COLLUM_INDEX, OnDoubleClick)
END_MESSAGE_MAP()

BOOL DlgAdjacencyConstraintSpec::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	InitToolBar();
	
	SetResize(m_wndToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndListCtrl.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_GROUP, SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_CANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_OK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	CUnitPiece::InitializeUnitPiece(m_nProjectID,UM_ID_25,this);
	CRect rectItem;
	GetDlgItem(IDC_BUTTON_OK)->GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int yPos = rectItem.top;
	m_wndListCtrl.GetWindowRect(rectItem);
	ScreenToClient(rectItem);
	int xPos = rectItem.left;
	CUnitPiece::MoveUnitButton(xPos,yPos);
	SetResize(XIAOHUABUTTON_ID, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	CUnitPiece::SetUnitInUse(ARCUnit_Length_InUse);

	InitListControl();
	return TRUE;
}

void DlgAdjacencyConstraintSpec::InitToolBar()
{
	CRect rcToolbar;
	m_wndListCtrl.GetWindowRect(&rcToolbar);
	ScreenToClient(&rcToolbar);
	rcToolbar.left += 2;
	rcToolbar.top -= 26;
	rcToolbar.bottom = rcToolbar.top + 22;
	m_wndToolBar.MoveWindow(rcToolbar);

	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADD_ADJACENCYCONSTRAINTSPEC);
	m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ADJACENCYCONSTRAINTSPEC,FALSE);
}

void DlgAdjacencyConstraintSpec::InitListControl()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle(dwStyle);

	m_wndListCtrl.DeleteAllItems();
	int nColumnCount = m_wndListCtrl.GetHeaderCtrl()->GetItemCount();
	for (int i = 0;i < nColumnCount;i++)
		m_wndListCtrl.DeleteColumn(0);

	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.csList = &strList;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Index");
	lvc.cx = 40;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Stand1");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(1, &lvc);

	lvc.pszText = _T("Stand2");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(2, &lvc);

	lvc.pszText = _T("Stand3");
	lvc.cx = 150;
	lvc.fmt = LVCFMT_NOEDIT;
	m_wndListCtrl.InsertColumn(3, &lvc);

	lvc.pszText = _T("Condition Of Use");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_DROPDOWN;
	m_wndListCtrl.InsertColumn(4, &lvc);

	CString strTitle;
	strTitle.Format(_T("Available Width(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strTitle.GetBuffer();
	strTitle.ReleaseBuffer();
	lvc.cx = 110;
	m_wndListCtrl.InsertColumn(5, &lvc);


	strTitle.Format(_T("WingTip Clearance(%s)"),CARCLengthUnit::GetLengthUnitString(CUnitPiece::GetCurSelLengthUnit()));
	lvc.fmt = LVCFMT_NUMBER;
	lvc.pszText = strTitle.GetBuffer();
	strTitle.ReleaseBuffer();
	lvc.cx = 130;
	m_wndListCtrl.InsertColumn(6, &lvc);
	InitListItem();	
}

void DlgAdjacencyConstraintSpec::InitListItem()
{
	int nConstraintCount = m_pAdjConstraintSpec->GetCount();
	for (int i = 0; i < nConstraintCount; i++)
	{
		AddConstraintItem(m_pAdjConstraintSpec->GetItem(i));
	}
}

int DlgAdjacencyConstraintSpec::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE |CBRS_ALIGN_TOP) ||
		!m_wndToolBar.LoadToolBar(IDR_TOOLBAR_ADJACENCYCONSTRAINTSPEC))
	{
		return -1;
	}

	CToolBarCtrl& toolbar = m_wndToolBar.GetToolBarCtrl();
	toolbar.SetCmdID(0, ID_ADD_ADJACENCYCONSTRAINTSPEC);
	toolbar.SetCmdID(1, ID_DEL_ADJACENCYCONSTRAINTSPEC);

	return 0;
}

void DlgAdjacencyConstraintSpec::OnNewItem()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return ;


	//CSelectStandDialog objDlg(0,vAirport.at(0));
	//if(objDlg.DoModal() != IDOK ) return;
	CDlgStandFamily objDlg(m_nProjectID);
	if(objDlg.DoModal()!=IDOK) return ;

	CString pnewIDstr;
//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return;
	pnewIDstr = objDlg.GetSelStandFamilyName();
	if (pnewIDstr.IsEmpty())
		return;

	
	AdjacencyConstraintSpecDate pAdjacencyConSpec;
	pAdjacencyConSpec.SetFirstGate(ALTObjectID((const char*)pnewIDstr));

	m_pAdjConstraintSpec->AddItem(pAdjacencyConSpec);	
	AddConstraintItem(pAdjacencyConSpec);
	int CurSel = m_wndListCtrl.GetItemCount();
	m_wndListCtrl.SetCurSel(CurSel-1);
}	

void DlgAdjacencyConstraintSpec::OnDeleteItem()
{
	int CurSel = m_wndListCtrl.GetCurSel();
	if(CurSel < 0)
		return;
	m_pAdjConstraintSpec->DeleteItem(CurSel);
	m_wndListCtrl.DeleteAllItems();

	int nConstraintCount = m_pAdjConstraintSpec->GetCount();
	for (int i = 0; i < nConstraintCount; i++)
	{
		AddConstraintItem(m_pAdjConstraintSpec->GetItem(i));
	}
}

void DlgAdjacencyConstraintSpec::AddConstraintItem( AdjacencyConstraintSpecDate& AdjConSpeItem )
{
	int iNewIndex = m_wndListCtrl.InsertItem(m_wndListCtrl.GetItemCount(), NULL);
	SetListItemContent(iNewIndex, AdjConSpeItem);
}

void DlgAdjacencyConstraintSpec::SetListItemContent( int nIndex,AdjacencyConstraintSpecDate& AdjConSpeItem )
{
	// Index
	CString strValue;
	strValue.Format(_T("%d"),nIndex+1);
	m_wndListCtrl.SetItemText(nIndex, 0, strValue);

	// Stand1
	strValue.Format(_T("%s"), AdjConSpeItem.m_FirstGate.GetIDString());
	m_wndListCtrl.SetItemText(nIndex, 1, strValue);

	// Stand2
	strValue.Format(_T("%s"), AdjConSpeItem.m_SecondGate.GetIDString());	
	m_wndListCtrl.SetItemText(nIndex, 2, strValue);

	// Stand3
	if (AdjConSpeItem.m_ThirdGate.IsBlank())
	{
		strValue.Format(_T(" "));
	} 
	else
	{
		strValue.Format(_T("%s"), AdjConSpeItem.m_ThirdGate.GetIDString());
	}
	m_wndListCtrl.SetItemText(nIndex, 3, strValue);

	// Condition Of Use
	strValue.Format(_T("%s"), consSpeTypes[AdjConSpeItem.GetConType()]);
	m_wndListCtrl.SetItemText(nIndex, 4, strValue);

	// available Width(m)
	double dWidth =  CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),AdjConSpeItem.m_Width);
	strValue.Format(_T("%.2f"), dWidth);
	m_wndListCtrl.SetItemText(nIndex, 5, strValue);

	// WingTip Clearance(m)
	double dClearance = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,CUnitPiece::GetCurSelLengthUnit(),AdjConSpeItem.m_Clearance);
	strValue.Format(_T("%.2f"), dClearance);
	m_wndListCtrl.SetItemText(nIndex, 6, strValue);
}

void DlgAdjacencyConstraintSpec::OnLvnItemchangedList( NMHDR *pNMHDR, LRESULT *pResult )
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;

	*pResult = 0;

	if((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
	{
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADD_ADJACENCYCONSTRAINTSPEC,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ADJACENCYCONSTRAINTSPEC,FALSE);
		return;
	}

	if(pnmv->uOldState & LVIS_SELECTED) 
	{
		if(!(pnmv->uNewState & LVIS_SELECTED)) 
		{
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADD_ADJACENCYCONSTRAINTSPEC,TRUE);
			m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ADJACENCYCONSTRAINTSPEC,FALSE);
		}	
	}
	else if(pnmv->uNewState & LVIS_SELECTED) {
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_ADD_ADJACENCYCONSTRAINTSPEC,TRUE);
		m_wndToolBar.GetToolBarCtrl().EnableButton(ID_DEL_ADJACENCYCONSTRAINTSPEC,TRUE);
	}
	else
		ASSERT(0);	
}

void DlgAdjacencyConstraintSpec::OnLvnEndlabeleditListContents( NMHDR *pNMHDR, LRESULT *pResult )
{
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	if (nItem < 0)
		return;

	AdjacencyConstraintSpecDate& conItem = m_pAdjConstraintSpec->GetItem(nItem);
	CString strValue = m_wndListCtrl.GetItemText(nItem, plvItem->iSubItem);
	switch(plvItem->iSubItem)
	{
	case 4:
		{
			if (conItem.m_SecondGate.IsBlank())
					return;
			if((!conItem.m_FirstGate.IsBlank()) && (!conItem.m_SecondGate.IsBlank()) && (!conItem.m_ThirdGate.IsBlank()))
			{
				for (int j = 0; j < 3; j++)
				{
					if (strValue.Compare(consSpeTypes[j]) == 0)
					{
						conItem.SetConType((ConditionOfUseType)j);
					}
				}
			}
			if ((!conItem.m_FirstGate.IsBlank()) && (!conItem.m_SecondGate.IsBlank()) && (conItem.m_ThirdGate.IsBlank()))
			{
				for (int j = 0; j < 2; j++)
				{
					if (strValue.Compare(consSpeTypes[j]) == 0)
					{
						conItem.SetConType((ConditionOfUseType)j);
					}
				}						
			} 
		}
		break;
	case 5:
		{
			double dWidth = atof(strValue);
			double nWidth = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dWidth);
			conItem.m_Width = nWidth;
		}
		break;

	case 6:
		{
			double dClearance = atof(strValue);
			double nClearance = CARCLengthUnit::ConvertLength(CUnitPiece::GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,dClearance);
			conItem.m_Clearance = nClearance;
		}
		break;

	default:
		break;
	}

	SetListItemContent(nItem, conItem);
	*pResult = 0;
}

LRESULT DlgAdjacencyConstraintSpec::OnDoubleClick( WPARAM wParam, LPARAM lParam )
{
	int nCollumn = lParam;
	int nComboxSel = (int)wParam;
	int nItem = m_wndListCtrl.GetCurSel();
	if (nItem < 0)
		return 0;

	AdjacencyConstraintSpecDate& conItem = m_pAdjConstraintSpec->GetItem(nItem);
	CString strValue = m_wndListCtrl.GetItemText(nItem, nCollumn);

	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	std::vector<int> vAirport;
	InputAirside::GetAirportList(pDoc->GetProjectID(),vAirport);
	if(vAirport.size()<1) return 0 ;

	if (nCollumn == 1) //  Stand1
	{
	/*	CSelectStandDialog objDlg(0,vAirport.at(0));
		if(objDlg.DoModal() != IDOK ) return 0;*/
		CDlgStandFamily objDlg(m_nProjectID);
		if(objDlg.DoModal()!=IDOK) return 0;


		CString pnewIDstr;
	//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return 0;
		pnewIDstr = objDlg.GetSelStandFamilyName();
		if (pnewIDstr.IsEmpty())
			return 0;

	
		conItem.SetFirstGate(ALTObjectID((const char*)pnewIDstr));
		SetListItemContent(nItem, conItem);
	}
	else if (nCollumn == 2) // Stand2
	{
		if (conItem.m_FirstGate.IsBlank())
			return 0;	
	/*	CSelectStandDialog objDlg(0,vAirport.at(0));
		if(objDlg.DoModal() != IDOK ) return 0;*/
		CDlgStandFamily objDlg(m_nProjectID);
		if(objDlg.DoModal()!=IDOK) return 0;

		CString pnewIDstr;
	//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return 0;
		pnewIDstr = objDlg.GetSelStandFamilyName();
		if (pnewIDstr.IsEmpty())
			return 0;

		conItem.SetSecondGate(ALTObjectID((const char*)pnewIDstr));
		SetListItemContent(nItem, conItem);
	}
	else if (nCollumn == 3) //Stand3
	{	
		if (conItem.m_FirstGate.IsBlank() || conItem.m_SecondGate.IsBlank())
			return 0;
	
		//CSelectStandDialog objDlg(0,vAirport.at(0));
		//if(objDlg.DoModal() != IDOK ) return 0;
		CDlgStandFamily objDlg(m_nProjectID);
		if(objDlg.DoModal()!=IDOK) return 0;

		CString pnewIDstr;
	//	if( !objDlg.GetObjectIDString(pnewIDstr) )  return 0;
		pnewIDstr = objDlg.GetSelStandFamilyName();
		if (pnewIDstr.IsEmpty())
			return 0;


		conItem.SetThirdGate(ALTObjectID((const char*)pnewIDstr));
		SetListItemContent(nItem, conItem);
	}
	else if (nCollumn == 4)
	{
		if (conItem.m_SecondGate.IsBlank())
				return 0;

		if(!conItem.m_ThirdGate.IsBlank())
		{
			LVCOLDROPLIST* pList =  m_wndListCtrl.GetColumnStyle(4);
			pList->List.RemoveAll();
			for (int i = 0; i < 3; i++)
				pList->List.AddTail(consSpeTypes[i]);	
		}
		if (conItem.m_ThirdGate.IsBlank())
		{
			LVCOLDROPLIST* pList =  m_wndListCtrl.GetColumnStyle(4);
			pList->List.RemoveAll();
			for (int i = 0; i < 2; i++)
				pList->List.AddTail(consSpeTypes[i]);
		} 
	}
	return 0;
}

CString DlgAdjacencyConstraintSpec::GetProjPath()
{
	CMDIChildWnd* pMDIActive = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
	ASSERT( pMDIActive != NULL );

	CTermPlanDoc* pDoc = (CTermPlanDoc*)pMDIActive->GetActiveDocument();
	ASSERT( pDoc!= NULL );

	return pDoc->m_ProjInfo.path;
}

void DlgAdjacencyConstraintSpec::OnOK()
{
	int Count = m_pAdjConstraintSpec->GetCount();
	for (int i = 0; i < Count; i++)
	{
		AdjacencyConstraintSpecDate AdjConstraintSpec = m_pAdjConstraintSpec->GetItem(i);
		if (AdjConstraintSpec.GetSecondGate().IsBlank())
		{
			MessageBox(_T("You must define two stand at least"), "Error", MB_OK);
			return;
		}
	}
	m_pAdjConstraintSpec->saveDataSet(GetProjPath(),true);
	CXTResizeDialog::OnOK();
}

void DlgAdjacencyConstraintSpec::OnCancel()
{
	m_pAdjConstraintSpec->loadDataSet(GetProjPath());
	CXTResizeDialog::OnCancel();
}

bool DlgAdjacencyConstraintSpec::ConvertUnitFromDBtoGUI( void )
{
	return (true);
}

bool DlgAdjacencyConstraintSpec::RefreshGUI( void )
{
	InitListControl();
	return (true);
}

bool DlgAdjacencyConstraintSpec::ConvertUnitFromGUItoDB( void )
{
	return (true);
}
	