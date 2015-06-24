#pragma once
//#include "e:\arcportmianline\src\mfcexcontrol\listctrlex.h"
#include "..\Main\FuelsPropertiesList.h"
#include "..\InputEnviroment\AirPollutantList.h"
class FuelProperties;

// CDlgFuelsProperties dialog

class CDlgFuelsProperties : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgFuelsProperties)

public:
	CDlgFuelsProperties(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgFuelsProperties();

// Dialog Data
	enum { IDD = IDD_DIALOG_ENV_FUELSPROPERTIES };


protected:
	FuelProperties *m_pFuelProperties;
	CAirPollutantList *m_pAirPollutantList;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void InitToolbar();
	void InitList();

	void ReloadList();
protected:
	CToolBar m_toolBar;
	CFuelsPropertiesList m_listFuelsProperties;
public:
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedButtonSave();

	afx_msg void OnNMClickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHdnItemclickListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnColumnclickListData(NMHDR *pNMHDR, LRESULT *pResult);

	void OnNewFuelsProperty();
	void OnDelFuelsProperty();
	afx_msg void OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
