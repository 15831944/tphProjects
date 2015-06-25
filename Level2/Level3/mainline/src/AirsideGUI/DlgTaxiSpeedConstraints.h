#pragma once
#include "afxcmn.h"

#include "resource.h"
// CDlgTaxiSpeedConstraints dialog
#include "MFCExControl/ARCTreeCtrl.h"
#include "MFCExControl/ListCtrlEx.h"
#include <MFCExControl/XTResizeDialogEx.h>
#include "..\MFCExControl\SplitterControl.h"

#include "NodeViewDbClickHandler.h"
#include "DlgTaxiwaySelect.h"

#include "InputAirside/TaxiSpeedConstraints.h"
#include "InputAirside/IntersectionNode.h"


class CDlgTaxiSpeedConstraints : public CXTResizeDialogEx
{
	DECLARE_DYNAMIC(CDlgTaxiSpeedConstraints)

private:
	enum ToolBarCmdID
	{
		ID_TAXIWAY_NEW = 10,
		ID_TAXIWAY_EDIT,
		ID_TAXIWAY_DEL,

		ID_CONSTRAINTS_NEW,
		ID_CONSTRAINTS_DEL,
	};

	enum TaxiwayTreeNodeType
	{
		TAXIWAY_NODE = 0,
		FLTTYPE_NODE,
	};
public:
	CDlgTaxiSpeedConstraints(int nProjID, CAirportDatabase* pAirportDB, PFuncSelectFlightType pFuncSelectFlightType, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTaxiSpeedConstraints();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

// Dialog Data
	enum { IDD = IDD_TAXISPEED_CONSTRAINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	void InitToolbar();
	void SetResizeItems();
	void InitListCtrlHeader();

	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);

	afx_msg void OnMsgTaxiwayNew();

	afx_msg void OnMsgTaxiwayEdit();
	afx_msg void OnMsgTaxiwayDel();

	afx_msg void OnAddFlightType();
	afx_msg void OnEditFlightType();
	afx_msg void OnDelFlightType();

	afx_msg void OnSelChangedTaxiwayTree(NMHDR *pNMHDR, LRESULT *pResult);

	afx_msg void OnMsgConstraintsNew();
	afx_msg void OnMsgConstraintsDel();

	afx_msg void OnSize(UINT nType, int cx, int cy);

	void ResetTreeCtrlContent();
	void UpdateRightListCtrl();

	void UpdateSplitterRange();

	void UpdateConstraintsToolbar();
	HTREEITEM GetSelTreeItemByNodeType(TaxiwayTreeNodeType nodeType);

	static TaxiwaySpeedConstraintDataItem* GetOneConstraintItemByTaxiwayID( int nTaxiwayID );

	DECLARE_MESSAGE_MAP()

private:
	PFuncSelectFlightType m_pFuncSelectFlightType;

	int m_nProjID;
	TaxiSpeedConstraints m_TaxiwayConstraints;
	CAirportDatabase*    m_pAirportDB;

	class TaxiwayChecker : public ITaxiwayFilter
	{
	public:
		TaxiwayChecker(TaxiSpeedConstraints* pTaxiwayConstraints, int nCurTaxiwayID = 0);

		virtual BOOL IsAllowed(int nTaxiwayID, CString& strError);

	private:
		TaxiSpeedConstraints* m_pTaxiwayConstraints;
		int m_nCurTaxiwayID;
	};

	class TaxiwayNodeSorter
	{
	public:
		TaxiwayNodeSorter(int nTaxiwayID)
			: m_nTaxiwayID(nTaxiwayID)
		{

		}
		bool operator()(const IntersectionNode& lhs, const IntersectionNode& rhs)
		{
			return lhs.GetDistance(m_nTaxiwayID) < rhs.GetDistance(m_nTaxiwayID);
		}
	private:
		int m_nTaxiwayID;
	};



	class CTaxiSpeedConstraintsListCtrlEx : public CListCtrlEx
	{
	public:
		CTaxiSpeedConstraintsListCtrlEx();
		~CTaxiSpeedConstraintsListCtrlEx();

		void InitListHeader();
		void ResetListContent(TaxiwaySpeedConstraintFlttypeItem* pConstraintFlttyptItem, int nTaxiwayID);

		void AddOneNewItem();
		void DeleteSelectedItem();

		BOOL IsAllItemsValid(CString& strError);

	protected:
		LRESULT OnComboBoxChange(WPARAM wParam, LPARAM lParam);
		LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

		void ResetDropDownList();

		DECLARE_MESSAGE_MAP()
	private:
		TaxiwaySpeedConstraintFlttypeItem* m_pConstraintFlttypeItem;
		int m_nTaxiwayID;

		IntersectionNodeList m_taxiwayNodeList;
	};

	// controls
	CARCTreeCtrl m_wndTaxiway;
	CTaxiSpeedConstraintsListCtrlEx  m_wndConstraints;
	CSplitterControl m_wndSplitterVer;

	CToolBar     m_wndTaxiwayToolbar;
	CToolBar     m_wndConstraintsToolbar;

	HTREEITEM    m_hTaxiwayLastSelFltTypeItem;

};
