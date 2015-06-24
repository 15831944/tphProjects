// OpenComparativeReportsGroup.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "OpenComparativeReportsGroup.h"

#include <vector>
#include "../compare/ComparativeReportManager.h"
#include "../compare/ModelsManager.h"

#include "../common/ProjectManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenComparativeReportsGroup dialog


COpenComparativeReportsGroup::COpenComparativeReportsGroup(CWnd* pParent /*=NULL*/)
	: CDialog(COpenComparativeReportsGroup::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenComparativeReportsGroup)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void COpenComparativeReportsGroup::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenComparativeReportsGroup)
	DDX_Control(pDX, IDC_LISTREPORTGROUPS, m_ListReportsGroup);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenComparativeReportsGroup, CDialog)
	//{{AFX_MSG_MAP(COpenComparativeReportsGroup)
	ON_WM_SIZE()
	ON_NOTIFY(NM_DBLCLK, IDC_LISTREPORTGROUPS, OnDblclkProjlist)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LISTREPORTGROUPS, OnColumnclickListreportgroups)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenComparativeReportsGroup message handlers

void COpenComparativeReportsGroup::InitListCtrl()
{
	//m_ListReportsGroup.DeleteAllItems();
	//DWORD dystyle=m_ListReportsGroup.GetExtendedStyle();
	//dystyle|=LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	//m_ListReportsGroup.SetExtendedStyle(dystyle);
	DWORD dwStyle = m_ListReportsGroup.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_ListReportsGroup.SetExtendedStyle( dwStyle );

	//LV_COLUMNEX lvc;
	//lvc.mask=LVCF_WIDTH|LVCF_TEXT;
	char columnLable[][128]={"Name", "Description", "User", "Machine", "Date Created", "Last Modified"};

	CRect rclist;
	CStringList strList;
	m_ListReportsGroup.GetWindowRect(rclist);
	//int defaultColumnwidth[]={rclist.Width()/2,rclist.Width()/2};
	int defaultColumnwidth[]={rclist.Width() / 8, rclist.Width() / 8, 
		rclist.Width() / 8, rclist.Width() / 8, rclist.Width() / 4, rclist.Width()/4};
	int _iFormat[]={LVCFMT_LEFT|LVCFMT_NOEDIT,LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT,
	LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT, LVCFMT_LEFT|LVCFMT_NOEDIT};

	EDataType dt[] = {dtSTRING, dtSTRING, dtSTRING, dtSTRING, dtDATETIME, dtDATETIME};

	m_headerCtl.SubclassWindow( m_ListReportsGroup.GetHeaderCtrl()->m_hWnd );
	for( int i=0;i<6;i++)
	{
		/*lvc.csList=&strList;
		lvc.pszText=columnLable[i];
		lvc.cx=defaultColumnwidth[i];
		lvc.fmt=_iFormat[i];*/
		//m_ListReportsGroup.InsertColumn(i,&lvc);
		m_ListReportsGroup.InsertColumn(i, columnLable[i], _iFormat[i], defaultColumnwidth[i]);

		m_headerCtl.SetDataType(i, dt[i]);
	}
}

void COpenComparativeReportsGroup::OnOK() 
{
	// TODO: Add extra validation here
	int nCurSel = GetSelIndex();

	if (nCurSel == -1)
	{
		AfxMessageBox(_T("Please select an item."));
		return;
	}

	m_strName = m_ListReportsGroup.GetItemText(nCurSel, 0);
	m_strDesc = m_ListReportsGroup.GetItemText(nCurSel, 1);

	CDialog::OnOK();
}

void COpenComparativeReportsGroup::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}


BOOL COpenComparativeReportsGroup::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_ListReportsGroup.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

    //ListView_SetExtendedListViewStyle(m_ListReportsGroup.GetSafeHwnd(),LVS_EX_FULLROWSELECT);
	InitListCtrl();

	FillProjectList();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void COpenComparativeReportsGroup::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	if(::IsWindow(m_ListReportsGroup.GetSafeHwnd())) {
		m_ListReportsGroup.MoveWindow(0, 0, cx, cy-46);
		CRect rclist;
		m_ListReportsGroup.GetClientRect(rclist);
		int defaultColumnwidth[]={rclist.Width() / 8, rclist.Width() / 8, 
			rclist.Width() / 8, rclist.Width() / 8, rclist.Width() / 4, rclist.Width()/4};
		for (int i = 0; i < 6; i++)
			m_ListReportsGroup.SetColumnWidth(i, defaultColumnwidth[i]);

		GetDlgItem(IDCANCEL)->MoveWindow(cx-85, cy-36, 75, 26);
		GetDlgItem(IDOK)->MoveWindow(cx-170, cy-36, 75, 26);
	}
	// TODO: Add your message handler code here
	
}

void COpenComparativeReportsGroup::FillProjectList()
{
	CComparativeProjectDataSet dsProj;
	std::vector<CComparativeProject *>	vProjs;
	dsProj.loadDataSet(PROJMANAGER->GetAppPath());

	if (dsProj.GetProjects(vProjs))
	{
		//	Fill listbox
		for (int i = 0; i < static_cast<int>(vProjs.size()); i++)
		{
			int nIndex = m_ListReportsGroup.GetItemCount();
			m_ListReportsGroup.InsertItem(nIndex, vProjs[i]->GetName());
			m_ListReportsGroup.SetItemText(nIndex, 1, vProjs[i]->GetDescription());
			m_ListReportsGroup.SetItemText(nIndex, 2, vProjs[i]->GetUser());
			m_ListReportsGroup.SetItemText(nIndex, 3, vProjs[i]->GetMachine());
			CTime t = vProjs[i]->GetCreatedTime();
			m_ListReportsGroup.SetItemText(nIndex, 4, t.Format("%m/%d/%Y %H:%M:%S"));
			t = vProjs[i]->GetLastModifiedTime();
			m_ListReportsGroup.SetItemText(nIndex, 5, t.Format("%m/%d/%Y %H:%M:%S"));
			m_ListReportsGroup.SetItemData(nIndex, nIndex);
		}
	}
}

void COpenComparativeReportsGroup::OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	OnOK();
}

void COpenComparativeReportsGroup::OnColumnclickListreportgroups(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_headerCtl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_headerCtl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_headerCtl.SaveSortList();
		//m_iPreSortByColumnIdx = nTestIndex;
	}

	*pResult = 0;
}

int COpenComparativeReportsGroup::GetSelIndex()
{
	int i= m_ListReportsGroup.GetItemCount()-1;
	for( ; i>-1; i-- )
	{
		if( ( m_ListReportsGroup.GetItemState(i,LVIS_SELECTED) & LVIS_SELECTED ) == LVIS_SELECTED )
		{
			break;
		}
	}
	if( i == -1 )
		return -1;

	return i;
}
