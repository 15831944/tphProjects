#pragma once
#include "..\compare\ComparativeProject.h"
#include "..\Engine\terminal.h"
#include "..\Compare\CmpReport.h"

class CCompareReportDoc : public CDocument
{
protected:
	CCompareReportDoc(); 
	DECLARE_DYNCREATE(CCompareReportDoc)

public:
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
public:

	//{{AFX_VIRTUAL(CCompareReportDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

public:
	CCmpReport* GetCmpReport(){ return &m_cmpReport; }
	virtual ~CCompareReportDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	CCmpReport m_cmpReport;
	//{{AFX_MSG(CCompareReportDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
