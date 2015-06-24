#pragma once
#include "..\MFCExControl\CoolTree.h"
#include "ResizePS\ResizablePage.h"
#include "MFCExControl/SplitterControl.h"
#include "Landside\FacilityTaxiQBehavior.h"

// CFacilityDataPage dialog
class InputLandside;
class InputTerminal;

class CTaxiQFacilityDataPage : public CResizablePage
{
	DECLARE_DYNAMIC(CTaxiQFacilityDataPage)
	class TaxiQTreeNodeData
	{
	public:
		enum TaxiQTreeNodeType
		{
			Linkage_Node,
			Teminal_Node,
			TeminalLinkage_Node,
			ParkingLot_Node,
			ParkingLotLinkage_Node,
			VehicleType_Node,
			Source_Node,
			HavePool_Node,
			PoolRule_Node,
			FiFo_Node,
			LongQueue_Node,
			Dedicted_Node,
			NoPool_Node,
			External_Node,
			Entry_Node,
			Offset_Node,
			Delay_Node
		};

		TaxiQTreeNodeData()
		{

		}
		~TaxiQTreeNodeData()
		{

		}

		DWORD m_dwData;
		TaxiQTreeNodeType m_emType;
	};

public:
	CTaxiQFacilityDataPage(InputLandside *pInputLandside,
		InputTerminal* _pInTerm,
		CFacilityBehaviorList *pBehaviorDB,
		enum ALTOBJECT_TYPE enumFacType,
		UINT nIDCaption,
		int nProjectID);

	virtual ~CTaxiQFacilityDataPage();

	// Dialog Data
	enum { IDD = IDD_PROPPAGE_FACILITYDATA };

protected:
	ALTOBJECT_TYPE	m_enumType;
	int				m_nProjectID;
protected:
	HTREEITEM m_hCapacity;
	HTREEITEM m_hServeTaxi;
protected:
	CStatic		m_staticLinkage;
	CCoolTree	m_TreeData;
	CListBox	m_listboxFacility;

	CToolBar m_ToolBar;
	CToolBar m_ToolBarLeft;
	CSplitterControl m_wndSplitter;

	InputLandside *m_pInputLandside;
	InputTerminal *m_pInTerm;
	CFacilityBehaviorList *m_pTaxiQBehaviorDB;
protected:
	void InitToolbar();	
	void InitSpiltter();
	void SetSplitterRange();


	void InitFacilityList();
	int AddFacilityToList(CFacilityTaxiQBehavior *pBehavior);

	void UpdateFacilityToolbarState();

	void LoadTreeProperties();
	void UpdateTreeToolbarState();
	void SelectLevelItem(HTREEITEM hItem);
	CFacilityTaxiQBehavior *GetCurrentTaxiQBehavior();

	//check the item in the existed in the list
	bool IsItemDuplicate(HTREEITEM hParentItem,const CString& strName);
	void EvenPercent();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg void OnButtonAdd();
	afx_msg void OnButtonDel();
	virtual afx_msg void OnToolbarbuttonadd();
	virtual afx_msg void OnToolbarbuttondel();
	virtual afx_msg void OnLinkageOnetoone();

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
