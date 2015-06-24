// DotNetTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DotNetTabWnd.h"
#include "ProcessFlowView.h"
using namespace SplitTabCtrlLib;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDotNetTabWnd
CDotNetTabWnd::CDotNetTabWnd()
{
	m_bDeleted = FALSE;
	m_pFlowVect = NULL;
}

CDotNetTabWnd::~CDotNetTabWnd()
{

}


BEGIN_MESSAGE_MAP(CDotNetTabWnd, CWnd)
	//{{AFX_MSG_MAP(CDotNetTabWnd)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_USER+100, OnChildDelete)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// DotNetTabWnd drawing

LRESULT CDotNetTabWnd::OnChildDelete(WPARAM wParam, LPARAM lParam)
{
	if(m_bDeleted)
		return TRUE;

	m_bDeleted = FALSE;

	CString strName = (CString)(char*)wParam;
	
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		if(strName == m_vpWndPane[i]->GetFlowName())
		{
			m_vpWndPane.erase(m_vpWndPane.begin()+i);
			return TRUE;
		}
	}	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// DotNetTabWnd diagnostics

#ifdef _DEBUG
void CDotNetTabWnd::AssertValid() const
{
	CWnd::AssertValid();
}

void CDotNetTabWnd::Dump(CDumpContext& dc) const
{
	CWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDotNetTabWnd message handlers

bool CDotNetTabWnd::Create(LPCTSTR lpszWindowName, DWORD dwStyle,
						   const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (CreateControl(__uuidof(SplitPane), lpszWindowName, dwStyle, rect, pParentWnd, nID))
	{
		ISplitPane* pSplitPane;
		if (FAILED(GetControlUnknown()->QueryInterface(__uuidof(pSplitPane), (void**)&pSplitPane)))
		{
			AfxMessageBox(_T("Failed to query interface!"));
			return false;
		}
		else
		{
			m_pSplitPane.Attach(pSplitPane);
		}
	}
	else
	{
		AfxMessageBox(_T("Failed to create SplitPane!"));
		return false;
	}

	return true;
}

void CDotNetTabWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	for(int i=0; i<static_cast<int>(m_vpWndPane.size()); i++)
	{
		m_vpWndPane[i]->MoveWindow(0, 0, cx, cy);
	}
}

void CDotNetTabWnd::AddChartPane(const CString& strName)
{
	//if the window with the name already exist, return;
	for(int i=0; i<static_cast<int>(m_vpWndPane.size()); i++)
	{
		CString str1 = m_vpWndPane[i]->GetFlowName();
		CString str2 = strName;
		if(_strnicmp(str1.GetBuffer(str1.GetLength()), str2.GetBuffer(str2.GetLength()), str1.GetLength()) == 0)
			return ;
	}

	CFlowChartPane* pPane = new CFlowChartPane;
	if(pPane == NULL)
		return ;


	pPane->Create(NULL, strName, WS_CHILD | WS_VISIBLE,
		CRect(0, 0, 0, 0), this, 0);
	if(pPane->GetSafeHwnd() == NULL)
		return ;
	
	pPane->SetDocumentPtr(((CProcessFlowView*)GetParent())->GetDocument());
	pPane->SetFlowName(strName);
	const std::vector<CMathFlow> *pvFlow = ((CProcessFlowView*)GetParent())->GetMathFlowPtr();
	pPane->SetMathFlowPtr(pvFlow);		
	
	pPane->InitFlowChart();

	CString strTemp = strName;
	char* pText = strTemp.GetBuffer(strTemp.GetLength());
	GetSplitPanePtr()->AddPane( _bstr_t(pText), (long)pPane->GetSafeHwnd(), _bstr_t(pText) );
	strTemp.ReleaseBuffer();

	m_vpWndPane.push_back(pPane);
}

BOOL CDotNetTabWnd::DeleteChartPane(const CString& strName)
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		if(strName.CompareNoCase(m_vpWndPane[i]->GetFlowName())== 0)
		{
			m_bDeleted = TRUE;
			CString str = m_vpWndPane[i]->GetFlowChartFileName();
			GetSplitPanePtr()->DeletePane((long)m_vpWndPane[i]->GetSafeHwnd());
			DeleteFile(str);
			m_vpWndPane.erase(m_vpWndPane.begin()+i);
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CDotNetTabWnd::DeleteAllPane()
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		m_bDeleted = TRUE;
		if(IsWindow(m_vpWndPane[i]->GetSafeHwnd()))
		{
			GetSplitPanePtr()->DeletePane((long)m_vpWndPane[i]->GetSafeHwnd());
		}
	}
	
	m_vpWndPane.clear();
	
	return TRUE;
}

BOOL CDotNetTabWnd::ModifyChartPane(const CString& strOldName, const CString& strNewName)
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		if(strOldName == m_vpWndPane[i]->GetFlowName())
		{
			DeleteFile(m_vpWndPane[i]->GetFlowChartFileName());
			m_vpWndPane[i]->SetFlowName(strNewName);
			CString strTemp = strNewName;
			char* pszName = strTemp.GetBuffer(strTemp.GetLength());
			GetSplitPanePtr()->SetWndInfo((long)m_vpWndPane[i]->GetSafeHwnd(), pszName, pszName);
			strTemp.ReleaseBuffer();
			//the second way: delete the pane named strOldName, and add a new with the strNewName
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL CDotNetTabWnd::ShowChildWnd(const CString& strName)
{

	return FALSE;
}

BOOL CDotNetTabWnd::SaveFlowChartData()
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		m_vpWndPane[i]->SaveFlowChartData();
	}

	return TRUE;
}

BOOL CDotNetTabWnd::DeleteProcess(const CString &strName)
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		m_vpWndPane[i]->DeleteProcess(strName);
	}

	return TRUE;
}

BOOL CDotNetTabWnd::UpdateProcess(const CString &strOld, const CString &strNew)
{
	for(int i = 0; i < static_cast<int>(m_vpWndPane.size()); i++)
	{
		m_vpWndPane[i]->UpdateProcess(strOld, strNew);
	}

	return TRUE;
}
