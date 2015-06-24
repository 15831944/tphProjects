#if !defined(AFX_PROBDISTDLG_H__3E0EC21B_99FF_4CB3_B6D6_B2A290B1D484__INCLUDED_)
#define AFX_PROBDISTDLG_H__3E0EC21B_99FF_4CB3_B6D6_B2A290B1D484__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProbDistDlg.h : header file
//

#include "inputs\con_db.h"
#include "..\MFCExControl\ListCtrlEx.h"
#include "..\inputs\in_term.h"

/////////////////////////////////////////////////////////////////////////////
// CProbDistDlg dialog


class CProbDistDlg : public CDialog
{
// Construction
public:
	CProbDistDlg( bool _bNeedReturn, CWnd* pParent = NULL );   // standard constructor

	virtual ~CProbDistDlg();


	int GetLastSelection(){ return m_nLastSelection; }

// Dialog Data
	//{{AFX_DATA(CProbDistDlg)
	enum { IDD = IDD_DIALOG_PROBDIST };
	CButton	m_staticGroup;
	CStatic	m_toolbarcontenter1;
	CStatic	m_staPic;
	CStatic	m_staticNote;
	CButton	m_btnCancel;
	CStatic	m_staticTotalVal;
	CEdit	m_editName;
	CButton	m_btnSave;
	CButton	m_btnDel;
	CStatic	m_staticTotal;
	CStatic	m_staticThirdVal;
	CStatic	m_staticSecondVal;
	CStatic	m_staticForthVal;
	CStatic	m_staticFirstVal;
	CListCtrlEx	m_listCtrlValue;
	CEdit	m_editThirdVal;
	CEdit	m_editSecondVal;
	CEdit	m_editForthVal;
	CEdit	m_editFirstVal;
	CListBox	m_listBoxName;
	CTreeCtrl	m_treeDist;
	double	m_dFirstVal;
	double	m_dForthVal;
	double	m_dSecondVal;
	double	m_dThirdVal;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProbDistDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	int GetSelectedItem();
	void InitToolBar();
	CToolBar m_ToolBar;
	bool m_bNeedReturn;

	int m_nLastSelection;

	bool CheckFullPercent();

	bool m_bEditChanged;
	void SetDistValue( ProbabilityDistribution* _pProbDist );


	// from the listctrol set the database.
	void SetHistDistDB();
	
	// Set the probdist tree label
	void SetTree();

	// init list control
	void SetListCtrl();

	// load glabal probdist db entry name list. use index as item data.
	void ReloadDatabase( int _nSelDBIdx );

	// base on the selection in the name list. refresh the contents.
	void ReloadContents();

	// return index in name list database
	int GetSelDBIndex();

	// return -1 for no selection
	int GetDistType();

	ProbabilityDistribution* GetDist( int _nIdx );





	
	
	CString GetProjPath();
	InputTerminal* GetInputTerminal();

	// Generated message map functions
	//{{AFX_MSG(CProbDistDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedTreeDistribution(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeListNamelist();
	afx_msg void OnButtonAdd();
	afx_msg void OnChangeEdit();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonSave();
	afx_msg void OnPeoplemoverDelete();
	afx_msg void OnPeoplemoverNew();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnKillfocusEditName();
	afx_msg void OnKillfocusEditVal();
	afx_msg void OnEndlabeleditListValue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnItemchangedListValue(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	ConstraintDatabase* m_pConDB;		// global database, donot delete it.
	enum PROBDISTCONSTTYPE m_enumType;	// dialog type
	int m_nIdxChanged;					// cur selected item changed.



//	void RefreshDistControl();
	
	bool m_bCodeSelTree;
	CImageList m_ilNodes;

	void ReCreateCurDistItem(int _nType, 	ProbabilityDistribution* _pCurDist );
	void WriteCurItem();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROBDISTDLG_H__3E0EC21B_99FF_4CB3_B6D6_B2A290B1D484__INCLUDED_)
