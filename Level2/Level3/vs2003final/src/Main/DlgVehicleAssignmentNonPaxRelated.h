#pragma once
#include "../MFCExControl/CoolTree.h"

// CVehicleAssignmentNonPaxRelated dialog
class InputLandside;
class NewVehicleAssignNonPaxRelated;
class CAirportDatabase;
class VehicleNonPaxRelatedData;
class VehicleNonPaxRelatedPosition;
class VehicleNonPaxRelatedEntry;
class CProbDistEntry;

class CDlgVehicleAssignmentNonPaxRelated : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgVehicleAssignmentNonPaxRelated)

	class TreeNodeData
	{
	public:
		enum TreeNodeType
		{
			Vehicle_Node,
			Position_Node,
			Daily_Node,
			Start_Node,
			Arrival_Node,
			Stop_Node,
			EntryRoot_Node,
			Entry_Node,
			Offset_Node
		};

		TreeNodeData()
			:m_emType(Vehicle_Node)
		{

		}
		~TreeNodeData()
		{

		}

		DWORD m_dwData;
		TreeNodeType m_emType;
	};
public:
	CDlgVehicleAssignmentNonPaxRelated(InputLandside* pInLandside, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgVehicleAssignmentNonPaxRelated();

// Dialog Data
	enum { IDD = IDD_DIALOG_LANDSIDEVEHICLEASSIGNMENT_NONPAX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

protected:
	void OnInitToolbar();
	void UpdateToolBarState();
	void SetTreeContent();
	
	//Vehicle item
	void InsertVehicleItem(HTREEITEM hItem,VehicleNonPaxRelatedData* pVehicle);
	//position item
	void InsertPositionItem(HTREEITEM hItem,VehicleNonPaxRelatedPosition* pPosition);
	//entry item
	void InsertEntryItem(HTREEITEM hItem, VehicleNonPaxRelatedEntry* pEntry);

	void GetProbDropdownList(CStringList& strList);

	void DeleteTreeNodeData( TreeNodeData* pNodeData );
	CProbDistEntry* ChangeProbDist( HTREEITEM hItem,const CString& strDist);
protected:
	virtual void OnOK();
	virtual void OnCancel();


	void SetModified(BOOL bModified = TRUE);
	afx_msg void OnCmdNewItem();
	afx_msg void OnCmdDeleteItem();
	afx_msg void OnAddObject();
	afx_msg void OnDeleteObject();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnBtnSave();
protected:
	CCoolTree	   m_wndTreeCtrl;
	CToolBar       m_wndToolBar;

	InputLandside*	m_pInLandside;
	NewVehicleAssignNonPaxRelated* m_pAssignment;
	std::vector<TreeNodeData*> m_vNodeData;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};
