#include "stdafx.h"
#include "TermPlan.h"
#include "ImportUserShapeBarDlg.h"
#include "afxdialogex.h"
#include "Common\ProjectManager.h"
#include "Common\ZipInfo.h"
#include "Common\DirectoryOperation.h"
#include "Common\TERMFILE.H"

IMPLEMENT_DYNAMIC(CImportUserShapeBarDlg, CDialog)

CImportUserShapeBarDlg::CImportUserShapeBarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CImportUserShapeBarDlg::IDD, pParent),
    m_oldWindowWidth(0),
    m_oldWindowHeight(0),
    m_strZipFileName(_T(""))
{
}

CImportUserShapeBarDlg::~CImportUserShapeBarDlg()
{
}

void CImportUserShapeBarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ZIPFILENAME, m_editZipFileName);
	DDX_Control(pDX, IDC_EDIT_SHAPEBARLOCATION, m_editBarLocation);
    DDX_Control(pDX, IDC_EDIT_SHAPEBARNAME, m_editBarName);
}


BEGIN_MESSAGE_MAP(CImportUserShapeBarDlg, CDialog)
	ON_BN_CLICKED(IDC_BTN_LOADZIP, OnLoadZip)
	ON_BN_CLICKED(IDC_BTN_SELECTLOCATION, OnSelectLocation)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_BN_CLICKED(IDOK, OnOk)
    ON_WM_SIZE()
    ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CImportUserShapeBarDlg message handlers
void CImportUserShapeBarDlg::OnLoadZip()
{
    CFileDialog  dlgFile(TRUE,NULL,NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"(*.zip)|*.zip|");
    if (dlgFile.DoModal() == IDOK)
    {
        m_strZipFileName = dlgFile.GetPathName();

        // unzip to temp file
        CZipInfo zipInfo;
        if(PathFileExists(m_strTempPath) == TRUE)
        {
            myDeleteDirectory(m_strTempPath);
        }
        zipInfo.ExtractFiles(m_strZipFileName, m_strTempPath);
        // source directory file list
        std::vector<CString> vSrcDirFL;
        FindFiles(m_strTempPath, vSrcDirFL);
        CString strShapeFileName;
        for(int i=0; i<(int)vSrcDirFL.size(); i++)
        {
            if(m_strShapeFileName.Compare(vSrcDirFL.at(i)) == 0)
            {
                strShapeFileName = vSrcDirFL.at(i);
                break;
            }
        }

        if(strShapeFileName.IsEmpty()) // a file named "export_shape_bars.txt" can not be found
        {
            myDeleteDirectory(m_strTempPath);
            CString strErr = "Selected Zip file is not exported shape bar file.";
            MessageBox(strErr, NULL, MB_OK);
            return;
        }

        // parse the bar information file
        m_userShapeMan.loadDataSetFromOtherFile(m_strTempPath + "\\" + m_strShapeFileName);

        CUserShapeBar* pImportBar = m_userShapeMan.GetUserBarByIndex(0);
		pImportBar->SetBarLocation(m_strBarLocation);
        m_editZipFileName.SetWindowText(m_strZipFileName);
        m_editBarName.SetWindowText(pImportBar->GetBarName());
        m_editBarLocation.SetWindowText(pImportBar->GetBarLocation());
        UpdateData(FALSE);
    }
}


void CImportUserShapeBarDlg::OnSelectLocation()
{
    char szPath[MAX_PATH] = {0};
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "Shape Bar location";
    bi.ulFlags = 0;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);

    CString ExportFile;
    if(lp && SHGetPathFromIDList(lp, szPath))
    {
        m_editBarLocation.SetWindowText(szPath);
        UpdateData(FALSE);
    }
}

BOOL CImportUserShapeBarDlg::OnInitDialog()
{
    CDialog::OnInitDialog();
    CUserShapeBar* pImportBar = m_userShapeMan.GetUserBarByIndex(0);
    if(pImportBar)
    {
        m_editBarName.SetWindowText(pImportBar->GetBarName());
        m_editBarLocation.SetWindowText(pImportBar->GetBarLocation());
    }
    m_editZipFileName.SetWindowText(m_strZipFileName);
    return TRUE;
}

void CImportUserShapeBarDlg::OnCancel()
{
    CDialog::OnCancel();
}


