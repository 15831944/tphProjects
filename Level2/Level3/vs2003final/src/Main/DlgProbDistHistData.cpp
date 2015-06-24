// DlgProbDistHistData.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgProbDistHistData.h"
#include "inputs\HistogramManager.h"
#include "..\MFCExControl\InPlaceEdit.h"
#include <algorithm>
#include ".\dlgprobdisthistdata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgProbDistHistData dialog

CDlgProbDistHistData::CDlgProbDistHistData(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgProbDistHistData::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgProbDistHistData)
	m_nRecordCount = 0;
	m_strFileName = _T("");
	//}}AFX_DATA_INIT
	m_pvdbValue = NULL;

	m_nMenu.m_hMenu = NULL;
	m_pSubMenu = NULL;

	m_nSelected = -1;
}


void CDlgProbDistHistData::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgProbDistHistData)
	DDX_Control(pDX, IDC_LIST_HISTFILEDATA, m_lstHistFileData);
	DDX_Text(pDX, IDC_EDIT_RECORDCOUNT, m_nRecordCount);
	DDX_Text(pDX, IDC_EDIT_FILENAME, m_strFileName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgProbDistHistData, CDialog)
	//{{AFX_MSG_MAP(CDlgProbDistHistData)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	ON_WM_DESTROY()
	ON_COMMAND(ID_CTX_HISTOGRAM_ADD, OnCtxHistogramAdd)
	ON_COMMAND(ID_CTX_HISTOGRAM_MODIFY, OnCtxHistogramModify)
	ON_COMMAND(ID_CTX_HISTOGRAM_DELETE, OnCtxHistogramDelete)
	ON_COMMAND(ID_CTX_HISTOGRAM_DELETEALL, OnCtxHistogramDeleteall)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_HISTFILEDATA, OnRclickListHistfiledata)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_LIST_HISTFILEDATA, OnItemchangingListHistfiledata)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_HISTFILEDATA, OnEndlabeleditListHistfiledata)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgProbDistHistData message handlers

void CDlgProbDistHistData::OnButtonBrowse() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlgFile( TRUE, ".txt","*.txt",OFN_OVERWRITEPROMPT|OFN_HIDEREADONLY,"txt Files (*.txt)|*.txt||" );

	if (dlgFile.DoModal() != IDOK)
		return ;
	
	m_strFileName = dlgFile.GetPathName();
	
	if(m_strFileName.IsEmpty())
		return ;

	CFileFind f;
	BOOL bRet = f.FindFile(m_strFileName);
	if(!bRet) 
	{
		AfxMessageBox("File not Find!");
		return ;
	}

	SetDlgItemText(IDC_EDIT_FILENAME, m_strFileName);

	AddListItems();
}

void CDlgProbDistHistData::InitListColumns()
{
	CRect rc;
	m_lstHistFileData.GetWindowRect(&rc);
	DWORD dwStyle = m_lstHistFileData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EX_GRIDLINES;
	m_lstHistFileData.SetExtendedStyle( dwStyle );
	int nWidth = (rc.Width() - 22) / 3;
	m_lstHistFileData.InsertColumn(0, _T(""), LVCFMT_CENTER, 0, 0);
	m_lstHistFileData.InsertColumn(1, _T("Index"), LVCFMT_CENTER, nWidth,1);
	m_lstHistFileData.InsertColumn(2, _T("Value"), LVCFMT_CENTER, nWidth,2);
	if(m_nType == 0)
	{
		m_lstHistFileData.InsertColumn(3, _T("Range"), LVCFMT_CENTER, nWidth,3);
//		int nWidth = (rc.Width() > 288) ? (rc.Width()-288) : 0;
//		m_lstHistFileData.InsertColumn(4, _T(""), LVCFMT_CENTER, nWidth, 4);
	}
	else
	{
//		int nWidth = (rc.Width() > 168) ? (rc.Width()-168) : 0;
//		m_lstHistFileData.InsertColumn(3, _T(""), LVCFMT_CENTER, nWidth, 3);
	}

//	LoadProbDistHistList();
}

