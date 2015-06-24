#pragma once


class CDlgLaneRange : public CDialog
{
public:
	CDlgLaneRange(int nstartLane, int nendLane, CWnd* pParent = NULL);
	virtual ~CDlgLaneRange();

	enum { IDD = IDD_DIALOG_LANERANGE };
	
protected:

	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();	
	void UpdateDataEx();

	afx_msg void OnDeltaposSpinStartLane(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinEndLane(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCheckAlll();

protected:

	int m_nStartLane;
	int m_nEndLane;
	BOOL m_bAll;
	CSpinButtonCtrl m_spinStartLane; 
	CSpinButtonCtrl m_spinEndLane;

public:

	int GetStartLane() ;
	int GetEndLane() ;
	BOOL IsAll();
};
