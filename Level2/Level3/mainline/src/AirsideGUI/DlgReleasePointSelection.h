#pragma once
#include "afxcmn.h"
#include "InputAirside/TowOperationSpecification.h"
#include "InputAirside/HoldShortLines.h"
#include "../MFCExControl/XTResizeDialog.h"
// DlgReleasePointSelection dialog

class AIRSIDEGUI_API DlgReleasePointSelection: public CXTResizeDialog
{
	DECLARE_DYNAMIC(DlgReleasePointSelection)

public:
	DlgReleasePointSelection(int nPrjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~DlgReleasePointSelection();

// Dialog Data
	enum { IDD = IDD_DLGRELEASEPOINTSELECTION };

	virtual BOOL OnInitDialog();

	void GetSelection(CReleasePoint* pPoint);
	afx_msg void OnBnClickedOk();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	bool ALTObjectGroupInTree(TOWOPERATIONDESTINATION eType );

	TOWOPERATIONDESTINATION GetSelectionType(HTREEITEM hSelItem);
private:
	CTreeCtrl m_wndTreeCtrl;

	int m_nPrjID;

	TaxiInterruptLineList m_holdShortLines;

	CString m_strSelectName;
	int m_nObjID;
	TOWOPERATIONDESTINATION m_eSelType;
};
