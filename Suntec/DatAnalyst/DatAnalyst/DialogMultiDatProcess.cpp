#include "stdafx.h"
#include "DatAnalyst.h"
#include "DialogMultiDatProcess.h"
#include "MyFolderWalker.h"
#include "DatParser.h"
#include "MyFileDialog.h"
#include "MyImageType.h"

IMPLEMENT_DYNAMIC(CDialogMultiDatProcess, CDialog)

CDialogMultiDatProcess::CDialogMultiDatProcess(CWnd* pParent /*=NULL*/)
: CDialog(CDialogMultiDatProcess::IDD, pParent)
{
}

CDialogMultiDatProcess::~CDialogMultiDatProcess()
{
}

void CDialogMultiDatProcess::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDialogMultiDatProcess, CDialog)
    ON_BN_CLICKED(IDC_BTN_GO, &CDialogMultiDatProcess::OnBnClickedGo)
    ON_BN_CLICKED(IDC_BTN_GETINPUTPATH, &CDialogMultiDatProcess::OnBnClickedBtnGetinputpath)
    ON_BN_CLICKED(IDC_BTN_GETOUTPUTPATH, &CDialogMultiDatProcess::OnBnClickedBtnGetoutputpath)
    ON_WM_SIZE()
END_MESSAGE_MAP()

BOOL CDialogMultiDatProcess::OnInitDialog()
{
    CDialog::OnInitDialog();
    GetDlgItem(IDC_PROGRESS_MULTIDAT)->ShowWindow(FALSE);
    return TRUE;
}

void CDialogMultiDatProcess::OnBnClickedGo()
{
    CString strInput, strOutput;
    GetDlgItem(IDC_EDIT_INPUTPATH)->GetWindowText(strInput);
    GetDlgItem(IDC_EDIT_OUTPUTPATH)->GetWindowText(strOutput);
    CMyFolderWalker myFileWalker;
    if(!myFileWalker.IsDir(strInput))
    {
        CString strMsg;
        strMsg.Format(_T("can not find directory: \"%s\"."), strInput);
        MessageBox(strMsg);
        return;
    }

    if(!myFileWalker.IsDir(strOutput))
    {
        int iErr;
        myFileWalker.CreateDirTree(iErr, strOutput);
    }

    myFileWalker.ListDir(strInput);
    std::vector<CString> datFileList;
    for(size_t i=0; i<myFileWalker.m_vFileItemList.size(); i++)
    {
        CMyFolderItem& myFileItem = myFileWalker.m_vFileItemList[i];
        for(size_t j=0; j<myFileItem.m_fileNames.size(); j++)
        {
            CString strFilePath = myFileItem.m_strCurDir + "\\" + myFileItem.m_fileNames[j];
            if(GetBinaryDataType(strFilePath) == ImageType_Dat)
            {
                datFileList.push_back(strFilePath);
            }
        }
    }

    int iErr = 0;
    DatParser datParser;
    for(size_t i=0; i<datFileList.size(); i++)
    {
        datParser.Init(iErr, datFileList[i]);
        datParser.DatToJpgs(iErr, strOutput);
    }
}

void CDialogMultiDatProcess::OnBnClickedBtnGetinputpath()
{
    TCHAR szFilters[]= _T("All Files|*..*||");
    CMyFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST, szFilters);
    if(IDOK!=dlg.DoModal())
        return;
    CString strPath = dlg.m_strFolderPath;
    GetDlgItem(IDC_EDIT_INPUTPATH)->SetWindowText(strPath);
}

void CDialogMultiDatProcess::OnBnClickedBtnGetoutputpath()
{
    char szPath[MAX_PATH];
    ZeroMemory(szPath, sizeof(szPath));
    BROWSEINFO bi;
    bi.hwndOwner = m_hWnd;
    bi.pidlRoot = NULL;
    bi.pszDisplayName = szPath;
    bi.lpszTitle = "select output path:";
    bi.ulFlags = BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_EDITBOX;
    bi.lpfn = NULL;
    bi.lParam = 0;
    bi.iImage = 0;
    LPITEMIDLIST lp = SHBrowseForFolder(&bi);
    if(lp && SHGetPathFromIDList(lp, szPath))
    {
        GetDlgItem(IDC_EDIT_OUTPUTPATH)->SetWindowText(szPath); 
    }
    else
    {
        return;
    }
}


void CDialogMultiDatProcess::LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy)
{
    CRect rcS;
    pCtrl->GetWindowRect(&rcS);
    ScreenToClient(&rcS);
    int deltaX = cx - m_oldCx;
    int deltaY = cy - m_oldCy;
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
}


void CDialogMultiDatProcess::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);
    if(GetDlgItem(IDC_BTN_GO) == NULL)
    {
        if(nType != SIZE_MINIMIZED)
        {
            m_oldCx = cx;
            m_oldCy = cy;
        }
        return;
    }

    LayoutControl(GetDlgItem(IDC_STATIC_INPUTPATH), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_STATIC_OUTPUTPATH), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_INPUTPATH), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_EDIT_OUTPUTPATH), TopLeft, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_GETINPUTPATH), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_GETOUTPUTPATH), TopRight, TopRight, cx, cy);
    LayoutControl(GetDlgItem(IDC_BTN_GO), TopLeft, TopLeft, cx, cy);
    LayoutControl(GetDlgItem(IDC_PROGRESS_MULTIDAT), TopLeft, TopRight, cx, cy);

    if(nType != SIZE_MINIMIZED)
    {
        m_oldCx = cx;
        m_oldCy = cy;
    }
    InvalidateRect(NULL);
}
