#pragma once
#include "afxwin.h"


// CDlgTowingRouteFromAndTo dialog

class CDlgTowingRouteFromAndTo : public CDialog
{
	DECLARE_DYNAMIC(CDlgTowingRouteFromAndTo)

public:
	CDlgTowingRouteFromAndTo(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTowingRouteFromAndTo();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOWINGROUTE_FROMANDTO };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	int m_nProjID;
	CString m_strFrom;
	CString m_strTo;
	CButton m_buttonFrom;
	CButton m_buttonTo;
	int m_nFromID;
	int m_nToID;
	int m_nFromFlag;
	int m_nToFlag;
public:
	CString GetFromName(){return m_strFrom;}
	CString GetToName(){return m_strTo;}
	int GetFromID(){return m_nFromID;}
	int GetToID(){return m_nToID;}
	int GetFromFlag(){return m_nFromFlag;}
	int GetToFlag(){return m_nToFlag;}
	afx_msg void OnBnClickedButtonFrom();
	afx_msg void OnBnClickedButtonTo();
};
