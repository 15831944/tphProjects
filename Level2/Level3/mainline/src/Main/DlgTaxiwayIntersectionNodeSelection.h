#pragma once

class DlgTaxiwayIntersectionNodeSelection: public CDialog
{
	DECLARE_DYNAMIC(DlgTaxiwayIntersectionNodeSelection)
public:
	DlgTaxiwayIntersectionNodeSelection(int nProjID, int nTaxiwayID, CWnd* pParent = NULL);
	virtual ~DlgTaxiwayIntersectionNodeSelection(void);

	enum { IDD = IDD_DIALOG_REPORT_SELECTALTOBJECT };

	int GetNodeID();
	const CString& GetNodeName();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual afx_msg void OnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);

	CTreeCtrl m_wndALTObjectTree;

	int m_nProjectID;
	int m_nTaxiwayID;
	int m_nIntersectionNodeID;
	CString m_strNodeName;


};
