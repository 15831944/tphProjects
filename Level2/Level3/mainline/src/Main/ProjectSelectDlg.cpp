// ProjectSelectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProjectSelectDlg.h"

#include "..\common\ProjectManager.h"
#pragma warning( disable : 4786 )
#include <vector>
#include ".\projectselectdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectSelect dialog


CDlgProjectSelect::CDlgProjectSelect(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProjectSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProjectSelect)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_sSelProjName = _T("");
}


void CDlgProjectSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProjectSelect)
	DDX_Control(pDX, IDC_PROJLIST, m_cProjList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProjectSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgProjectSelect)
	ON_WM_SIZE()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_PROJLIST, OnPLColumnClick)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_PROJLIST, OnPLGetDispInfo)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_PROJLIST, OnClickProjList)
	ON_NOTIFY(NM_DBLCLK, IDC_PROJLIST, OnDblclkProjlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProjectSelect message handlers

BOOL CDlgProjectSelect::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	CRect wndrect, listrect;
	GetClientRect(&wndrect);
	CString colname;
	int cx = wndrect.Width();
	int cy = wndrect.Height();
	m_cProjList.MoveWindow(0, 0, cx, cy-46);
	GetDlgItem(IDCANCEL)->MoveWindow(cx-85, cy-36, 75, 26);
	GetDlgItem(IDOK)->MoveWindow(cx-170, cy-36, 75, 26);
	m_cProjList.GetClientRect(&listrect);
	colname.LoadString(IDS_PSD_PROJNAME);
	m_cProjList.InsertColumn(0, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_USERNAME);
	m_cProjList.InsertColumn(1, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_MACHINENAME);
	m_cProjList.InsertColumn(2, colname, LVCFMT_LEFT, listrect.Width()/6);
	colname.LoadString(IDS_PSD_DATECREATED);
	m_cProjList.InsertColumn(3, colname, LVCFMT_LEFT, listrect.Width()/4);
	colname.LoadString(IDS_PSD_DATEMODIFIED);
	m_cProjList.InsertColumn(4, colname, LVCFMT_LEFT, listrect.Width()/4);

	m_cProjList.ModifyStyle(LVS_TYPEMASK, LVS_REPORT);

	ListView_SetExtendedListViewStyle(m_cProjList.GetSafeHwnd(),LVS_EX_FULLROWSELECT);

	//now populate the list
	std::vector<CString> vList;
	PROJMANAGER->GetProjectList(vList);
	for(int i=0; i<static_cast<int>(vList.size()); i++) {
		CString projName = vList[i];
		PROJECTINFO* pi = new PROJECTINFO();
		PROJMANAGER->GetProjectInfo(projName, pi);
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT |  LVIF_PARAM;
		lvi.iItem = i;
		lvi.iSubItem = 0;
		lvi.iImage = 0;
		lvi.pszText = LPSTR_TEXTCALLBACK;
		lvi.lParam = (LPARAM) pi;
		if(pi->isDeleted!=1)
			m_cProjList.InsertItem(&lvi);
/*		m_cProjList.InsertItem(i, projName, 0);
		m_cProjList.SetItemText(i, 1, pi->user);
		m_cProjList.SetItemText(i, 2, pi->machine);
		m_cProjList.SetItemText(i, 3, pi->createtime.Format("%Y, %a %b %d %h %m %s"));
		m_cProjList.SetItemText(i, 4, pi->modifytime.Format("%Y, %a %b %d %h %m %s"));
*/	}
	
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProjectSelect::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	if(::IsWindow(m_cProjList.GetSafeHwnd())) {
		m_cProjList.MoveWindow(0, 0, cx, cy-46);
		GetDlgItem(IDCANCEL)->MoveWindow(cx-85, cy-36, 75, 26);
		GetDlgItem(IDOK)->MoveWindow(cx-170, cy-36, 75, 26);
	}
	// TODO: Add your message handler code here
}

