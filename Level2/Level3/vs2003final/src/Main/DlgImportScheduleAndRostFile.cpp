// DlgImportScheduleAndRostFile.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgImportScheduleAndRostFile.h"
#include <algorithm>

// CDlgImportScheduleAndRostFile dialog

IMPLEMENT_DYNAMIC(CDlgImportScheduleAndRostFile, CXTResizeDialog)
CDlgImportScheduleAndRostFile::CDlgImportScheduleAndRostFile(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgImportScheduleAndRostFile::IDD, pParent)
	,m_strScheduleName(_T(""))
	,m_strRosterName(_T(""))
{

}

CDlgImportScheduleAndRostFile::~CDlgImportScheduleAndRostFile()
{
}

void CDlgImportScheduleAndRostFile::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_LIST_DATA,m_wndListCtrl);
}


BEGIN_MESSAGE_MAP(CDlgImportScheduleAndRostFile, CXTResizeDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPEN,OnBtnOpen)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DATA, OnColumnclickListFile)
	ON_MESSAGE(WM_XLISTCTRL_DBCLICKED,OnDBClickListCtrl)
END_MESSAGE_MAP()

LRESULT CDlgImportScheduleAndRostFile::OnDBClickListCtrl(WPARAM wParam,LPARAM lParam)
{
	return FALSE;
}

// CDlgImportScheduleAndRostFile message handlers
BOOL CDlgImportScheduleAndRostFile::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	FillListHeader();

	SetResize(IDC_STATIC_PATH,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_EDIT_PATH,SZ_TOP_LEFT,SZ_TOP_RIGHT);
	SetResize(IDC_BUTTON_OPEN,SZ_TOP_RIGHT,SZ_TOP_RIGHT);
	SetResize(IDC_STATIC_GROUP,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_FILE,SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_DATA,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);

	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);
	return TRUE;
}

void CDlgImportScheduleAndRostFile::OnColumnclickListFile(NMHDR* pNMHDR, LRESULT* pResult)
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

void CDlgImportScheduleAndRostFile::FillListHeader()
{
	DWORD dwStyle = m_wndListCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_wndListCtrl.SetExtendedStyle( dwStyle );

	m_wndSortHeaderList.SubclassWindow( m_wndListCtrl.GetHeaderCtrl()->GetSafeHwnd() );

	const char* colLabelEnvironment[] = { "Schedule file", "Roster file"};
	const int nFormatEnvironment[] = { LVCFMT_LEFT, LVCFMT_LEFT};
	const int DefaultColumnWidthEnvironment[] = { 250,250};
	const EDataType typeEnvironment[] = { dtSTRING, dtSTRING};

	int nColNum = 2;
	for( int i=0; i<nColNum; i++ )
	{
		m_wndListCtrl.InsertColumn( i, colLabelEnvironment[i], nFormatEnvironment[i], DefaultColumnWidthEnvironment[i] );
		m_wndSortHeaderList.SetDataType( i, typeEnvironment[i] );
	}
}

void CDlgImportScheduleAndRostFile::OnBtnOpen()
{
 	CXTBrowseDialog browseDlg;
	browseDlg.SetOptions(BIF_RETURNONLYFSDIRS);
	CString szFolderPath(_T(""));
 	if(browseDlg.DoModal() != IDOK)
		return;

	m_folderParseList.ParseFolder(browseDlg.GetSelPath());
	FillListCtrl();
	GetDlgItem(IDC_EDIT_PATH)->SetWindowText(browseDlg.GetSelPath());
}

void CDlgImportScheduleAndRostFile::FillListCtrl()
{
	if (m_folderParseList.IsEmpty())
		return;
	
	m_wndListCtrl.DeleteAllItems();

	for (int i = 0; i < m_folderParseList.GetCount(); i++)
	{
		FolderParse folderParse = m_folderParseList.GetFolderParse(i);
		for (int j = 0; j < folderParse.GetCount(); j++)
		{
			FilePair& filePair = folderParse.GetFilePair(j);
			int nIdx = m_wndListCtrl.GetItemCount();
			m_wndListCtrl.InsertItem(nIdx,filePair.get<0>());

			m_wndListCtrl.SetItemText(nIdx,1,filePair.get<1>());
			m_wndListCtrl.SetItemData(nIdx,(DWORD_PTR)i);
		}
	}
}

void CDlgImportScheduleAndRostFile::OnOK()
{
	int nCurSel = m_wndListCtrl.GetCurSel();

	if (nCurSel == LB_ERR)
	{
		::AfxMessageBox(_T("Please select one pair of schedule and roster file"));
		return;
	}
	m_strScheduleName = m_wndListCtrl.GetItemText(nCurSel,0);
	m_strRosterName = m_wndListCtrl.GetItemText(nCurSel,1);

	int nIdx = (int)m_wndListCtrl.GetItemData(nCurSel);
	m_strFilePath = m_folderParseList.GetFolderParse(nIdx).GetFolderPath();

	if (m_strScheduleName.IsEmpty() || m_strRosterName.IsEmpty())
	{
		::AfxMessageBox(_T("Please select one valid pair of schedule and roster file"));
		return;
	}

	CXTResizeDialog::OnOK();
}

CString CDlgImportScheduleAndRostFile::GetScheduleFilePath()const
{
	return CString(m_strFilePath + "//" + m_strScheduleName);
}

