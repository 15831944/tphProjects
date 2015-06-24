//{{AFX_INCLUDES()
#include "gtchart.h"
//}}AFX_INCLUDES
#if !defined(AFX_ASSIGNFLIGHTDLG_H__14DEEEAB_7071_4F93_9546_BAC6DFEF31CA__INCLUDED_)
#define AFX_ASSIGNFLIGHTDLG_H__14DEEEAB_7071_4F93_9546_BAC6DFEF31CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AssignFlightDlg.h : header file
//
#include "..\MFCExControl\ListCtrlEx.h"
#include "inputs\IN_term.h"
#include "inputs\ProcessorSwitchTimeDB.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// CAssignFlightDlg dialog

class CAssignFlightDlg : public CDialog
{
// Construction
public:
	CAssignFlightDlg(InputTerminal* _terminal,CWnd* pParent,CString& _strProcName);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAssignFlightDlg)
	enum { IDD = IDD_DIALOG_ASSIGNFLIGHT_TO_PRO };
	CListCtrl	m_listAll;
	CButton	m_butApply;
	CButton	m_butSave;
	CGTChart	m_gtCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAssignFlightDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAssignFlightDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSave();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBegindragListAll(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnClickBtnAddLineGt();
	afx_msg void OnClickBtnDelLineGt(BOOL bSuccess, long nLineIndex, LPCTSTR strCaption, LPCTSTR strAddition, long nLineData);
	afx_msg void OnItemMovedGtchartctrl(long nLineIndexFrom, long nLineIndexTo, long nItemIndex, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnItemResizedGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration);
	afx_msg void OnDblClickItemGtchartctrl(long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnClickBtnDelItemGtchartctrl(BOOL bSuccess, long nIndexLine, long nIndexItem, LPCTSTR strCaption, LPCTSTR strAddition, long timeBegin, long timeDuration, long itemData);
	afx_msg void OnClickBtnAddItemGtchartctrl(BOOL bSuccess, long nLineIndex);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	std::vector<ProSwitchTime> m_switchlist;
private:
	InputTerminal* p_InTerm;
	CImageList* m_pDragImage;
	CImageList m_imageList;
	int m_nDragIndex;
	bool m_bDragging;

private:
	void UpdateGTLine( long _lLineIdx );
	void RebuildGt();
	void AddLineToGT( ProSwitchTime& pst,int lineData );
	bool AddItemFromLib();
	bool IsExistInGT( );
	bool IsExistInGT( ProSwitchTime& pst);
	void InitGtCtrl();
	CString strProcGroupName;
	CString GetProjectPath();
	void SetListData();
	void InitListCtrl();
private:
	std::vector<CString> m_proNamesList ;
	int m_cout_pro ;
protected:
	void initProcessorName(CString& groupName) ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ASSIGNFLIGHTDLG_H__14DEEEAB_7071_4F93_9546_BAC6DFEF31CA__INCLUDED_)
