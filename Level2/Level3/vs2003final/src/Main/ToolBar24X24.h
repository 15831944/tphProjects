// ToolBar24X24.h: interface for the CToolBar24X24 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBAR24X24_H__BFB78312_A1EF_42EE_AF09_7C344A3C672C__INCLUDED_)
#define AFX_TOOLBAR24X24_H__BFB78312_A1EF_42EE_AF09_7C344A3C672C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <afxtempl.h>
#include "XPStyle/ToolBarXP.h"
#define TEMP_ID_FOR_NOT_BUTTON 0
class CToolBar24X24 : public CToolBar  
{
public:
	void AppendImageList(UINT nIDImageNormal, UINT nIDImageHot, UINT nIDImageDisable, COLORREF clrMask);
	int m_nCountNotButton;
	void AddButtonAndResize(UINT nID,int cx);
	static void InitTBBUTTON(TBBUTTON& tbb);
	CSize GetTextSize(CString str);
	void AddTextComboBox(CString strTitle,CComboBox* pComboBox, CSize sizeEx,UINT nID,DWORD dwStyleAppend=0);
	void AddComboBox(CComboBox* pComboBox,CSize size,UINT nID,DWORD dwStyleAppend=0);
	void InitFont(int nPointSize, LPCTSTR lpszFaceName);
	CFont m_Font;
	void SetImageList();
	void InitImageList(UINT nIDImageNormal,UINT nIDImageHot,UINT nIDImageDisable,COLORREF clrMask);
	void AddButtonSep();
	void AddButton(TBBUTTON* pTBB,int cx=-1,BOOL bTempButton=FALSE);
	void AddButtons(int nBtnCount,TBBUTTON* pTBB,int cx=-1);
	BOOL Create(CWnd* pParent,UINT nID,DWORD dwStyleAppend=0);
	CImageList m_ImageListDisable;
	CImageList m_ImageListHot;
	CImageList m_ImageListNormal;
	CArray<CStatic*,CStatic*> m_titleList;
	CToolBar24X24();
	virtual ~CToolBar24X24();
protected: 
	// Generated message map functions
	//{{AFX_MSG(CToolBar24X24)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

#endif // !defined(AFX_TOOLBAR24X24_H__BFB78312_A1EF_42EE_AF09_7C344A3C672C__INCLUDED_)
