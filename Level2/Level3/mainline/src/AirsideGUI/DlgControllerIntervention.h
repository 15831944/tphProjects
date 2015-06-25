#pragma once
//////////////////////////////////////////////////////////////////////////
//Author : cjchen
//DATE : 4/20/2008
//Class : CControllerIntervention 
//for create Controller Intervention Dialog
//////////////////////////////////////////////////////////////////////////
#include "ControllerInterventionData.h"
// CDlgControllerSocter dialog
#define  FLIGHT_CHECKBOX  0 
#define  CONFLICT_CHECKBOX 1 
#define  DISTANCE_CHECKBOX 2 
#define  MAINTENACE_CHECKBOX 3 
#define  DEPARTURE_CHECKBOX 4 
class CControllerIntervention : public CDialog
{
	DECLARE_DYNCREATE(CControllerIntervention)

public:
	CControllerIntervention(CWnd* pParent = NULL);   // standard constructor
	CControllerIntervention(int projId,CWnd* pParent = NULL);
	virtual ~CControllerIntervention();

	BOOL    OnSave();
	// Dialog Data
	enum { IDD = IDD_DIALOG_INTERVENTION_SECTOR};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
protected:
	BOOL InitTree();
	BOOL InitHoldPriList() ;
	BOOL InitPriorityList();
	BOOL InitFIxList() ;
	DECLARE_MESSAGE_MAP()
	//DECLARE_DHTML_EVENT_MAP()
public:
	HTREEITEM AddTreeItem(CControllerInterventionData::P_SECTOR_TYPE psector);
	BOOL DelTreeItem();
private:
	CControllerInterventionData  ControllerData ;
public:
	afx_msg void OnTvnSelchangedTreeSector(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CButton flightCheckBox;
	CButton conflictCheckbox;
	CButton DistanceCheckbox;
	CButton maintenaceCheckbox;
	CButton departurecheckbox;
	CTreeCtrl sectortree;
	CStatic sector_name ;
	CEdit sector_radius;
	CComboBox Holdpri_list;
	CComboBox pri1_list;
	CComboBox pri2_list;
	CComboBox pri3_list;
	CComboBox pri4_list;
	CComboBox fix_list ;
protected:
	void updateMessage(HTREEITEM tree_Item) ;
	void setCheckBoxGroup(CSectorData* data) ;
	void setRadiusOfConcern(CSectorData* data) ;
	void setEditHoldPriotity(CSectorData* data) ;
	void setPriority(CSectorData* data) ;
	BOOL CheckPoliy() ;
protected:
	BOOL SaveSector(CControllerInterventionData::P_SECTOR_TYPE psector);
	CControllerInterventionData::P_SECTOR_TYPE getCurSelectedItemData() ;
public:
	afx_msg void OnBnClickedButtonCreate();
	afx_msg void OnBnClickedButtonApply();

private:
	void SetSectorNameEditTodefault() ;
	void setCheckboxesTodefault() ;
	void setRadiusEditToDefault() ;
	void setPriorityListsToDefault() ;
	void SetComboToNoselect(CComboBox& box) ;
	void  setFixID(CSectorData* data) ;
	HTREEITEM FindItemFromTree(const TCHAR* item_name) ;
	void setAllDlgItemsStart(BOOL bEnable) ;
public:
	afx_msg void OnCbnSelchangeComboPririty();
	afx_msg void OnCbnSelchangeComboPririty2();
	afx_msg void OnCbnSelchangeComboPririty3();
	afx_msg void OnCbnSelchangeComboPririty4();
	afx_msg void OnTvnSelchangingTreeSector(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnCbnSelchangeComboHoldpriority();

protected:

	CStatic STATIC_FIX;
private:
	int projID ;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();

	//DECLARE_MESSAGE_MAP()
};
