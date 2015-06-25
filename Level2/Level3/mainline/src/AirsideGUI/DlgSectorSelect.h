#pragma once
#include "afxcmn.h"
#include "Resource.h"
#include "SectorSelectDoc.h"
// DlgSectorSelect dialog

class DlgSectorSelect : public CDialog
{
	DECLARE_DYNAMIC(DlgSectorSelect)

protected:
	DlgSectorSelect(CWnd* pParent = NULL);   // standard constructor
	
public: 
    DlgSectorSelect(int projid) ;
	virtual ~DlgSectorSelect();
// Dialog Data
	enum { IDD = IDD_DIALOG_SELECTSECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	CTreeCtrl sector_tree;
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL  OnInitDialog();
protected:
	void loadTree();
	int  setSelectedNameAndID();
	void AddObjectToTree(HTREEITEM hObjRoot, ALTObject* pObject) ;
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText) ;
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem) ;
protected:
    CSectorSelectDoc* sectorDoc ;
	CString  selectedName ;
	int nameID ;
public:

	CString getName() ;
	int getNameID() ;
};
