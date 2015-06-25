#pragma once
#include "NodeViewDbClickHandler.h"
#include "MFCExControl/ListCtrlEx.h"
#include "..\InputAirside\ItinerantFlight.h"
#include "..\InputAirside\InputAirside.h"
#include "afxwin.h"
#include <iostream>
#include "../MFCExControl/XTResizeDialog.h"

class CServicingRequirement;
class CFlightServicingRequirement;
class CFlightServicingRequirementList;
class CVehicleSpecifications;
// CServiceRequirementDlg dialog

class CServiceRequirementDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CServiceRequirementDlg)

public:
	CServiceRequirementDlg(int nProjID, PSelectFlightType pSelectFlightType,
		InputAirside* pInputAirside,  PSelectProbDistEntry pSelectProbDistEntry,
		CAirportDatabase* pAirportDB, CWnd* pParent = NULL);

	virtual ~CServiceRequirementDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_SERVICEREQUIREMENT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnCmdNewFltType();
	afx_msg void OnCmdDeleteFltType();
	afx_msg void OnCmdEditFltType();
	afx_msg void OnLvnEndlabeleditListVehicleNum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListVehicleNum(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLbnSelchangeListFlightType();
	afx_msg LRESULT OnMsgComboChange(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBtnServiceLocationSpecification();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();



private:
	void InitToolBar(void);
	void InitListCtrl(void);
	//void InitConditionCol(enumVehicleBaseType enumBaseType, enumVehicleUnit enumUnit, CServicingRequirement* pServicingRequirement);
	//if is new item set bIsNewItem true,else set false
	void SetListContent(bool bIsNewItem);
	void InitServiceTimeList(void);
	void SetFlightType(void);

private:
	CToolBar        m_ToolBarFltType;
	CListBox        m_ListBoxFltType;
	CListCtrlEx	    m_ListFltTypeServiceRequirement;


	int                               m_nProj;
	CStringList                       m_ServiceTimeList;
	CStringList                       m_ConditionNameList;     //condition type list

	PSelectFlightType                 m_pSelectFlightType;
	CAirportDatabase*                 m_pAirportDB;
	CServicingRequirement             *m_pServicingRequirement;
	CFlightServicingRequirement       *m_pFlightServicingRequirement;
	CFlightServicingRequirementList   *m_pFltServicingRequirementList;
	CVehicleSpecifications            *m_pVehicleSpecifications;
	InputAirside                      *m_pInputAirside;
	PSelectProbDistEntry              m_pSelectProbDistEntry;
	map<int, CString>                 m_ServiceTimeMap;
};
