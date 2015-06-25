#pragma once

#include "..\InputAirside\AirsideEnrouteQueueCapacity.h"
#include "..\MFCExControl\ARCTreeCtrlExWithColor.h"
#include "..\InputAirside\ALTObject.h"
#include "resource.h"


// CAirsideEnrouteQueueCapacity dialog

class CDlgAirsideEnrouteQueueCapacity : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgAirsideEnrouteQueueCapacity)

public:
	CDlgAirsideEnrouteQueueCapacity(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirsideEnrouteQueueCapacity();


public:
	enum ItemType
	{
		ItemType_TimeRange = 0,
		ItemType_TakeOffPos,
		ItemType_MaxLen
	};
	class CTreeItemData
	{
	public:
		CTreeItemData(ItemType itemtype )
		{
			pTimeRangeData = NULL;
			pEnrouteQItem = NULL;
			enumType = itemtype;

		}

		~CTreeItemData(){}

	public:
		AirsideEnrouteQCapacityTimeRange *pTimeRangeData;
		AirsideEnrouteQCapacityItem *pEnrouteQItem;
		ItemType enumType;
	};


// Dialog Data
	enum { IDD = IDD_DIALOG_AIRSIDE_ENROUTEQCAPACITY };

protected:
	CToolBar m_wndToolBar;
	CARCTreeCtrlExWithColor m_treeCtrl;

	CAirsideEnrouteQueueCapacity m_enrouteQCapacity;
	
	std::vector<ALTObject > m_vRunways;

	int m_nProjectID;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void InitToolbar();
	void LoadTree();
	void AddTimeRange(AirsideEnrouteQCapacityTimeRange *pTimeRangeData);
	void AddEnrouteQueueCapacity(AirsideEnrouteQCapacityTimeRange *pTimeRangeData, AirsideEnrouteQCapacityItemList *pEnrouteQ, HTREEITEM hTimeRangeItem);
	void AddEnrouteQueueCapacityItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData, AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hTimeRangeItem);
	void AddEnrouteQueueTakeOffPositionItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData, AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hTimeRangeItem);
	void AddEnrouteQueueMaxLengthItem(AirsideEnrouteQCapacityTimeRange *pTimeRangeData, AirsideEnrouteQCapacityItem *pEnrouteQItem, HTREEITEM hMaxLengthItem);
	void ReadRunwayList();

	void UpdateItemText(HTREEITEM hItemUpdate);
	
	void UpdateToolbarState();

	void SetModified( BOOL bModified = TRUE);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnEditOperation();
	afx_msg void OnNewOperation();
	afx_msg void OnDelOperation();

	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnTreeDoubleClick(WPARAM, LPARAM);
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	

};
