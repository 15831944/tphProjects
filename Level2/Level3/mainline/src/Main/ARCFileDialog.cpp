// ARCFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "ARCFileDialog.h"
#include "afxdialogex.h"
#include "ACCatPropertiesDlg.h"
#include "DlgAircraftDoors.h"


// ARCFileDialog dialog

IMPLEMENT_DYNAMIC(ARCFileDialog, CDialog)

ARCFileDialog::ARCFileDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ARCFileDialog::IDD, pParent)
{

}

ARCFileDialog::ARCFileDialog( BOOL bOpenFileDialog, /* TRUE for FileOpen, FALSE for FileSaveAs */ 
	LPCTSTR lpszDefExt /*= NULL*/, 
	LPCTSTR lpszFileName /*= NULL*/, 
	DWORD dwFlags /*= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/, 
	LPCTSTR lpszFilter /*= NULL*/, 
	CWnd* pParentWnd /*= NULL*/, 
	DWORD dwSize /*= 0*/, 
	BOOL bVistaStyle /*= TRUE*/ )
	:m_bOpenFileDialog(bOpenFileDialog)
	,m_lpszDefExt(lpszDefExt)
	,m_lpszFileName(lpszFileName)
	,m_dwFlags(dwFlags)
	,m_lpszFilter(lpszFilter)
	,m_pParentWnd(pParentWnd)
	,m_dwSize(dwSize)
	,m_bVistaStyle(bVistaStyle)
{

}

ARCFileDialog::~ARCFileDialog()
{
}

void ARCFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ARCFileDialog, CDialog)
END_MESSAGE_MAP()


INT_PTR ARCFileDialog::DoModal()
{
	return CDialog::DoModal();
}

// ARCFileDialog message handlers
INT_PTR ARCFileDialog::ShowDialog()
{
	//CFileDialog fileDlg(m_bOpenFileDialog, /* TRUE for FileOpen, FALSE for FileSaveAs */ 
	//	m_lpszDefExt /*= NULL*/, 
	//	m_lpszFileName /*= NULL*/, 
	//	m_dwFlags /*= OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT*/, 
	//	m_lpszFilter /*= NULL*/, 
	//	this /*= NULL*/, 
	//	m_dwSize /*= 0*/, 
	//	m_bVistaStyle /*= TRUE*/);

	//INT_PTR fdRet= fileDlg.DoModal();
	//if(fdRet == IDOK)
	//{
	//	m_strFileName = fileDlg.GetPathName();
	//}
	//else
	//{
	//	m_strFileName = "";
	//}

	CDlgAircraftDoors dlg;
	INT_PTR fdRet= dlg.DoModal();

	return fdRet;
}

CString ARCFileDialog::GetPathName() const 
{
	return m_strFileName;
}
