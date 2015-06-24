#pragma once
#include "afxwin.h"


// DlgOnboardFlightSelection dialog
class InputTerminal;
class CFlightTypeCandidate;
class Flight;
class DlgOnboardFlightSelection : public CDialog
{
	DECLARE_DYNAMIC(DlgOnboardFlightSelection)

public:
	DlgOnboardFlightSelection(InputTerminal* pTerminal, CFlightTypeCandidate* fltTypeCandidate, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgOnboardFlightSelection();

// Dialog Data
	enum { IDD = IDD_DLGONBOARDFLIGHTSELECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

	CString m_strFlight;
	Flight* m_pSelFlight;
private:
	CListBox m_lFlightList;
	InputTerminal* m_pTerminal;
	CFlightTypeCandidate* m_pFltTypeCandidate;

};	
