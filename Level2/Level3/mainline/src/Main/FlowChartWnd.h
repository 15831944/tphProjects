#if !defined(AFX_FLOWCHARTWND_H__79FCB70A_7F6B_40F9_8B04_D4AF174EC005__INCLUDED_)
#define AFX_FLOWCHARTWND_H__79FCB70A_7F6B_40F9_8B04_D4AF174EC005__INCLUDED_

// FlowChartWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFlowChartWnd window
#pragma once

#import "..\..\bin\flowchart.dll" no_auto_exclude rename("GetObject", "GetFlowChartObject")

/////////////////////////////////////////////////////////////////////////////
// CFlowChartWnd view
 
class CFlowChartWnd : public CWnd
{
	DECLARE_DYNCREATE(CFlowChartWnd)

public:
	CFlowChartWnd();
	virtual ~CFlowChartWnd();

	bool Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	FLOWCHARTLib::IFlowChartPtr& GetFlowChartPtr() { return m_pFlowChart; }

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	FLOWCHARTLib::IFlowChartPtr m_pFlowChart;

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FLOWCHARTWND_H__79FCB70A_7F6B_40F9_8B04_D4AF174EC005__INCLUDED_)
