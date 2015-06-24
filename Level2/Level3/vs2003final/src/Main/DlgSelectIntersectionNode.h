#pragma once

// CDlgSelectTakeoffPosition dialog

class CDlgSelectIntersectionNode : public CDialog
{
	DECLARE_DYNAMIC(CDlgSelectIntersectionNode)

public:
	CDlgSelectIntersectionNode(int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSelectIntersectionNode(void); 
	// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTALTOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSelNode;
	int m_nSelID;
protected:
	int m_nProjID;
	CXTTreeCtrl m_wndTreeCtrl;
protected:
	void SetTreeContents(void);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult);
};
