// ProjectCommentDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProjectCommentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProjectCommentDlg dialog


CProjectCommentDlg::CProjectCommentDlg(CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CProjectCommentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProjectCommentDlg)
	m_commets = _T("");
	//}}AFX_DATA_INIT

	m_pDoc=_pDoc;
}


void CProjectCommentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProjectCommentDlg)
	DDX_Text(pDX, IDC_EDIT_COMMENT, m_commets);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProjectCommentDlg, CDialog)
	//{{AFX_MSG_MAP(CProjectCommentDlg)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_EN_CHANGE(IDC_EDIT_COMMENT, OnChangeEditComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProjectCommentDlg message handlers

int CProjectCommentDlg::DoModal() 
{

	
	ReadFromFile();
	return CDialog::DoModal();
}

void CProjectCommentDlg::ReadFromFile()
{


	CString projectName=this->m_pDoc->m_ProjInfo.path+"\\project_comments.txt";
	
	CFile file;
	CFileException e;
	if(!file.Open(projectName,CFile::modeCreate|CFile::modeNoTruncate|CFile::modeRead),&e)

   {
#ifdef _DEBUG
   afxDump << "File could not be opened " << e.m_cause << "\n";
#endif
   }
;
	
	DWORD dwLength=static_cast<DWORD>(file.GetLength());
	if(dwLength>0)
	{
	char* pBuffer=new char[dwLength+1];
	
	file.Read(pBuffer,dwLength);
	pBuffer[dwLength]='\0';
	this->m_commets=pBuffer;
		
	delete [] pBuffer;

	}

	file.Close();
	
	

	
}

void CProjectCommentDlg::SaveToFile()
{

	CString projectName=this->m_pDoc->m_ProjInfo.path+"\\project_comments.txt";
	
	CFile file;
	file.Open(projectName,CFile::modeCreate|CFile::modeWrite);

	
	file.Write(m_commets,m_commets.GetLength());
	
			
	file.Close();

}

void CProjectCommentDlg::OnSave() 
{

	this->UpdateData();
	//AfxMessageBox(m_commets);
	SaveToFile();

	this->GetDlgItem(IDSAVE)->EnableWindow(false);

}

void CProjectCommentDlg::OnOK() 
{
		this->UpdateData();
	//AfxMessageBox(m_commets);
	SaveToFile();
	
	CDialog::OnOK();
}

void CProjectCommentDlg::OnChangeEditComment() 
{

	this->GetDlgItem(IDSAVE)->EnableWindow();
	
}
