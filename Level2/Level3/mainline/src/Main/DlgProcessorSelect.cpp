// DlgProcessorSelect.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgProcessorSelect.h"

#include <deque>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorSelect dialog


CDlgProcessorSelect::CDlgProcessorSelect(CPROCESSOR2LIST* pProcList, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProcessorSelect::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProcessorSelect)
	//}}AFX_DATA_INIT
	m_pProcList = pProcList;
}


void CDlgProcessorSelect::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProcessorSelect)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProcessorSelect, CDialog)
	//{{AFX_MSG_MAP(CDlgProcessorSelect)
	ON_WM_DESTROY()
	ON_EN_UPDATE(IDC_EDIT_PROCNAME, OnUpdateEditProcName)
	ON_EN_CHANGE(IDC_EDIT_PROCNAME, OnChangeEditProcName)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCNAMES, OnSelChangedTreeProcNames)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProcessorSelect message handlers

BOOL CDlgProcessorSelect::OnInitDialog() 
{
	/*
	CDialog::OnInitDialog();
	
	CBitmap bmL;
	m_ilNodes.Create(16,16,ILC_COLOR16 | ILC_MASK,0,1);
	bmL.LoadBitmap(IDB_SMALLNODEIMAGES);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	bmL.Detach();
	bmL.LoadBitmap(IDB_NODESMALLIMG);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	bmL.Detach();
	bmL.LoadBitmap(IDB_FLOORSMALLIMG);
	m_ilNodes.Add(&bmL, RGB(255,0,255));
	m_cProcSelect.SetImageList(&m_ilNodes, TVSIL_NORMAL);

	LONG nStyle = GetWindowLong(m_cProcSelect.GetSafeHwnd(), GWL_STYLE);
	SetWindowLong(m_cProcSelect.GetSafeHwnd(), GWL_STYLE, nStyle | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS);

	/*
	//populate the tree view with the proc names
	for(int i=0; i<m_pProcList->size(); i++) {
		CProcessor2* pProc = m_pProcList->at(i);
		CString sName = pProc->Name();
		VERIFY(AddProcName(sName));
	}

	//disable the OK button
	CWnd* pOK = GetDlgItem(IDOK);
	pOK->EnableWindow(FALSE);
	*/
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgProcessorSelect::AddProcName(LPCTSTR lpszProcName)
{
	/*
	CString sName(lpszProcName);
	std::vector<CString> vNameSplit;
	//split the name into parts seperated by dashes
	int first = -1, next = -1;
	while(TRUE) {
		next = sName.Find('-', first+1);
		if(next != -1)
			vNameSplit.push_back(sName.Mid(first+1,next-first-1));
		else {
			vNameSplit.push_back(sName.Mid(first+1));
			break;
		}
		first = next;
	}
	
	//now search the tree (starting at root) to find the first part of name	
	UINT nMask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE;
	HTREEITEM hItem = NULL;
	HTREEITEM hLastItem = NULL;
	BOOL bFound = TRUE;
	for(int i=0; i<vNameSplit.size(); i++) {
		hLastItem = hItem;
		hItem = FindName(vNameSplit[i], hItem);
		if(hItem == NULL) {
			//add name and break
			for(int j=i; j<vNameSplit.size(); j++) {
				CString s = vNameSplit[j];
				hItem = m_cProcSelect.InsertItem(nMask, s, 0, 0, 0, 0, (LPARAM) NULL, hLastItem, TVI_LAST);
				hLastItem = hItem;
			}
			bFound = FALSE;
			break;
		}
	}
	return !bFound;
	*/
	return FALSE;
}

HTREEITEM CDlgProcessorSelect::FindName(LPCTSTR lpszProcName, HTREEITEM startItem)
{
	/*
	std::deque<HTREEITEM> vList;
	//put startItem (or root)into list..
	if(startItem == NULL) {
		startItem = m_cProcSelect.GetRootItem();
		vList.push_front(startItem);
		HTREEITEM sibItem = m_cProcSelect.GetNextSiblingItem(startItem);
		while(sibItem != NULL) {
			vList.push_front(sibItem);
			sibItem = m_cProcSelect.GetNextSiblingItem(sibItem);
		}
	}
	else
		vList.push_front(startItem);

	while(vList.size() != 0) {
		HTREEITEM item = vList[0];
		vList.pop_front();
		if(m_cProcSelect.GetItemText(item).CompareNoCase(lpszProcName) == 0)
			return item;
		HTREEITEM childItem = m_cProcSelect.GetChildItem(item);
		while(childItem != NULL) {
			vList.push_front(childItem);
			childItem = m_cProcSelect.GetNextSiblingItem(childItem);
		}
	}
	*/
	return NULL;
}

void CDlgProcessorSelect::OnDestroy() 
{
	FreeItemMemory();
	CDialog::OnDestroy();
}

void CDlgProcessorSelect::FreeItemMemory()
{
	/*
	HTREEITEM hItem = m_cProcSelect.GetRootItem();
	hItem = m_cProcSelect.GetChildItem(hItem);
	while(hItem != NULL) {
		//
		hItem = m_cProcSelect.GetNextSiblingItem(hItem);
	}
	*/
}

BOOL NameExistsInList(CPROCESSOR2LIST* pProcList, LPCTSTR lpszName)
{
	/*
	CString sFindName(lpszName);
	for(int i=0; i<pProcList->size(); i++) {
		CProcessor2* pProc = pProcList->at(i);
		CString sProcName = pProc->Name();
		if (sProcName.GetLength() > sFindName.GetLength() &&
			sFindName.CompareNoCase(sProcName.Left(sFindName.GetLength())) == 0) {
			return TRUE;
		}
		else if(sProcName.CompareNoCase(sFindName.Left(sProcName.GetLength())) == 0) {
			return TRUE;
		}
	}
	*/
	return FALSE;
}

void CDlgProcessorSelect::OnUpdateEditProcName() 
{
	
}

void CDlgProcessorSelect::OnChangeEditProcName() 
{
	/*
	//validate the processor name and enable OK button if valid
	//name is valid if:
	//1) name does not exist
	//2) name is not empty
	//3) name does not contain /,*,&,%,$,#,@,!
	UpdateData(TRUE);
	CWnd* pOK = GetDlgItem(IDOK);
	pOK->EnableWindow(!m_sProcName.IsEmpty() && !NameExistsInList(m_pProcList, m_sProcName));
	*/
}

void CDlgProcessorSelect::OnSelChangedTreeProcNames(NMHDR* pNMHDR, LRESULT* pResult) 
{
	/*
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;
	
	TVITEM item = pNMTreeView->itemNew;
	if(item.mask & TVIF_HANDLE) {
		//put the name of the proc (up to that node) into m_sProcName
		HTREEITEM hItem = item.hItem;
		m_sProcName = m_cProcSelect.GetItemText(hItem);
		while(TRUE) {
			HTREEITEM hParentItem = m_cProcSelect.GetNextItem(hItem, TVGN_PARENT);
			if(hParentItem != TVI_ROOT && hParentItem != NULL)
				m_sProcName = m_cProcSelect.GetItemText(hParentItem) + _T("-") + m_sProcName;
			else
				break;
			hItem = hParentItem;
		}
		UpdateData(FALSE);
	}
	*/
	*pResult = 0;
}
