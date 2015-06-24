#pragma once
#include "..\MFCExControl\CoolTree.h"
#include "ResizePS\ResizablePage.h"


// CFacilityDataPage dialog
class InputLandside;
class CFacilityBehaviorList;
class CFacilityTaxiPoolBehavior;
class InputTerminal;

class CTaxiPoolFacilityDataPage : public CResizablePage
{
	DECLARE_DYNAMIC(CTaxiPoolFacilityDataPage)

public:
	CTaxiPoolFacilityDataPage(InputLandside *pInputLandside,
		InputTerminal* _pInTerm,
		CFacilityBehaviorList *pBehaviorDB,
		enum ALTOBJECT_TYPE enumFacType,
		UINT nIDCaption,
		int nProjectID);

	virtual ~CTaxiPoolFacilityDataPage();

	// Dialog Data
	enum { IDD = IDD_PROPPAGE_FACILITYDATA };
protected:
	enum EntryNodeType
	{
		EntryNodeType_Offset = 0,
		EntryNodeType_Delay,
	};

protected:
	ALTOBJECT_TYPE	m_enumType;
	int				m_nProjectID;

protected:
	CCoolTree	m_TreeData;
	CListBox	m_listboxFacility;

	CToolBar m_ToolBar;
	CToolBar m_ToolBarLeft;


	InputLandside *m_pInputLandside;
	InputTerminal *m_pInTerm;
	CFacilityBehaviorList *m_pTaxiPoolBehaviorDB;
protected:
	void InitToolbar();	
	void InitSpiltter();
	void EvenPercent();

	void InitTaxiPoolFacilityList();
	int AddTaxiPoolFacilityToList(CFacilityTaxiPoolBehavior *pBehavior);

	void UpdateFacilityToolbarState();

	void LoadTreeProperties();
	void UpdateTreeToolbarState();
	CFacilityTaxiPoolBehavior* GetCurrentTaxiPoolBehavior();

	//check the item in the existed in the list
	bool IsItemDuplicate(HTREEITEM hParentItem,const CString& strName);
protected:
	HTREEITEM m_hVehicleType;
	HTREEITEM m_hRecordLevel;
	HTREEITEM m_hRecordQuantity;
	HTREEITEM m_hCallFromExternal;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	virtual afx_msg void OnToolbarbuttonadd();
	virtual afx_msg void OnToolbarbuttondel();


	virtual afx_msg void OnSelchangeListFacility();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