void CImportUserShapeBarDlg::OnOk()
{
    m_editZipFileName.GetWindowText(m_strZipFileName);
    CString str;
    CUserShapeBar* pImportBar = m_userShapeMan.GetUserBarByIndex(0);
    m_editBarName.GetWindowText(str);
    pImportBar->SetBarName(str);
    m_editBarLocation.GetWindowText(str);
    pImportBar->SetBarLocation(str);

    if(m_strZipFileName.IsEmpty())
    {
        CString strErr = "Please select an exported shape bar zip file.";
        MessageBox(strErr, NULL, MB_OK);
        return;
    }

    if(pImportBar->GetBarName().IsEmpty())
    {
        CString strErr = "Please input a shape bar name.";
        MessageBox(strErr, NULL, MB_OK);
        return;
    }

    if(pImportBar->GetBarLocation().IsEmpty())
    {
        CString strErr = "Please select a file location.";
        MessageBox(strErr, NULL, MB_OK);
        return;
    }

    CDialog::OnOK();
}


void CImportUserShapeBarDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(m_editZipFileName.m_hWnd == NULL)
    {
        if (nType != SIZE_MINIMIZED)
        {
            m_oldWindowWidth = cx;
            m_oldWindowHeight = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_STATIC_SHAPEBARNAME), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_ZIPFILE), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_BARLOCATION), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_SHAPEBARNAME), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_ZIPFILENAME), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_SHAPEBARLOCATION), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_SELECTLOCATION), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_LOADZIP), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDOK), BottomRight, BottomRight, cx, cy);
    LayoutControl(GetDlgItem(IDCANCEL), BottomRight, BottomRight, cx, cy);
    if (nType != SIZE_MINIMIZED)
    {
        m_oldWindowWidth = cx;
        m_oldWindowHeight = cy;
    }
    InvalidateRect(NULL);
}

// refTopLeft: pCtrl's top left references to the new window's 'refTopLeft'.
// refBottomRight: pCtrl's bottom right references to the new window's 'refBottomRight'.
// cx: new window's width
// cy: new window's height
void CImportUserShapeBarDlg::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldWindowWidth;
    int deltaY = cy - m_oldWindowHeight;
    if(refTopLeft == TopLeft && refBottomRight == TopLeft)
    {
        pCtrl->MoveWindow(&rcS);
    }
    else if(refTopLeft == TopLeft && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top, rcS.Width()+deltaX, rcS.Height()+deltaY);
    }
    else if(refTopLeft == TopRight && refBottomRight == TopRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == TopRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top, rcS.Width(), rcS.Height()+deltaY);
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomLeft)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else if(refTopLeft == BottomLeft && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left, rcS.top+deltaY, rcS.Width()+deltaX, rcS.Height());
    }
    else if(refTopLeft == BottomRight && refBottomRight == BottomRight)
    {
        pCtrl->MoveWindow(rcS.left+deltaX, rcS.top+deltaY, rcS.Width(), rcS.Height());
    }
    else
    {
        return;
    }
}

void CImportUserShapeBarDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
    lpMMI->ptMinTrackSize=CPoint(369,300);
    CDialog::OnGetMinMaxInfo(lpMMI);
}

int CImportUserShapeBarDlg::FindFiles( CString pathName ,std::vector<CString>& vFiles )
{
    WIN32_FIND_DATA FindFileData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    TCHAR PathBuffer[_MAX_PATH];

    _tcscpy_s(PathBuffer,_MAX_PATH, pathName.GetBuffer());
    _tcscat_s(PathBuffer,_MAX_PATH, _T("\\*"));

    hFind = FindFirstFile( PathBuffer ,&FindFileData );
    if( INVALID_HANDLE_VALUE == hFind )
    {
        return 0;
    }
    else
    {
        CString strFile(FindFileData.cFileName);
        // do not add "." and ".." to the found list
        if(strFile.Compare(".") != 0 && strFile.Compare("..") != 0)
            vFiles.push_back(strFile);
        while( 0 != FindNextFile( hFind,&FindFileData ))
        {
            strFile = FindFileData.cFileName;
            if(strFile.Compare(".") != 0 && strFile.Compare("..") != 0)
                vFiles.push_back(CString(FindFileData.cFileName));
        }
        FindClose( hFind );
    }
    return (int)vFiles.size();
}
