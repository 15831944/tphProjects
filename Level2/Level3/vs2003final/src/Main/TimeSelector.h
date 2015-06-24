#if !defined(AFX_TIMESELECTOR_H__7B6ECD10_0927_4DD0_B7D8_064F8AE6E883__INCLUDED_)
#define AFX_TIMESELECTOR_H__7B6ECD10_0927_4DD0_B7D8_064F8AE6E883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeSelector.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTimeSelector window

class CTimeSelector : public CStatic
{
// Construction
public:
	CTimeSelector();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeSelector)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeSelector();

	void SetMaxTime(long nTime);
	void SetMinTime(long nTime);
	void SetStartTime(long nTime);
	void SetEndTime(long nTime);

	long GetMaxTime() { return m_nMaxTime; }
	long GetMinTime() { return m_nMinTime; }
	long GetStartTime() { return m_nStartTime; }
	long GetEndTime() { return m_nEndTime; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeSelector)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	long m_nMaxTime;		//max end time
	long m_nMinTime;		//min start time
	long m_nStartTime;		//start time
	long m_nEndTime;		//end time

	HBITMAP m_hMarkerBitmap;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMESELECTOR_H__7B6ECD10_0927_4DD0_B7D8_064F8AE6E883__INCLUDED_)
