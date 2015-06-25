#pragma once
#include "AirsideGUIAPI.h"
class ALTObject;
class Stand;
// CDlgTaxiwaySelect dialog


// use this interface to decide whether stand family is allowed
// if not, strError will be displayed in a message box
class ITaxiwayFilter
{
public:
	virtual BOOL IsAllowed(int nTaxiwayFamilyID, CString& strError) = 0;
};

class AIRSIDEGUI_API CDlgTaxiwaySelect : public CDialog
{
	DECLARE_DYNAMIC(CDlgTaxiwaySelect)

public:
	CDlgTaxiwaySelect(int nProjID, ITaxiwayFilter* pTaxiwayFilter = NULL, CWnd* pParent = NULL);   
	virtual ~CDlgTaxiwaySelect();


	int GetSelTaxiwayID(){return m_nSelTaxiwayID;}
	CString GetSelTaxiwayName(){return m_strTaxiway;}

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
	bool TaxiwayInTree();

	DECLARE_MESSAGE_MAP()

private:
// 	std::vector<ALTObject> m_vTaxiways;
	int m_nProjID;
	CTreeCtrl m_TreeTaxiway;
	CString m_strTaxiway;
	int m_nSelTaxiwayID;

	ITaxiwayFilter* m_pTaxiwayFilter;

	static LPCTSTR m_sAllTaxiwayString;
};