BOOL CDlgProbDistHistData::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitListColumns();

	if(m_nMenu.m_hMenu == NULL)
		m_nMenu.LoadMenu(IDR_CTX_HISTOGRAM);

	if(m_nType == 0)
		SetWindowText("Histogram File Dialog");
	else
		SetWindowText("Empirical File Dialog");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CDlgProbDistHistData::AddListItems()
{
	if(m_pvdbValue == NULL)
		return 0;

	CSVFile csvFile( m_strFileName, READ );
	// title should be on second line
	
	double b = 0.0;
	do
	{
		csvFile.getFloat(b);
		m_pvdbValue->push_back(b);
	}while(csvFile.getLine ());
	
	
	//load data from the file
	std::sort(m_pvdbValue->begin(), m_pvdbValue->end());
	if(m_pvdbValue->size() <= 0)
	{
		AfxMessageBox("No data in the file");
		return 0;
	}

	AddDataToListCtrl();
	
	return m_pvdbValue->size();
}

void CDlgProbDistHistData::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_nMenu.m_hMenu != NULL)
	{
		m_nMenu.DestroyMenu();
		m_nMenu.m_hMenu = NULL;
		m_pSubMenu = NULL;
	}
}


CWnd* CDlgProbDistHistData::EditSubItem(int nItem, int nSubItem)
{
	CRect rcSubItem;
	m_lstHistFileData.GetSubItemRect( nItem, nSubItem, LVIR_LABEL, rcSubItem );

	CEdit *pEdit = new CInPlaceEdit(nItem, nSubItem, m_lstHistFileData.GetItemText(nItem, nSubItem), IPEDIT_TYPE_NUMBER2);
	pEdit->Create( WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL, rcSubItem, &m_lstHistFileData, IDC_IPEDIT );
	pEdit->BringWindowToTop();
	pEdit->SetFocus();
	return pEdit;
}

void CDlgProbDistHistData::OnCtxHistogramAdd() 
{
	// TODO: Add your command handler code here
	int idx = m_lstHistFileData.GetItemCount();
	CString strIndex = "";
	strIndex.Format("%d", idx+1);
	m_lstHistFileData.InsertItem(LVIF_TEXT, idx, strIndex, LVIS_SELECTED | LVIS_FOCUSED, 0, 0, idx);
	m_lstHistFileData.SetFocus();
	m_lstHistFileData.EnsureVisible( idx, FALSE );

	m_nSelected = idx;
	double d = 0.0;
	m_pvdbValue->push_back(d);

	EditSubItem(idx,1);		
}

void CDlgProbDistHistData::OnCtxHistogramModify() 
{
	// TODO: Add your command handler code here
	EditSubItem(m_nSelected, 1);
}


void CDlgProbDistHistData::OnCtxHistogramDelete() 
{
	// TODO: Add your command handler code here
	ASSERT(m_nSelected!=-1);
	POSITION pos = m_lstHistFileData.GetFirstSelectedItemPosition();
	int idx = m_lstHistFileData.GetNextSelectedItem(pos);
	ASSERT(idx != -1);
	m_lstHistFileData.DeleteItem(idx);
	
//	LoadProbDistHistList();

	m_pvdbValue->erase(m_pvdbValue->begin() + m_nSelected);

	AddDataToListCtrl();
}

void CDlgProbDistHistData::OnCtxHistogramDeleteall() 
{
	// TODO: Add your command handler code here
	m_pvdbValue->erase(m_pvdbValue->begin(), m_pvdbValue->end());

	AddDataToListCtrl();
}

