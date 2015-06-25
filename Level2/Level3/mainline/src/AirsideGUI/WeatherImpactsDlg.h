#pragma once
#include "NodeViewDbClickHandler.h"
#include "MFCExControl/ListCtrlEx.h"
#include "MFCExControl/XTResizeDialog.h"

// CWeatherImpactsDlg dialog
class CWeatherImpact;
class CWeatherImpactList;
enum WEATHERCONDITION;

class AIRSIDEGUI_API CWeatherImpactsDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CWeatherImpactsDlg)

public:
	CWeatherImpactsDlg(int nProjID, PSelectFlightType pSelectFlightType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CWeatherImpactsDlg();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();    
	DECLARE_MESSAGE_MAP()

private:
	void InitToolBar( void );
	void InitWeatherConditionTree(void);
	void InitListCtrl(void);
	void SetListColumn(void);
	void SetListContent(void);
	void UpdateToolBarState(void);

private:
	CToolBar m_ToolBar;
	CTreeCtrl m_WeatherConditionTreeCtrl;
	CListCtrlEx	m_ListCtrl;

	CWeatherImpactList *m_WeatherImpactList;
	CWeatherImpact     *m_WeatherImpact;
	PSelectFlightType	m_pSelectFlightType;
	WEATHERCONDITION    m_WeatherCondition;
	int                 m_nProjID;
	
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnTvnSelchangedTreeWeathercondition(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMSetfocusListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMKillfocusListWeatherimpacts(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedCancel();
};
