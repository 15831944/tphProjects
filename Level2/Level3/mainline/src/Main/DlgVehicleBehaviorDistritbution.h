#pragma once
#include "MFCExControl/CoolTree.h"
#include "../MFCExControl/XTResizeDialog.h"
// CDlgVehicleBehaviorDistritbution dialog

class InputLandside;
class VehicleBehaviorDistributions;
class LandsideFacilityLayoutObjectList;
class VBDistributionObject;
class VBDistributionObjectServiceTime;

class CDlgVehicleBehaviorDistritbution : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicleBehaviorDistritbution)

protected:
	enum NodeType
	{
		NodeType_Resource = 0,
		NodeType_VehicleType,
		NodeType_ServiceTime,
		NodeType_Unit,


	};
	class VBNode
	{
	public:
		VBNode(NodeType ntype, VBDistributionObject *pO, VBDistributionObjectServiceTime *pS)
		{
			nodeType = ntype;
			pObject = pO;
			pServiceTime = pS;
		}
		~VBNode(){}

	public:
		NodeType nodeType;

		VBDistributionObject *pObject;

		VBDistributionObjectServiceTime *pServiceTime;
	};

public:
	CDlgVehicleBehaviorDistritbution(InputLandside* pInLandside, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleBehaviorDistritbution();

// Dialog Data
	enum { IDD = IDD_DIALOG_VEHICLEBEHAVIORDIST };


public:
	InputLandside* m_pInLandside;


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

public:
	void OnInitToolbar();
	void UpdateToolBarState();
	void LoadTreeContent();

	//return the object item handler
	HTREEITEM AddToTree(VBDistributionObject *pObject);

	HTREEITEM AddToTree(VBDistributionObject *pObject, VBDistributionObjectServiceTime *pServiceTime, HTREEITEM hParentItem);
	HTREEITEM InsertTreeItem(NodeType nodeType,
		VBDistributionObject *pObject,
		VBDistributionObjectServiceTime *pServiceTime, 
		HTREEITEM hParentItem );

	HTREEITEM UpdateItemText(NodeType nodeType,
		VBDistributionObject *pObject,
		VBDistributionObjectServiceTime *pServiceTime, 
		HTREEITEM hCurItem );

	void SetModified(BOOL bModified = TRUE);


private:
	CToolBar  m_wndToolbar;
	CCoolTree m_wndTreeCtrl;


	std::vector<VBNode *> m_vNodes;
protected:
	VehicleBehaviorDistributions *m_pVehicleDistriution;

protected:
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	afx_msg void OnNewResource();
	afx_msg void OnNewVehicle();
	afx_msg void OnDelResVehi();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	LandsideFacilityLayoutObjectList* GetLandsideLayoutObjectList();
	afx_msg void OnBtnSave();
};
