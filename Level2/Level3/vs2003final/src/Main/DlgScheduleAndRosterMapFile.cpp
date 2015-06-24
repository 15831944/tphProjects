// DlgScheduleAndRosterMapFile.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgScheduleAndRosterMapFile.h"
#include ".\dlgscheduleandrostermapfile.h"
#include "../Inputs/AODBImportManager.h"
#include "DlgMapCodeInput.h"
// CDlgScheduleAndRosterMapFile dialog

IMPLEMENT_DYNAMIC(CDlgScheduleAndRosterMapFile, CXTResizeDialog)
CDlgScheduleAndRosterMapFile::CDlgScheduleAndRosterMapFile(AODBImportManager* pAodbImportManager,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgScheduleAndRosterMapFile::IDD, pParent)
	,m_pAodbImportManager(pAodbImportManager)
	,m_mapFile(m_pAodbImportManager->GetMapFile())
{
}

CDlgScheduleAndRosterMapFile::~CDlgScheduleAndRosterMapFile()
{
}

void CDlgScheduleAndRosterMapFile::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgScheduleAndRosterMapFile, CXTResizeDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBAR_ADD,OnToolBarAdd)
	ON_COMMAND(ID_TOOLBAR_DEL,OnToolBarDel)
	ON_COMMAND(ID_TOOLBAR_EDIT,OnToolBarEdit)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnLvnEndlabeleditListData)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_DATA,OnSelListCtrlListData)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DATA, OnColumnclickListMap)
	ON_BN_CLICKED(IDC_BUTTON_SELECT,OnBtnOpen)
	ON_MESSAGE(WM_XLISTCTRL_DBCLICKED,OnDBClickListCtrl)
END_MESSAGE_MAP()

LRESULT CDlgScheduleAndRosterMapFile::OnDBClickListCtrl(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

void CDlgScheduleAndRosterMapFile::OnColumnclickListMap(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_wndSortHeaderList.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_wndSortHeaderList.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_wndSortHeaderList.SaveSortList();
	}	
	*pResult = 0;
}

// CDlgScheduleAndRosterMapFile message handlers
BOOL CDlgScheduleAndRosterMapFile::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	OnInitToolbar();
	FillListHeader();
	FillListCtrl();

	CString strFilePath = m_mapFile.getFileName();
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(strFilePath);

	UpdateToolbar();
	SetResize(IDC_STATIC_MAP,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_EDIT_PATH,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_BUTTON_SELECT,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgScheduleAndRosterMapFile::FillListHeader()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndSortHeaderList.SubclassWindow( m_wndListCtrl.GetHeaderCtrl()->GetSafeHwnd() );

	const char* colLabelEnvironment[] = { "Code", "PROCESS NAME"};
	const int nFormatEnvironment[] = { LVCFMT_LEFT, LVCFMT_LEFT};
	const int DefaultColumnWidthEnvironment[] = { 100,300};
	const EDataType typeEnvironment[] = { dtSTRING, dtSTRING};

	int nColNum = 2;
	for( int i=0; i<nColNum; i++ )
	{
		m_wndListCtrl.InsertColumn( i, colLabelEnvironment[i], nFormatEnvironment[i], DefaultColumnWidthEnvironment[i] );
		m_wndSortHeaderList.SetDataType( i, typeEnvironment[i] );
	}
}

