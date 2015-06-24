#if !defined(AFX_IMPORTFLIGHTFROMFILEDLG_H__D5E37763_4938_47C6_BAE9_CC1F0BC7E712__INCLUDED_)
#define AFX_IMPORTFLIGHTFROMFILEDLG_H__D5E37763_4938_47C6_BAE9_CC1F0BC7E712__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ImportFlightFromFileDlg.h : header file
//
class InputTerminal;
class Flight;
#define COLUMN_NUM 24
/////////////////////////////////////////////////////////////////////////////
// CImportFlightFromFileDlg dialog
#include <vector>
#include "..\MFCExControl\ListCtrlEx.h"
#include "HeaderCtrlDrag.h"	// Added by ClassView
#include "XPSTYLE\ComboBoxXP.h"	// Added by ClassView
#include "../Common/StartDate.h"
#include "CoolBtn.h"
#include "afxwin.h"


#ifndef ID_COMBOBOX
#define ID_COMBOBOX 100
#endif
class CImportFlightFromFileDlg : public CDialog
{
// Construction
public:
	BOOL IsFlightScheduleDlg();
	void SetFlightScheduleDlg(BOOL bValue);
	CString m_strResultInfo;
	void EnableCompileBtn();
	int m_nColIndexClicked;
	CHeaderCtrlDrag m_headerCtrlDrag;
	CToolBar m_wndToolBarLeft;
	BOOL m_bColumnDropped[COLUMN_NUM];
	
	CWnd* m_pDragWnd;
	CImageList* m_pDragImage;
	CImageList m_ImageList;
	HCURSOR m_hcArrow;
	HCURSOR m_hcNo;
	void AddColumn(int nColNum,BOOL bAdd2ListBox=TRUE);
	void SetItemText(CStringArray& strArray);
	void DeleteAllColumn();
	int GetColNumFromStr(CString strText,CString strSep,CStringArray& strArray);
	int GetColNumFromStr2_9(CString strText,CString strSep,CStringArray& strArray);   //add by adam 2007-08-24
	CImportFlightFromFileDlg(InputTerminal* _pInTerm,const CString& _strPath,int nPrjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CImportFlightFromFileDlg();

	void Openfile();
// Dialog Data
	//{{AFX_DATA(CImportFlightFromFileDlg)
	enum { IDD = IDD_DIALOG_IMPORTFLIGHT_FROMFILE };
	CCoolBtn	m_btnOpen;
	CButton	m_btnAppend;
	CComboBoxXP	m_cmbImportedFileFormat;
	CEdit	m_editResultInfo;
	CButton	m_btnCompile;
	CButton	m_btnMoveToLeft;
	CButton	m_btnMoveToRight;
	CListCtrl	m_listMapping;
	CListBox	m_listSourceColumn;
	CEdit	m_editFileName;
	CListCtrlEx		m_listSource;
	CListCtrl		m_listResult;
	CButton	m_butOK;
	BOOL	m_bFullCompatibleFormat;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImportFlightFromFileDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bIsFlightScheduleDlg;
	CStartDate m_startDate;
	
	
	// Generated message map functions
	//{{AFX_MSG(CImportFlightFromFileDlg)
	//afx_msg void OnButConvert();
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnItemchangedListSource(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonTitle();
	afx_msg void OnDeleteRow();
	afx_msg void OnDeleteColumn();
	afx_msg void OnRclickListResult(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeListSourceColumn();
	afx_msg void OnClickListMapping(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnButtonMovetoRight();
	afx_msg void OnButtonMovetoLeft();
	afx_msg void OnCheckFullCompatibleFormat();
	afx_msg void OnCompile();
	afx_msg void OnSelchangeComboImportedFileFormat();
	afx_msg void OnAppend();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnHeaderCtrlClickColumn(WPARAM wParam,LPARAM lParam);
	afx_msg void OnComboxSelectChanged();
	afx_msg void OnOpenFromFile();
	afx_msg void OnOpenFromSQL();
	afx_msg void OnOpenFromArcCapture();
	
	void OnOpenClick();

	DECLARE_MESSAGE_MAP()
private:
	void GetMinStartDate(CStartDate& minStartDate);  //add by adam 2007-05-25
	int GetStringArrayFromEdit();
	void ClearFlightVect();
	int AddAFlightToList(Flight* _pFlight);
	void AddExtendInfoToList( int _sourceIdx, int _destIdx );
	void InitListCtrl();
	int AddFlightToEdit();
	bool ReadFile( const CString& strFileName );
	void getMoreInfoFromList( Flight* _pFlight,int _iItemIdx);
	bool isNumString( const CString& _strNum );
	bool isProcessor( const CString& _strProcName );
	CStringArray flightSchedule;
	BOOL	m_bArctermFile;
	InputTerminal* m_pInTerm;
	CString m_strProjPath;
	std::vector<Flight*> m_vectFlight;
	bool m_bHaveAssignTitle;

	//add by adam 2007-05-11
	CStartDate m_ConvertStartDate;  // used as the first day
	//End add by adam 2007-05-11

	//add by adam 2007-08-24
	double m_dFileVersion;       //indicate the flight schedule file version
	//End add by adam 2007-08-24

	int m_nPrjID;
	
protected:
	CComboBoxXP m_DataFormatControl;
	int GetCurrentListColumnIndex(CString& colum_name);
	BOOL CheckTheFlightActype() ;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMPORTFLIGHTFROMFILEDLG_H__D5E37763_4938_47C6_BAE9_CC1F0BC7E712__INCLUDED_)
