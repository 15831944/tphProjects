#pragma once
#include "afxwin.h"
#include "../InputAirside/ALTObject.h"
#include "muliseltreectrlex.h"
#include "..\inputs\in_term.h"

class CSelectALTObjectDialog :
	public CDialog
{
public:
	CSelectALTObjectDialog(int prjID, int airportID, CWnd* pParent = NULL );
	virtual ~CSelectALTObjectDialog(void);



	// Dialog Data
	//{{AFX_DATA(CSelectALTObjectDialog)
	enum { IDD = IDD_DIALOG_ALTOBJECTSELECT };
	CButton	m_btCancle;
	CButton	m_btOk;
	CEdit	m_editResult;
	CListBox	m_listObject;
	CMuliSeltTreeCtrlEx	m_ObjectTree;
	CString	m_strObjID;
	std::vector<CString> m_vStrList;
	//}}AFX_DATA 

public:
	void SetType(ALTOBJECT_TYPE objType);
	void SetSelectString(const CString& str);
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectALTObjectDialog)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	ALTOBJECT_TYPE m_nObjType;
public:
	bool GetObjectIDString(CString& objIDstr);
	int GetObjectIDStringList(std::vector<CString>& strList);
	CString GetSelectString()const{ return m_strObjID; }
protected:
	int m_nPrjID;
	int m_nAirportID;
	
	bool LoadObjectsTree(HTREEITEM hObjRoot);
	CMuliSeltTreeCtrlEx& GetTreeCtrl(){ return m_ObjectTree; }
	HTREEITEM AddTreeItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);

	CString GetTreeItemFullString(HTREEITEM hItem);

	HTREEITEM GetObjectTreeItem(const ALTObjectID& objID);

	// Generated message map functions
	//{{AFX_MSG(CSelectALTObjectDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTreeALTObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBtnSelect();
	afx_msg void OnSelchangedTreeALTObject(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeALTObjectList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()



};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
