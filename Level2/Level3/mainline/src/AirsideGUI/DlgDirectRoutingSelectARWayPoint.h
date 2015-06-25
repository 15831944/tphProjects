#pragma once
#include "..\InputAirside\DirectRouting.h"
#include "afxcmn.h"
#include "Resource.h"
// CDlgDirectRoutingSelectARWayPoint dialog

class CDlgDirectRoutingSelectARWayPoint : public CDialog
{
	DECLARE_DYNAMIC(CDlgDirectRoutingSelectARWayPoint)

public:
	CDlgDirectRoutingSelectARWayPoint(int nProjID, CDirectRouting* pDirectRouting, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgDirectRoutingSelectARWayPoint();

// Dialog Data
	enum { IDD = IDD_DIALOG_DIRECTROUTINGSELECTWAYPOINT };
private:
	int m_nProjID;
	CDirectRouting* m_pDirectRouting;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_wndWayPointStartTree;
	CTreeCtrl m_wndWayPointEndTree;
	CTreeCtrl m_wndWayPointMaxTree;
	void SetWayPointStartTree();
	void SetWayPointEndTree();
	void SetWayPointMaxTree();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeWaypointstart(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeWaypointend(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeWaypointmax(NMHDR *pNMHDR, LRESULT *pResult);

public:
	afx_msg void OnBnClickedOk();
};
