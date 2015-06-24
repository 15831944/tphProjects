#pragma once
#include "afxwin.h"


// CDlgSelectAirRoute dialog
class AirRouteList;
class CAirRoute;
class CDlgReportSelectAirRoute : public CDialog
{
	DECLARE_DYNAMIC(CDlgReportSelectAirRoute)

public:
	CDlgReportSelectAirRoute(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReportSelectAirRoute();

// Dialog Data
	//enum { IDD = IDD_DIALOG_REPORTSELECTAIRROUTE };

	std::vector<CAirRoute *> m_vAirARouteSel;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual void OnOK();
	virtual void OnCancel();

	int m_nProjID;

	//all airroute
	std::vector<AirRouteList *> m_vAirRouteList;
public:
	virtual BOOL OnInitDialog();
	CListBox m_lbAirRoute;
};
