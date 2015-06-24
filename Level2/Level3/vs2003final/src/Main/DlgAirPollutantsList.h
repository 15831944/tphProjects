#pragma once
#include "../MFCExControl/ListCtrlEx.h"
#include "..\InputEnviroment\AirPollutantList.h"

// CDlgAirPollutantsList dialog

class CDlgAirPollutantsList : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAirPollutantsList)

public:
	CDlgAirPollutantsList(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirPollutantsList();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnNewAirPollutant();
	void OnDelAirPollutant();
	void ResetAirPollutantList();


// Dialog Data
	enum { IDD = IDD_ENVIRO_AIRPOLLUTANTS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	void InitToolBar();
	void InitAirPollutantList();
	


	DECLARE_MESSAGE_MAP()
private:
	CAirPollutantList m_vAirPollutantList;
	CToolBar m_toolBarAirPollutant;
	CListCtrlEx m_listAirPollutant;
public:
	afx_msg void OnBnClickedDefault();
	afx_msg void OnLvnItemchangedListAirpollutants(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListAirpollutants(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
