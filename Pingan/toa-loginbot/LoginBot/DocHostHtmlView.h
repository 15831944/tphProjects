#if !defined(AFX_DOCHOSTHTMLVIEW_H__37829C74_D5BD_43F0_9F7A_BAD0B87E4479__INCLUDED_)
#define AFX_DOCHOSTHTMLVIEW_H__37829C74_D5BD_43F0_9F7A_BAD0B87E4479__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DocHostHtmlView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDocHostHtmlView html view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif
#include <afxhtml.h>

#ifndef _DOCHOSTSITE_H_
#include <mshtmhst.h>
#endif

class CDocHostHtmlView : public CHtmlView
{
protected:
	CDocHostHtmlView();           // protected constructor used by dynamic creation
	virtual ~CDocHostHtmlView();
	DECLARE_DYNCREATE(CDocHostHtmlView)

// Attributes
private:
	class CDocHostOccManager * m_pOccManager;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDocHostHtmlView)
public:

	//}}AFX_VIRTUAL

// Overrides
protected:
	// CHtmlView overrides
	virtual BOOL Create(LPCTSTR lpszClassName,
		LPCTSTR lpszWindowName,
		DWORD dwStyle,
		const RECT& rect,
		CWnd* pParentWnd,
		UINT nID,
		CCreateContext* pContext = NULL);
public:
	// for ambient properties exposed to contained OLE controls
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid,
		VARIANT* pvar);

#if _MFC_VER >= 0x0700
	virtual BOOL CreateControlSite(COleControlContainer* pContainer, 
	   COleControlSite** ppSite, UINT nID, REFCLSID clsid);
#endif

	// IDocHostShowUI overridables
	virtual HRESULT OnShowHelp(HWND hwnd,
		LPOLESTR pszHelpFile,
		UINT uCommand,
		DWORD dwData,
		POINT ptMouse,
		IDispatch * pDispatchObjectHit);

	virtual HRESULT OnShowMessage(HWND hwnd,
		LPOLESTR lpstrText,
		LPOLESTR lpstrCaption,
		DWORD dwType,
		LPOLESTR lpstrHelpFile,
		DWORD dwHelpContext,
		LRESULT * plResult);

	// INewWindowManager overridables
	virtual HRESULT EvaluateNewWindow(LPCWSTR pszUrl,
		LPCWSTR,
		LPCWSTR,
		LPCWSTR,
		BOOL,
		DWORD,
		DWORD);

	// IDocHostUIHandler
	HRESULT TranslateAccelerator(LPMSG lpMsg,
		const GUID * pguidCmdGroup,
		DWORD nCmdID);

#ifdef _DOCHOSTSITE_H_
	friend class CDocHostSite::XDocHostShowUI;
	friend class CDocHostSite::XDocHostUIHandler;
#if (_WIN32_IE >= 0x0501) // IE 5.5 and higher
	friend class CDocHostSite::XNewWindowManager;
	friend class CDocHostSite::XServiceProvider;
	friend class CDocHostSite::XCustHostDialogHelper;
	friend class CDocHostSite::XInternetSecurityManager;
#endif
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CDocHostHtmlView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DOCHOSTHTMLVIEW_H__37829C74_D5BD_43F0_9F7A_BAD0B87E4479__INCLUDED_)
