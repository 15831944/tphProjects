#pragma once

class CTowOffStandAssignmentData;
class ALTObject;
class Stand;
// CSelectIntermediateStandsDlg dialog

class CSelectIntermediateStandsDlg : public CDialog
{
	DECLARE_DYNAMIC(CSelectIntermediateStandsDlg)

public:
	enum SelectStandType
	{
		ParkStand = 0,
		PriorityStand,
		ReturnToStand,
	};
	CSelectIntermediateStandsDlg(int nProjID, CTowOffStandAssignmentData *pTowOffStandAssignmentData,
		int nStandID, SelectStandType enumParkStandType = ParkStand,
		int nPriorityStandID = -1/* ignore when stand enumParkStandType is not ReturnToStand */,
		CWnd* pParent = NULL);   // standard constructor
	virtual ~CSelectIntermediateStandsDlg();

	afx_msg void OnTvnSelchangedTreeIntermediatestands(NMHDR *pNMHDR, LRESULT *pResult);
// 	int GetCurStandID() const { return m_nCurStandID; }
	void* GetNewCreatedData() const { return m_pNewCreatedData; }
	CString GetStandName() { return m_strStandName.IsEmpty()?_T("All"):m_strStandName; }

// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTINTERMEDIATESTANDS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	BOOL DealWithStandFamilyID(int nStandFamilyID);
	DECLARE_MESSAGE_MAP()

private:
	BOOL IsSelectStand(HTREEITEM hSelItem);

	//Set standfamily
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);

	void SetCurStandGroupName(HTREEITEM hTreeItem);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);


	CTowOffStandAssignmentData*		m_pTowOffStandAssignment;
	int								m_nProjID;
	int								m_nCurStandID;
	CString							m_strStandName;
	int								m_nPriorityStandID;
	Stand*							m_pSelStand;
	SelectStandType					m_enumSelectStandType;
	CTreeCtrl						m_TreeCtrlIntermediateStands;
	void*							m_pNewCreatedData;
};
