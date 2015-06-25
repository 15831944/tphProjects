// FloorMapMatch.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FloorMapMatch.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFloorMapMatch dialog


CFloorMapMatch::CFloorMapMatch(CWnd* pParent /*=NULL*/)
	: CDialog(CFloorMapMatch::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFloorMapMatch)
	m_edit = _T("");
	m_static = _T("");
	m_filetype = _T("AutoCad DXF/DWG files");
	//}}AFX_DATA_INIT
}


void CFloorMapMatch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFloorMapMatch)
	DDX_Text(pDX, IDC_EDIT1, m_edit);
	DDX_Text(pDX, IDC_STATIC_TEXT, m_static);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFloorMapMatch, CDialog)
	//{{AFX_MSG_MAP(CFloorMapMatch)
	ON_WM_CANCELMODE()
	ON_BN_CLICKED(IDC_BUTTON1, OnBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFloorMapMatch message handlers

void CFloorMapMatch::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
	
}

void CFloorMapMatch::OnBrowser() 
{
	CString sName,sTempName;
	sTempName = m_name;
	sName.Format("%s %s%s%s%s%s",m_filetype.GetBuffer(),"(",sTempName.GetBuffer(0),")|",sTempName.GetBuffer(0),"||");
//	CFileDialog dlg(TRUE, "dxf", (TCHAR*)m_path.GetBuffer(0), 0, "AutoCad DXF/DWG files (*.dxf, *.dwg)|*.dxf;*.dwg|ARCTERMv1 CAD files (*.cad)|*.cad|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, "dxf", (TCHAR*)m_path.GetBuffer(0), 0, sName.GetBuffer(0));
	if(dlg.DoModal() == IDOK)
	{
		//m_name = dlg.GetFileName();
		m_path = dlg.GetPathName();
		m_edit = m_path;
		UpdateData(false);
	}
	
}

void CFloorMapMatch::OnOK() 
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CFloorMapMatch::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}
