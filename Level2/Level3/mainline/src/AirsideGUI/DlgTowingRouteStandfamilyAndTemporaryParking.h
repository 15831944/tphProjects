#pragma once
#include "afxcmn.h"


// CDlgTowingRouteStandfamilyAndTemporaryParking dialog
class ALTObject;
class CDlgTowingRouteStandfamilyAndTemporaryParking : public CDialog
{
	DECLARE_DYNAMIC(CDlgTowingRouteStandfamilyAndTemporaryParking)

public:
	CDlgTowingRouteStandfamilyAndTemporaryParking(int nProjID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgTowingRouteStandfamilyAndTemporaryParking();

// Dialog Data
	enum { IDD = IDD_DIALOG_TOWINGROUTESTANDFAMILYANDTEMPORARYPARKING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_wndTree;
	std::vector<ALTObject> m_vStandFamily;
	int m_nProjID;
	int m_nSelALTObjectID;
	CString m_strSelALTObjectName;
	int m_nSelALTObjectFlag;

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurStandGroupNamePos(HTREEITEM hTreeItem);
	bool StandFamilyAndTemporaryParkingInTree();
public:
	int GetSelALTObjectID(){return m_nSelALTObjectID;}
	CString GetSelALTObjectName(){return m_strSelALTObjectName;}
	int GetSelALTObjectFlag(){return m_nSelALTObjectFlag;}
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