int CDlgScheduleAndRosterMapFile::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (!m_wndToolbar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP
		| CBRS_TOOLTIPS | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolbar.LoadToolBar(IDR_FLIGHTMANAGER_TOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}

void CDlgScheduleAndRosterMapFile::OnInitToolbar()
{
	CWnd* pWnd = GetDlgItem(IDC_STATIC_TOOBAR);
	if (pWnd)
	{
		CRect rc;
		pWnd->GetWindowRect( &rc );
		ScreenToClient(&rc);
		pWnd->ShowWindow(SW_HIDE);
		m_wndToolbar.MoveWindow(&rc,true);
		m_wndToolbar.ShowWindow( SW_SHOW );

		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
	}
}

void CDlgScheduleAndRosterMapFile::OnOK()
{
	GetDlgItem(IDC_EDIT_PATH)->GetWindowText(m_strMapPath);
	m_pAodbImportManager->SetMapFile(m_mapFile);
	m_pAodbImportManager->GetRosterFile().Convert(m_pAodbImportManager->GetConverParameter());
	m_pAodbImportManager->GetScheduleFile().Convert(m_pAodbImportManager->GetConverParameter());

	CXTResizeDialog::OnOK();
}

const CString& CDlgScheduleAndRosterMapFile::GetMapFilePath()const
{
	return m_strMapPath;
}

void CDlgScheduleAndRosterMapFile::OnToolBarAdd()
{
	CDlgMapCodeInput dlg;
	if (dlg.DoModal() == IDOK)
	{
		AODB::FieldMapItem* item = new AODB::FieldMapItem();
		item->m_strCode = dlg.GetCode();
		item->m_strObject = dlg.GetProcess();

		int nIdx = m_wndListCtrl.GetItemCount();
		{
		 	//code
		 	m_wndListCtrl.InsertItem(nIdx,item->m_strCode);
		
		 	//process
		 	m_wndListCtrl.SetItemText(nIdx,1,item->m_strObject);
		
		 	m_wndListCtrl.SetItemData(nIdx,(DWORD_PTR)item);
		}
		m_mapFile.AddData(item);
		m_wndListCtrl.EnsureVisible(nIdx,TRUE);
		m_wndListCtrl.SetCurSel(nIdx);
	}
}

void CDlgScheduleAndRosterMapFile::OnToolBarEdit()
{
	int nCurSel = m_wndListCtrl.GetCurSel();
	if (nCurSel != LB_ERR)
	{
		m_wndListCtrl.EditSubLabel(nCurSel,0);
	}
}

void CDlgScheduleAndRosterMapFile::OnToolBarDel()
{
	int nCurSel = m_wndListCtrl.GetCurSel();
	if (nCurSel == LB_ERR)
		return;
	
	AODB::FieldMapItem* item = (AODB::FieldMapItem*)m_wndListCtrl.GetItemData(nCurSel);
	if (item)
	{
		m_mapFile.DeleteData(item);
		m_wndListCtrl.DeleteItem(nCurSel);
	}
}

void CDlgScheduleAndRosterMapFile::OnLvnEndlabeleditListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_wndListCtrl.OnEndlabeledit(pNMHDR, pResult);

	if (pDispInfo == NULL)
		return;
	
	AODB::FieldMapItem* item = (AODB::FieldMapItem*)m_wndListCtrl.GetItemData(pDispInfo->item.iItem);
	if (item == NULL)
		return;
	
	CString szText = m_wndListCtrl.GetItemText(pDispInfo->item.iItem,pDispInfo->item.iSubItem);
	switch (pDispInfo->item.iSubItem)
	{
	case 0:
		item->m_strCode = szText;
		break;
	case 1:
		item->m_strObject = szText;
		break;
	default:
		break;
	}
	*pResult = 0;
}

void CDlgScheduleAndRosterMapFile::OnSelListCtrlListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	NM_LISTVIEW* pnmv = (NM_LISTVIEW*)pNMHDR;
	if(!pnmv)
		return;	 
	*pResult = 0;

	if ((pnmv->uChanged & LVIF_STATE) != LVIF_STATE)
		return;

	if(!(pnmv->uOldState & LVIS_SELECTED) && !(pnmv->uNewState & LVIS_SELECTED))
		return;

	if(pnmv->uOldState & LVIS_SELECTED)
	{
		if(!(pnmv->uNewState & LVIS_SELECTED))
		{
			
		}
	}
	else if(pnmv->uNewState & LVIS_SELECTED)
	{
		//a new item has been selected
		m_wndListCtrl.SetCurSel(pnmv->iItem);
	}
	else
		ASSERT(0);

	UpdateToolbar();
}

void CDlgScheduleAndRosterMapFile::UpdateToolbar()
{
	if (m_mapFile.GetCount() == 0)
	{
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
		m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
	}
	else
	{
		int nCurSel = m_wndListCtrl.GetCurSel();
		if (nCurSel != LB_ERR)
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,TRUE);
		}
		else
		{
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_ADD,TRUE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_EDIT,FALSE);
			m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DEL,FALSE);
		}
	}
}

void CDlgScheduleAndRosterMapFile::FillListCtrl()
{
	m_wndListCtrl.DeleteAllItems();
	for (int i = 0; i < m_mapFile.GetCount(); i++)
	{
		AODB::FieldMapItem* item = m_mapFile.GetItem(i);
		{
			//code
			m_wndListCtrl.InsertItem(i,item->m_strCode);

			//process
			m_wndListCtrl.SetItemText(i,1,item->m_strObject);

			m_wndListCtrl.SetItemData(i,(DWORD_PTR)item);
		}
		m_wndListCtrl.EnsureVisible(i,TRUE);
	}
	UpdateToolbar();
}

void CDlgScheduleAndRosterMapFile::OnBtnOpen()
{
	CFileDialog dlgFile(TRUE,".csv",NULL,OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"AMS Files (*.csv)|*.csv||");
	CString sFileName;
	if (dlgFile.DoModal() != IDOK)
		return;

	sFileName = dlgFile.GetPathName();
	
	m_mapFile.Clear();
	m_mapFile.setFileName(sFileName);
	m_mapFile.Read();

	FillListCtrl();
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(sFileName);
}