void CDlgProjectSelect::OnPLColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_cProjList.SortItems(CompareFunc, pNMListView->iSubItem);	
	*pResult = 0;
}

int CALLBACK CDlgProjectSelect::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	//static int sortorder = 0;
	PROJECTINFO* pItem1 = (PROJECTINFO*) lParam1;
	PROJECTINFO* pItem2 = (PROJECTINFO*) lParam2;
	int nResult;
	switch(lParamSort) {
	case 0: // sort by proj name
		nResult = pItem1->name.CompareNoCase(pItem2->name);
		break;
	case 1: // sort by user
		nResult = pItem1->user.CompareNoCase(pItem2->user);
		break;
	case 2: // sort by machine
		nResult = pItem1->machine.CompareNoCase(pItem2->machine);
		break;
	case 3: // sort by create date
		nResult = (pItem1->createtime < pItem2->createtime) ? 1 : -1;
		if(pItem1->createtime == pItem2->createtime)
			nResult = 0;
		break;
	case 4: // sort by modify date
		nResult = (pItem1->modifytime < pItem2->modifytime) ? 1 : -1;
		if(pItem1->modifytime == pItem2->modifytime)
			nResult = 0;
		break;
	}
	//if(sortorder = (++sortorder % 2))
	//	nResult = -nResult;
	return nResult;
}

void CDlgProjectSelect::OnPLGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if(pDispInfo->item.mask & LVIF_TEXT) {
		PROJECTINFO* pInfo = (PROJECTINFO*) pDispInfo->item.lParam;
		switch(pDispInfo->item.iSubItem) {
		case 0: //proj name
			lstrcpy(pDispInfo->item.pszText, pInfo->name);
			break;
		case 1: //user
			lstrcpy(pDispInfo->item.pszText, pInfo->user);
			break;
		case 2: //machine
			lstrcpy(pDispInfo->item.pszText, pInfo->machine);
			break;
		case 3:
			lstrcpy(pDispInfo->item.pszText, pInfo->createtime.Format("%x %X"));
			break;
		case 4:
			lstrcpy(pDispInfo->item.pszText, pInfo->modifytime.Format("%x %X"));
			break;
		}
	}
	*pResult = 0;
}


void CDlgProjectSelect::FreeItemMemory()
{
	int nCount = m_cProjList.GetItemCount();
	if(nCount > 0)
		for(int i=0; i<nCount; i++)
			delete (PROJECTINFO*) m_cProjList.GetItemData(i);
}

void CDlgProjectSelect::OnDestroy() 
{
	FreeItemMemory();
	CDialog::OnDestroy();	
}

void CDlgProjectSelect::OnClickProjList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pNMHDR;
	if(lpnmitem->iItem != -1) {
		PROJECTINFO* pInfo = (PROJECTINFO*)	m_cProjList.GetItemData(lpnmitem->iItem);
		m_sSelProjName = pInfo->name;
	}
	
	*pResult = 0;
}

void CDlgProjectSelect::OnDblclkProjlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here	
	UINT i, uSelectedCount = m_cProjList.GetSelectedCount();
	int  nItem = -1;
	
	// Update all of the selected items.
	if (uSelectedCount > 0)
	{
		for (i=0;i < uSelectedCount;i++)
		{
			nItem = m_cProjList.GetNextItem(nItem, LVNI_SELECTED);
			ASSERT(nItem != -1);
			PROJECTINFO* pInfo = (PROJECTINFO*)	m_cProjList.GetItemData(nItem);
			m_sSelProjName = pInfo->name;
			*pResult = 0;
			OnOK();
		}
	}

//	if(lpnmitem->iItem != -1) 
//	{
//		PROJECTINFO* pInfo = (PROJECTINFO*)	m_cProjList.GetItemData(lpnmitem->iItem);
//		m_sSelProjName = pInfo->name;
//		*pResult = 0;
//		OnOK();
//	}
}

void CDlgProjectSelect::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_sSelProjName.IsEmpty())
	{
		MessageBox(_T("Please select one project"));
		return;
	}
	CDialog::OnOK();
}