CString CDlgImportScheduleAndRostFile::GetRosterFilePath()const
{
	return CString(m_strFilePath + "//" + m_strRosterName);
}

CString CDlgImportScheduleAndRostFile::GetScheduleFileName()const
{
	return m_strScheduleName;
}

CString CDlgImportScheduleAndRostFile::GetRosterFileName()const
{
	return m_strRosterName;
}
///////////////////////////////////////////////////////////////////////////////////////
const std::string FolderParse::m_sScheduleName = _T("AMSARCPORTSCLExport");
const std::string FolderParse::m_sRosterName = _T("AMSARCPORTRESExport");
const int FolderParse::m_iNumLength = 12;
//folder parse 
FolderParse::FolderParse(const CString& szPath)
:m_strFolderPath(szPath)
{

}

FolderParse::~FolderParse()
{

}

bool FolderParse::IsEmpty()const
{
	return m_vPairFile.empty();
}

int FolderParse::GetCount()const
{
	return (int)m_vPairFile.size();
}

FilePair& FolderParse::GetFilePair(int nPair)
{
	ASSERT(nPair >= 0 && nPair < GetCount());
	return m_vPairFile[nPair];
}

void FolderParse::ParseFolder()
{
	TCHAR oldDir[MAX_PATH+1];
	GetCurrentDirectory(MAX_PATH, oldDir);
	if (!SetCurrentDirectory(m_strFolderPath))
		return;
	

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind = FindFirstFile("*.*", &FindFileData);	// gets "."
	FindNextFile(hFind, &FindFileData);				// gets ".."
	while(GetLastError() != ERROR_NO_MORE_FILES) 
	{
		if (!FindNextFile(hFind, &FindFileData))
			break;
		
		if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) 
		{
			AddFileName(FindFileData.cFileName);
		}		
	}
	SetCurrentDirectory(oldDir);
	SetLastError(0);
}

bool FolderParse::fitRosterAndSetVaule(const CString& szFileName)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CString szName(szFileName);
		FilePair& filePair = GetFilePair(i);
		CString sScheduleName = filePair.get<1>();
		if (sScheduleName == szFileName)
			continue;
		if (!sScheduleName.IsEmpty())
			continue;

		szName.Replace(_T("RES"),_T("SCL"));
		if (filePair.get<0>() == szName)
		{
			filePair.get<1>() = szFileName;
			return true;
		}
	}

	m_vPairFile.push_back(boost::make_tuple(_T(""),szFileName));
	return false;
}

bool FolderParse::fitScheduleAndSetVaule(const CString& szFileName)
{
	for (int i = 0; i < GetCount(); i++)
	{
		CString szName(szFileName);
		FilePair& filePair = GetFilePair(i);
		CString sRosterName = filePair.get<0>();
		if (!sRosterName.IsEmpty())
			continue;

		szName.Replace(_T("SCL"),_T("RES"));
		if (filePair.get<1>() == szName)
		{
			filePair.get<0>() = szFileName;
			return true;
		}
	}
	m_vPairFile.push_back(boost::make_tuple(szFileName,_T("")));
	return false;
}

bool FolderParse::AddFileName(const CString& szFileName)
{
	int nLeftCount = strlen(szFileName) - 4;
	CString szName = szFileName.Left(nLeftCount);
	//schedule fit
	if (scheduleFile(szName))
	{
		fitScheduleAndSetVaule(szFileName);	
		return true;
	}
	//roster fit
	else if (rosterFile(szName))
	{
		fitRosterAndSetVaule(szFileName);
		return true;
	}

	return false;
}

bool FolderParse::scheduleFile(const CString& szFileName)const
{
	if(szFileName.Find(m_sScheduleName.c_str()) < 0)
		return false;
	
	CString szTime = szFileName.Right(m_iNumLength);
	if (!IsNum(szTime.GetString()))
		return false;

	return true;
}

bool FolderParse::rosterFile(const CString& szFileName)const
{
	if(szFileName.Find(m_sRosterName.c_str()) < 0)
		return false;

	CString szTime = szFileName.Right(m_iNumLength);
	if (!IsNum(szTime.GetString()))
		return false;

	return true;
}

bool FolderParse::IsNum(const LPCTSTR pszText)const
{
	for( int i = 0; i < lstrlen( pszText ); i++ )
		if( !_istdigit( pszText[ i ] ) )
			return false;

	return true;
}

bool FolderParse::operator ==(const FolderParse& folderParse)const
{
	if (m_strFolderPath == folderParse.m_strFolderPath)
	{
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////
//////FolderParseList
FolderParseList::FolderParseList()
{

}

FolderParseList::~FolderParseList()
{

}

void FolderParseList::ParseFolder(const CString& szPath)
{
	FolderParse folderParse(szPath);
	if(std::find(m_vFolderList.begin(),m_vFolderList.end(),folderParse) == m_vFolderList.end())
	{
		folderParse.ParseFolder();
		m_vFolderList.push_back(folderParse);
	}
}

void FolderParseList::AddFolderParse(const FolderParse& folderParse)
{
	m_vFolderList.push_back(folderParse);
}

const FolderParse& FolderParseList::GetFolderParse(int idx)const
{
	ASSERT(idx >= 0 && idx < GetCount());
	return m_vFolderList[idx];
}

int FolderParseList::GetCount()const
{
	return (int)m_vFolderList.size();
}

bool FolderParseList::IsEmpty()const
{
	return m_vFolderList.empty();
}