#pragma once

#include "NodeViewDbClickHandler.h"
#include "MFCExControl/ListCtrlEx.h"
#include "../MFCExControl/XTResizeDialog.h"

// CDlgPadAissgnment dialog

class ALTAirport;
class ALTObject;

class CDlgPadAissgnment : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgPadAissgnment)

public:
	CDlgPadAissgnment(int nProjID,PSelectFlightType pSelectFlightType,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgPadAissgnment();
	CTreeCtrl m_wndTreeCtrl;
	CListCtrlEx m_wndListCtrl;
	CListBox  m_wndListBox;
	CToolBar  m_wndFlighTypeToolbar;
	CToolBar  m_wndPadToolbar;
	int m_nProjID;
	WeatherListInfo* m_pWeatherList;
	WEATHERCONDITION m_emWeather;
	WeatherInfo* m_pCurWeather;
	PSelectFlightType	m_pSelectFlightType;
	FlightTypeInfo* m_pCurFlightType;
	PadInfo*		m_pPadInfo;
	BOOL			m_bEnable;
	ALTObjectList m_vPadList;


// Dialog Data
	enum { IDD = IDD_DIALOG_PADCONDITION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()

public:
	void InitWeatherConditionTree();
	void OnInitListCtrl();
	void OnInitToolbar();
	void OnInitPadValue();
	void DisplayFlightType(WEATHERCONDITION emWeather);
	void DisplayPadInfo(CString& strFlightType);
	void OnInitFlightTypeToolbarStatus();
	void OnInitPadToolbarStatus();

	afx_msg void OnPadItemAdd();
	afx_msg void OnPadItemDel();
	afx_msg void OnFlightTypeAdd();
	afx_msg void OnFlightTypeDel();
	afx_msg void OnFlightTypeEdit();
	afx_msg void OnTvnSelchangedTreeWeather(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnLbnSelchangeListFlighttype();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnItemchangedListPad(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedRadioGate();
	afx_msg void OnBnClickedRadioDeice();
	afx_msg void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
};
