// DockBarEx.h: interface for the CDockBarEx class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKBAREX_H__9F5F5356_D046_46D0_B17E_DA55D8CFDBED__INCLUDED_)
#define AFX_DOCKBAREX_H__9F5F5356_D046_46D0_B17E_DA55D8CFDBED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CDockBarEx Class

class  CDockBarEx : public CDockBar
{
	DECLARE_DYNAMIC(CDockBarEx)

public:

	// Default constructor
	//
	CDockBarEx();

	// Virtual destructor
	//
	virtual ~CDockBarEx();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDockBarEx)
	public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	//{{AFX_MSG(CDockBarEx)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

void FrameEnableDocking(CFrameWnd * pFrame, DWORD dwDockStyle);

/////////////////////////////////////////////////////////////////////////////

#endif // !defined(AFX_DOCKBAREX_H__9F5F5356_D046_46D0_B17E_DA55D8CFDBED__INCLUDED_)
