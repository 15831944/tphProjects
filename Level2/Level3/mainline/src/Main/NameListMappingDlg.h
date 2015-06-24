#if !defined(AFX_NAMELISTMAPPINGDLG_H__3E343EE0_4C5E_450F_8BAE_9F88C9E75BE9__INCLUDED_)
#define AFX_NAMELISTMAPPINGDLG_H__3E343EE0_4C5E_450F_8BAE_9F88C9E75BE9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NameListMappingDlg.h : header file
//
#include "ctrlToolbar.h"
#include "ListCtrlDrag.h"
#include "XPStyle\ComboBoxXP.h"
#include <MFCExControl/SplitterControl.h>
#include <vector>
class InputTerminal;
/////////////////////////////////////////////////////////////////////////////
// CNameListMappingDlg dialog
#define ID_COMBOBOX 100
class CNameListMappingDlg : public CDialog
{
// Construction
public:
	void DoResizeRight(int delta);
	void DoResizeLeft(int delta);
	CSplitterControl m_wndSplitterLeft;
	CSplitterControl m_wndSplitterRight;
	int m_nColIndexClicked;
	int m_nSeledRowIndex;
	HCURSOR m_hcNo;
	HCURSOR m_hcArrow;
	CWnd* m_pDragWnd;
	BOOL m_bColumnDropped[9];
	
	CComboBoxXP m_cmbScale;
	~CNameListMappingDlg();
	BOOL m_bInit;
	CCtrlToolBar m_wndToolBarRight;
	CToolBar m_wndToolBarLeft;
	CNameListMappingDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL          m_bDragging;       
	CImageList*   m_pDragImage;      
	HTREEITEM     m_hItemDragSrc;   
	HTREEITEM	  m_hItemDragDes;
	CImageList	  m_imageList;

	
// Dialog Data
	//{{AFX_DATA(CNameListMappingDlg)
	enum { IDD = IDD_DIALOG_NAMELISTMAP };
	CButton	m_butFile;
	CButton	m_butOK;
	CButton	m_butCancel;
	CTreeCtrl	m_treeProc;
	CStatic	m_staticText;
	CStatic	m_staticProc;
	CStatic	m_staticMapping;
	CListCtrlDrag	m_listText;
	CListCtrlDrag	m_listMapping;
	CButton	m_butSave;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNameListMappingDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNameListMappingDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonFile();
	afx_msg void OnBegindragTreeProc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickListText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListMapping(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnButtonSave();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDblclkListText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedListText(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNamelistTitle();
	afx_msg void OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPeoplemoverNew();
	//}}AFX_MSG
	afx_msg LRESULT OnBegindrag(WPARAM wParam,LPARAM lParam);
	afx_msg void OnDeleteColumn();
	afx_msg void OnDeleteRow();
	afx_msg LRESULT OnClickColumn(WPARAM wParam,LPARAM lParam);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnComboxSelectChanged();
	DECLARE_MESSAGE_MAP()

private:
	CImageList* CreateDragImageEx(int nColIndexDragged,CListCtrl *pList, LPPOINT lpPoint);
		
	bool readTextFromFile( const CString& _strFile );
	void addTextToList(char chDelimiter, int& _col,int& _row );
	void initProcTree();
	void initListCtrl();
	void loadUserNameList();
	//////////////////////////////////////////////////////////////////////////
	int GetColNumFromStr(CString strText,CString strSep,CStringArray& strArray);
	void DeleteAllColumn();
	void AddColumn(int nColNum);
	void SetItemText(CStringArray& strArray);
	InputTerminal* GetInputTerminal();
	CString GetProjectPath();
	bool getColumnText( std::vector<CString>& _vItemString, int _iColumn );
	bool getEntireColumText( std::vector<CString>& _vItemString, int _iColumn );
	bool getSomeColumnText(  const CString& _strItem, std::vector<CString>& _vItemString, int _iColumn ,int _iCurSel);
	bool isNumString( const CString& _strNum );
	bool getUserNameAndNum( const CString& strItem, CString& strName, CString& strNum );
private:
	char GetDelimiter( );
	void dragProcToListCtrl();
	CString m_strFileName;
	CStringArray m_strLineArray;
	bool m_bHaveAssignTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NAMELISTMAPPINGDLG_H__3E343EE0_4C5E_450F_8BAE_9F88C9E75BE9__INCLUDED_)
