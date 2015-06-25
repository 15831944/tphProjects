#pragma once
#include "afxwin.h"

#include "InputAirside/VehicleSpecifications.h"
// CGSESelectDlg dialog

class AIRSIDEGUI_API CGSESelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CGSESelectDlg)

public:
	CGSESelectDlg(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CGSESelectDlg();

	int GetSelGSEID() const { return m_nSelGSEID; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	afx_msg void OnLbnDblclkListGSE();

	DECLARE_MESSAGE_MAP()

private:
	CListBox m_listGSE;

	CVehicleSpecifications m_vehicleSpecifications;
	int m_nSelGSEID;
};
