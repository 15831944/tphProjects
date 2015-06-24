#pragma once

class InputTerminal;

class CStopOverAirportDialog : public CDialog
{
public:
	CStopOverAirportDialog(InputTerminal* pInputTerminal, CWnd* pParent = NULL);   // standard constructor
	virtual ~CStopOverAirportDialog();

	// return the selected airport, return empty string if the current mode is sector
	LPCTSTR GetSelectedAirport() { return m_strAirport;	}

	// return the selected sector, return empty string if the current mode is airport
	LPCTSTR GetSelectedSector() { return m_strSector; }

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void SetAirportContents();
	void SetSectorContents();

	InputTerminal* m_pInputTerminal;
	CString			m_strAirport;
	CString			m_strSector;
	CListBox m_wndAirpotSectorList;

	afx_msg void OnRadioAirport();
	afx_msg void OnRadioSector();
	afx_msg void OnSelchangeAirportSectorList();
};
