#include "stdafx.h"
#include "CompareReportDoc.h"
#include <shlwapi.h>
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CCompareReportDoc, CDocument)

CCompareReportDoc::CCompareReportDoc()
{
	m_bModified = FALSE;
}

BOOL CCompareReportDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CCompareReportDoc::~CCompareReportDoc()
{
}


BEGIN_MESSAGE_MAP(CCompareReportDoc, CDocument)
	//{{AFX_MSG_MAP(CCompareReportDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
void CCompareReportDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCompareReportDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

void CCompareReportDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
	}
	else
	{
	}
}

BOOL CCompareReportDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace /* = TRUE */)
{
	return m_cmpReport.SaveProject();
}
