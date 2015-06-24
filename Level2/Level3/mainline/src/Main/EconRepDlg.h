#if !defined(AFX_ECONREPDLG_H__12450249_4C00_492D_9FF5_D6D761D4BA83__INCLUDED_)
#define AFX_ECONREPDLG_H__12450249_4C00_492D_9FF5_D6D761D4BA83__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EconRepDlg.h : header file
//


#include "EconListCtrl.h"
#include "..\economic\EconomicManager.h"
#include "TermPlanDoc.h"

typedef struct _EconItem
{
	CString m_csTitle;
	double  m_dVal;
	int m_nLevel;
	int m_nRev;
} EconItem;

/////////////////////////////////////////////////////////////////////////////
// CEconRepDlg dialog

class CEconRepDlg : public CDialog
{
// Construction
public:

	// Description: Constructor.
	// Exception:	FileOpenError, StringError, FileVersionTooNewError
	CEconRepDlg( CTermPlanDoc* _pDoc, CWnd* pParent = NULL );   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEconRepDlg)
	enum { IDD = IDD_DIALOG_ECONREP };
	CButton	m_btnClose;
	CButton	m_btnPrint;
	CEconListCtrl	m_listRep;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEconRepDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	double CalculateNPV( double _dRevCost, double _dRate, int _nProjLife );
	double CalculateIRR( double _dInitCost, double _dRevCost, int _nProjLife );

	int m_nAnnualFactor;
	
	CTermPlanDoc* m_pDoc;
	Terminal* m_pTerm;
	CProgressBar* m_pBar;


	void LoadDatabase();

	void PreLoadAnnualFactor();
	
	void LoadRevenue( int& _nNextAvailIdx, double& _dResVal, int _nType  );
	
	void LoadFixedRevenue( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadLeases( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadAdvertising( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadContracts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	
	void LoadVarRevenue( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );

	void LoadPaxAirportFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadLandingFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadGateUsage( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadProcessorUsage( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadRetailPercent( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadParkingFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadLandsideTransFee( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );

	void LoadExpenses( int& _nNextAvailIdx, double& _dResVal, int _nType  );
	
	void LoadFixedCosts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadBuildingDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadInterests( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadInsurance( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadContracedServ( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadParkingLotsDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadLandsideDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void AirsideDepre( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	

	void LoadVarCosts( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadProcessorCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadPassengerCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadAircraftCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadUtilitiesCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadLaborCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );
	void LoadMaintenanceCost( int _nLevel, int& _nNextAvailIdx, double& _dVal, int _nType );

	void LoadWaitingCost( int _nLevel, int& _nNextAvailIdx, double& _dResVal, int _nType  );
	void LoadAnnualFactor( int& _nNextAvailIdx, int _nType  );
	void LoadCapitalBudgetDecision( int& _nNextAvailIdx, double _dRevCost );


	CString GetLevelString( CString _str, int _nLevel );
	CString GetLevelString( double _dVal, int _nLevel );


	CEconomicManager* GetEconomicManager();

	InputTerminal* GetInputTerminal();


	// Generated message map functions
	//{{AFX_MSG(CEconRepDlg)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECONREPDLG_H__12450249_4C00_492D_9FF5_D6D761D4BA83__INCLUDED_)