void CDlgProbDistHistData::OnRclickListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_nSelected = pNMListView->iItem;
	
	if(m_nMenu.m_hMenu == NULL)
		return ;
	
	CPoint point;
	GetCursorPos(&point);
	
	m_pSubMenu = DYNAMIC_DOWNCAST(CNewMenu, m_nMenu.GetSubMenu(0));
	if(m_pSubMenu == NULL)
		return ;

	if(m_nSelected < 0)
	{
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_MODIFY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_DELETEALL, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	else
	{
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_MODIFY, MF_BYCOMMAND | MF_ENABLED);
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_DELETE, MF_BYCOMMAND | MF_ENABLED);
		m_pSubMenu->EnableMenuItem(ID_CTX_HISTOGRAM_DELETEALL, MF_BYCOMMAND | MF_ENABLED);
	}

	m_pSubMenu->TrackPopupMenu(0, point.x, point.y, this, NULL);

	*pResult = 0;
}

void CDlgProbDistHistData::OnOK() 
{
	// TODO: Add extra validation here
	if(m_nRecordCount <= 0)
	{
		AfxMessageBox("Please select the file to get the source data!");
		return ;
	}

/*	CHistogramManager manager;
	manager.SetValueVectorPtr(m_pvdbValue);
	manager.saveDataSet(m_strFileName, false);
*/
	CDialog::OnOK();
}

void CDlgProbDistHistData::OnItemchangingListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
//	m_nSelected = pNMListView->iItem;
	
	*pResult = 0;
}

void CDlgProbDistHistData::SortByColumn(int nSortCol)
{
	static bool bSortAscending[3] = { true, true};
	if(nSortCol == m_nSortColumn) 
	{
		//toggle ascending/descending
		bSortAscending[nSortCol] = !(bSortAscending[nSortCol]);
	}
	
	std::pair<int, CListCtrlEx2*> pairSort;
	pairSort = std::make_pair(nSortCol, (CListCtrlEx2*)&m_lstHistFileData);

	if(bSortAscending[nSortCol])
		m_lstHistFileData.SortItems(CompareFuncAscending, (LPARAM)&pairSort);
	else
		m_lstHistFileData.SortItems(CompareFuncDescending, (LPARAM)&pairSort);
	
	m_nSortColumn = nSortCol;
}

int CALLBACK CDlgProbDistHistData::CompareFuncAscending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int idx1 = (int) lParam1;
	int idx2 = (int) lParam2;

	std::pair<int, CListCtrlEx2*> pairSort = *(std::pair<int, CListCtrlEx2*>*)lParamSort;
	CListCtrlEx2* pListCtrl = ( CListCtrlEx2*)pairSort.second;

	CString str1 = pListCtrl->GetItemText(idx1, pairSort.first);
	CString str2 = pListCtrl->GetItemText(idx2, pairSort.first);
	const char* pch1 = (LPCTSTR)str1;
	const char* pch2 = (LPCTSTR)str2;
	int nResult;
	switch(pairSort.first) 
	{
		case 0://
			{
				int i1 = ::atoi(pch1);
				int i2 = ::atoi(pch2);
				if(i1 == i2)
					nResult = 0;
				else
					nResult = i1 < i2 ? -1 : 1;
				break;
			}
		case 1: //
			{
				double d1 = ::atof(pch1);
				double d2 = ::atof(pch2);
				if(d1 == d2)
					nResult = 0;
				else
					nResult = d1 < d2 ? -1 : 1;
			break;
			}
		case 2: //
			break;
		default:
			ASSERT(0);
			break;
		
	}

	return nResult;
}

