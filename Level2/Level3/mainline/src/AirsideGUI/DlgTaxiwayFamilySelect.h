#pragma once
#include "AirsideGUIAPI.h"
class ALTObject;
class Stand;
// CDlgTaxiwayFamilySelect dialog


// use this interface to decide whether stand family is allowed
// if not, strError will be displayed in a message box
class ITaxiwayFamilyFilter
{
public:
	virtual BOOL IsAllowed(int nTaxiwayFamilyID, CString& strError) = 0;
};

class AIRSIDEGUI_API CDlgTaxiwayFamilySelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaxiwayFamilySelect)

public:
	CDlgTaxiwayFamilySelect(int nProjID, ITaxiwayFamilyFilter* pTaxiwayFamilyFilter = NULL, CWnd* pParent = NULL);   
	virtual ~CDlgTaxiwayFamilySelect();


	int GetSelTaxiwayFamilyID(){return m_nSelTaxiwayFamilyID;}
	CString GetSelTaxiwayFamilyName(){return m_strTaxiwayFamily;}

	bool IsSingleTaxiwaySelected(){return m_bIsSingleTaxiwaySel; }

// Dialog Data
	//enum { IDD = IDD_DIALOG_STANDFAMILY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);   
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	void LoadTree();
	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	int GetCurTaxiwayGroupNamePos(HTREEITEM hTreeItem);
	bool TaxiwayFamilyInTree();

	DECLARE_MESSAGE_MAP()

private:
	std::vector<ALTObject> m_vTaxiwayFamily;
	int m_nProjID;
	CTreeCtrl m_TreeTaxiwayFamily;
	CString m_strTaxiwayFamily;
	int m_nSelTaxiwayFamilyID;
	bool m_bIsSingleTaxiwaySel;

	ITaxiwayFamilyFilter* m_pTaxiwayFamilyFilter;

	static LPCTSTR m_sAllFamilyString;
};
