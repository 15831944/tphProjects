#pragma once
#include "afxcmn.h"
#include "../Common/NewPassengerType.h"

// CDlgDeplanementSequenceDefine dialog
class CDeplanementSequence ;
class COnBoardAnalysisCandidates ;
class COnboardFlight ;
class CDeck ;
class InputTerminal ;
class CDlgDeplanementSequenceDefine : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgDeplanementSequenceDefine)

public:
	CDlgDeplanementSequenceDefine(CDeplanementSequence* _seq ,InputTerminal* _input ,COnBoardAnalysisCandidates* _POnblardList,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeplanementSequenceDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_DEPLANE_DEFINE };
protected:
    COnBoardAnalysisCandidates* m_POnboardList ;
	CDeplanementSequence* m_Deplanement ;
	InputTerminal* m_input ;
	   CPassengerType paxty ;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTvnSelchangedTreeFlight(NMHDR *pNMHDR, LRESULT *pResult);

	virtual BOOL OnInitDialog() ;

protected:
	CTreeCtrl m_treeFlight;

	CEdit m_PaxTyEdit ;
	CComboBox m_ComboCabin ;
	CButton m_Select ;

protected:
	CString FormatFlightIdNodeForCandidate(const CString& _configName,int _nodeType,const CString& _Actype,CString FlightID) ;
protected:
	void InitTree() ;
	void AddItemFlightItem(COnboardFlight* _Analysis ,HTREEITEM _item) ;
	void AddDeckItem(CDeck* _deck , HTREEITEM _item) ;

	void OnOK() ;
	void OnCancel() ;

	void InitComboBox() ;
	void OnNewPassenger() ;
};
class CDlgDeplanementSequenceEdit : public CDlgDeplanementSequenceDefine
{
public:
	CDlgDeplanementSequenceEdit(CDeplanementSequence* _seq ,InputTerminal* _input ,COnBoardAnalysisCandidates* _POnblardList ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDeplanementSequenceEdit();
public:
    BOOL OnInitDialog() ;
};
