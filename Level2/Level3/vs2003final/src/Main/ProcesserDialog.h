#if !defined(AFX_PROCESSERDIALOG_H__79B29535_CEE6_4DB0_BA0C_D096B4553236__INCLUDED_)
#define AFX_PROCESSERDIALOG_H__79B29535_CEE6_4DB0_BA0C_D096B4553236__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcesserDialog.h : header file
//

#include <CommonData/procid.h>
#include "..\inputs\in_term.h"
#include "muliseltreectrlex.h"

enum PROCFILTERFLAG
{
	FilterByProcType,
	FilterByProcTypeList
};
/////////////////////////////////////////////////////////////////////////////
// CProcesserDialog dialog
class ProcessorIDList;
class CProcesserDialog : public CDialog
{
// Construction
public:
	CProcesserDialog( InputTerminal* _pInTerm, CWnd* pParent=NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProcesserDialog)
	enum { IDD = IDD_DIALOG_PROCESSER };
	CButton	m_btCancle;
	CButton	m_btOk;
	CEdit	m_editResult;
	CListBox	m_listProcess;
	CMuliSeltTreeCtrlEx	m_ProcTree;
	CString	m_strProcID;
	//}}AFX_DATA 

public:
	bool GetProcessorIDList(ProcessorIDList& _IDlist );
	void SetType( int _nProcType );
	void SetType2( int _nProcType );
	void LoadProcType2Proc();
	void SetTypeList( const std::vector<int>& _nProcTypeList );
	bool GetProcessorID( ProcessorID& _ID );

	void SetDisplayBaggageDeviceFlag( bool _bFlag ){ m_bDisplayBaggageDeviceSign = _bFlag;	};
	void SetDisplayProcess( bool _bFlag ){ m_bDisplayProcess = _bFlag;	};
	void SetOnlyShowACStandGate( bool _bFlag ) { m_bOnlyShowACStandGate = _bFlag;	}
	void SetOnlyShowPusherConveyor(bool _bFlag){ m_bOnlyShowPusherConveyor = _bFlag; }
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcesserDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
private:
	bool CheckACStandFlag( const CString& procIDString);
	bool CheckPusherConveyor(const CString& procIDString);
// Implementation
protected:
	int m_nProcType;
	int m_nProcType2;
	InputTerminal* m_pInTerm;
	std::vector<ProcessorID> m_vectID;
	int m_nCurSel;
	std::vector< int > m_vectSel;


	bool m_bDisplayBaggageDeviceSign;

	bool m_bDisplayProcess;

	bool m_bSelectProcessor;
	CString m_sSelectedProcess;
	
	// if the flag is true, then only display ACStand Gate
	// else display all Gate Processor
	bool m_bOnlyShowACStandGate;
	bool m_bOnlyShowPusherConveyor;

	PROCFILTERFLAG m_iFilterFlag;
	std::vector<int> m_vProcType;
	// Generated message map functions
	//{{AFX_MSG(CProcesserDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnSelect();
	afx_msg void OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeProcessList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSERDIALOG_H__79B29535_CEE6_4DB0_BA0C_D096B4553236__INCLUDED_)
