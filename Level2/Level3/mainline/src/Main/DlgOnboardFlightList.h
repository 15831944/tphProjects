#pragma once
#include "../Engine/terminal.h"
#include "../InputOnboard/onboardanalysiscondidates.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgOnboardFlightList dialog

class CDlgOnboardFlightList : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgOnboardFlightList)

public:
	enum FlightListType
	{
		ALL_FLIGHT_LIST=-1,
		ARR_FLIGHT_LIST=ArrFlight,
		DEP_FLIGHT_LIST=DepFlight
	};
	CDlgOnboardFlightList(Terminal* _pTerm,FlightListType eFlightListType=ALL_FLIGHT_LIST,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgOnboardFlightList();
	CString FormatFlightIdNodeForCandidate(const CString& _configName,COnboardFlight* pFlight);
	COnboardFlight* getOnboardFlight();
	CString getFlightString();


// Dialog Data
	enum { IDD = IDD_ONBOARD_FLIGHTLIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void OnInitFlightList();
	
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelChangeFlightList();


	DECLARE_MESSAGE_MAP()
private:
	CListBox m_listboxFlight;
	Terminal *m_pTerminal;
	COnboardFlight* m_pOnboardFlight;
	int m_nCurSel;
	CString m_strFlight;

	FlightListType m_eFlightListType;

};
