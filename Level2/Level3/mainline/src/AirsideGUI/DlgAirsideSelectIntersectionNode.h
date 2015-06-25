#pragma once

class CDlgAirsideSelectIntersectionNode : public CDialog
{
	DECLARE_DYNAMIC(CDlgAirsideSelectIntersectionNode)

public:
	CDlgAirsideSelectIntersectionNode(int nSelID, int nProjID,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAirsideSelectIntersectionNode(void); 
	// Dialog Data
	enum { IDD = IDD_DIALOG_ALTOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString m_strSelNode;
	int m_nSelID;
protected:
	int m_nProjID;
	CTreeCtrl m_wndTreeCtrl;
protected:
	void SetTreeContents(void);
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnTvnSelchangedTreeTakeoffposi(NMHDR *pNMHDR, LRESULT *pResult);
};