int CALLBACK CDlgProbDistHistData::CompareFuncDescending(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	int idx1 = (int) lParam2;
	int idx2 = (int) lParam1;

	std::pair<int, CListCtrlEx2*> pairSort = *(std::pair<int, CListCtrlEx2*>*)lParamSort;
	CListCtrlEx2* pListCtrl = ( CListCtrlEx2*)pairSort.second;

	CString str1 = pListCtrl->GetItemText(idx1, pairSort.first);
	CString str2 = pListCtrl->GetItemText(idx2, pairSort.first);
	const char* pch1 = (LPCTSTR)str1;
	const char* pch2 = (LPCTSTR)str2;
	int nResult;
	switch(pairSort.first) 
	{
		case 0://
			{
				int i1 = ::atoi(pch1);
				int i2 = ::atoi(pch2);
				if(i1 == i2)
					nResult = 0;
				else
					nResult = i1 < i2 ? -1 : 1;
				break;
			}
		case 1: //
			{
				double d1 = ::atof(pch1);
				double d2 = ::atof(pch2);
				if(d1 == d2)
					nResult = 0;
				else
					nResult = d1 < d2 ? -1 : 1;
			break;
			}
		case 2: //
			break;
		default:
			ASSERT(0);
			break;
		
	}

	return nResult;
}

void CDlgProbDistHistData::LoadProbDistHistList()
{	
	m_lstHistFileData.DeleteAllItems();
	for( int i=0; i<static_cast<int>(m_nRecordCount); i++ )
	{
		//CProbDistEntry* pProbDistItem = PROBDISTMANAGER->getItem( i );
		m_lstHistFileData.InsertItem(LVIF_TEXT | LVIF_PARAM, i, LPSTR_TEXTCALLBACK, 0, 0, 0, (LPARAM) i);
	}
}

void CDlgProbDistHistData::OnEndlabeleditListHistfiledata(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	if(!pDispInfo->item.pszText)
		return;

	int nItem = pDispInfo->item.iItem;
	int nSubItem = pDispInfo->item.iSubItem;

	// TRACE("End Label Edit: %s, item %d, subitem %d\n", pDispInfo->item.pszText, nItem, nSubItem);

	double val = atof(pDispInfo->item.pszText);
	if(nSubItem == 1)
	{
		m_pvdbValue->at(m_nSelected) = val;
	}

	AddDataToListCtrl();
	
	*pResult=1;
}

void CDlgProbDistHistData::AddDataToListCtrl()
{
	m_lstHistFileData.DeleteAllItems();

	if(m_pvdbValue == NULL)
		return ;

	if( (m_nRecordCount = m_pvdbValue->size()) <= 0)
		return ;
	
	std::sort(m_pvdbValue->begin(), m_pvdbValue->end());

	CString strText = "";
	for(int i = 0; i < static_cast<int>(m_pvdbValue->size()); i++)
	{
		m_lstHistFileData.InsertItem(LVIF_TEXT, i, "", LVIS_SELECTED | LVIS_FOCUSED, 0, 0, NULL);
		strText.Format("%d", i+1);
		m_lstHistFileData.SetItemText(i, 1, strText);
		strText.Format("%.2f", m_pvdbValue->at(i));
		m_lstHistFileData.SetItemText(i, 2, strText);
		if(m_nType == 0)
		{
			int nIndex = ValueInRange(static_cast<int>(m_pvdbValue->at(i)));
			if(nIndex < 0)
				strText = "(Unknown - Unknown]";
			else
				strText.Format("(%.2f - %.2f]", m_pvPairRange->at(nIndex).first, m_pvPairRange->at(nIndex).second);
			m_lstHistFileData.SetItemText(i, 3, strText);
		}
	}

	SetDlgItemInt(IDC_EDIT_RECORDCOUNT, m_nRecordCount);

	m_lstHistFileData.Invalidate();
}

int CDlgProbDistHistData::ValueInRange(int nValue)
{
	for(int i = 0; i < static_cast<int>(m_pvPairRange->size()); i++)
	{
		if( nValue == m_pvPairRange->at(0).first )
			return 0;
		else if( m_pvPairRange->at(i).first < nValue && nValue <= m_pvPairRange->at(i).second )
			return i;
	}

	return -1;
}

void CDlgProbDistHistData::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
