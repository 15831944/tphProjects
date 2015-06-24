#pragma once
#include "Resource.h"
#include "InputAirside/WaveCrossingSpecification.h"
#include "../MFCExControl/ARCTreeCtrlExWithColor.h"
// CDlgWaveCrossRunwaySpecification dialog
#include "../InputAirside/WaveCrossingSpecification.h"
class HoldPosition;


class CDlgWaveCrossRunwaySpecification : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgWaveCrossRunwaySpecification)

public:
	CDlgWaveCrossRunwaySpecification(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgWaveCrossRunwaySpecification();

// Dialog Data
	enum { IDD = IDD_DIALOG_WAVECROSSRUNWAYSPECIFICATION };
protected:
enum enumItemType
{	
	itemType_None = 0,
	itemType_WaveCrossName,
	itemType_HoldRoot,
	itemType_Hold,
	itemType_ActiveTakeOff,
	itemType_TakeOffMinState,
	itemType_TakeOffWaves,
	itemType_LandingMinState,
	itemType_LandingMinMiles,
	itemType_LandingMinSeconds,
	itemType_LandingAllowWaves,
	itemType_LandingAllowSeconds
};
	class CTreeItemData
	{
	public:
		CTreeItemData()
		{
			itemType = itemType_None; 
			pItem = NULL;
			pHoldPosition = NULL;
		}

		enumItemType itemType;
		CWaveCrossingSpecificationItem *pItem;
		//only take effect on hold item
		HoldPosition *pHoldPosition;

	};


protected:

void InitToolBar();
void LoadTree();
// update toolbar's state depend on conditions
void UpdateToolBarState();

//load one item
void LoadWaveCrossItem(CWaveCrossingSpecificationItem *pItem);



void OnAddWaveCross();
void OnDelWaveCross();

void OnAddHoldPosition();
void OnDeleteHoldPosition();

void OnEditWaveCrossName();

void OnEditTakeOffMinimumType();

void OnEditTakeOffWaves();

void OnEditLandingMinimumType();

void OnEditLandingMinMiles();

void OnEditLandingMinSeconds();

void OnEditLandingAllowWaves();

void OnEditLandingAllowSeconds();


void HideDialog(CWnd* parentWnd);
void ShowDialog(CWnd* parentWnd);
protected:
	//if have no type return itemType_Error
	enumItemType GetItemType(HTREEITEM hItem);
	//return wave cross item pointer
	CWaveCrossingSpecificationItem * GetItemWaveCross(HTREEITEM hItem);

	//return hold pinter
	//if have no, return NULL, only available in hold node
	HoldPosition* GetHoldPosition(HTREEITEM hItem);

	void LoadTakeOffItems(HTREEITEM hTakeOffItem, CWaveCrossingSpecificationItem *pItem);
	void LoadLandingItems(HTREEITEM hTakeOffItem, CWaveCrossingSpecificationItem *pItem);

	void DeleteItem(HTREEITEM hItem);
	void DeleteChildItem(HTREEITEM hItem);

protected:
	int m_nProjID;
	CWaveCrossingSpecification m_waveCrossSpecification;
protected:
	CToolBar m_toolbar;
	CARCTreeCtrlExWithColor m_treeCtrl;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
	virtual void OnCancel();
protected:
		virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedButtonSave();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPickfrommap();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnTreeDoubleClick(WPARAM, LPARAM);
	afx_msg void OnToolBarAdd();
	afx_msg void OnToolBarDel();
	afx_msg void OnToolBarEdit();
	afx_msg void OnTvnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult);
};
