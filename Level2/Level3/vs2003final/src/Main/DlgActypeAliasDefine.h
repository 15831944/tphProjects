#pragma once
#include "afxwin.h"
#include "../MFCExControl/EditListBox.h"

// CDlgActypeAliasDefine dialog
class CACTypesManager ;
class CAcTypeAlias ;
class CACType ;
class CDlgActypeAliasDefine : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgActypeAliasDefine)

public:
	CDlgActypeAliasDefine(CACTypesManager* _ActypeManger ,  CAcTypeAlias* _Alias ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgActypeAliasDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_ACTYPE_ALIAS };
protected:
	void OnSize(UINT nType, int cx, int cy) ;
	int OnCreate(LPCREATESTRUCT lpCreateStruct) ;

	afx_msg void OnTvnSelchangedAcTypeListBox();
	afx_msg void OnTvnSelchangedAliasListBox();

	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) ;
protected:
	BOOL OnInitDialog() ;
	void OnInitToolBar() ;

	void OnInitActypeListBox() ;
	void OnInitAliasListBox() ;

	void SetActypeText() ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CStatic m_Static_tool;
	CEditListBox m_ListBoxCtrl;
	CListBox m_ActypeListBox ;
	CStatic m_static_Cap;
	CStatic m_static_Spec;
	CToolBar m_toolBar ;
   CStatic m_Static_Name ;
protected:
	CACTypesManager* m_ActypeManager ;
	CAcTypeAlias* m_ALias ;

	CACType* m_CurSelect ;
public:
	afx_msg void OnStnClickedStaticAliasSpec();
protected:
	void OnAddALiasName() ;
	void OnDelAliasName() ;
	void OnEditAliasName() ;
	int FindALiasNameInListBox(CString& _aliasname) ;
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) ;
	afx_msg void OnOK() ;
};
