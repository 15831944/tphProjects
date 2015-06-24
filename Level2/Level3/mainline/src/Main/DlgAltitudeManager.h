#pragma once


// CDlgAltitudeManager dialog

class CDlgAltitudeManager : public CDialog
{
	DECLARE_DYNAMIC(CDlgAltitudeManager)

public:
	CDlgAltitudeManager(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAltitudeManager();

// Dialog Data
	enum { IDD = IDD_DIALOG_ALTMANAGER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
