#ifndef __TRACERTAGWND_H__
#define __TRACERTAGWND_H__

class CTermPlanDoc;



class CTracerTagWnd : public CWnd
{
public:
	enum TracerType {
		Terminal,
		Airside,
		Landside,
	};

	CTracerTagWnd(TracerType type, UINT idx, CTermPlanDoc* pDoc);

protected:
	TracerType m_eTracerType;
	UINT m_nTracerIdx;
	CTermPlanDoc* m_pDoc;

	CString m_sTagNames;
	CString m_sTagValues;
	CString m_sTitle;
	CSize m_szTagNames;
	CSize m_szTagValues;

	CString m_sWidestLineValue;

	void DrawTitle(const CRect& r);

	CSize BuildTagStrings(CDC* pDC, CString& _sTagNames, CString& _sTagValues);
	int BuildTitleString(CString& s);

	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};

#endif  // #ifndef __TRACERTAGWND_H__