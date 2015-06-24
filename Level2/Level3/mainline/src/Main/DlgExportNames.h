#pragma once
#include "afxcmn.h"
#include "..\MFCExControl\CoolTree.h"

// CDlgExportNames dialog
class CARCPort;
class ALTObjectIDList;
class CDlgExportNames : public CDialog
{
	DECLARE_DYNAMIC(CDlgExportNames)

protected:
	enum ItemType
	{
		IT_NORMAL = 0,
		IT_OBJECT
	};
	class ItemData
	{
	public:
		ItemData()
		{
			enumType = IT_NORMAL;
			strName = _T("");
		}

		ItemType enumType;
		CString strName;
	protected:
	private:
	};

public:
	CDlgExportNames(CARCPort *pARCPort, int nProjectID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgExportNames();

// Dialog Data
	enum { IDD = IDD_DIALOG_EXPORTNAMELIST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:


	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonExport();
	afx_msg LRESULT OnStatusChanged(WPARAM wParam, LPARAM lParam);
protected:

	HTREEITEM AddItem(const CString& strItemText, HTREEITEM hParentItem, ItemType enumType = IT_NORMAL, CString strName = _T(""));
protected:
	void  CheckChildItem(HTREEITEM hItem);
	
	void GetExportNames(HTREEITEM hItem, std::vector<CString>& vExportName );

protected:
	CCoolTree m_wndTreeName;
	CARCPort *m_pARCPort;
	int m_nProjectID;

	HTREEITEM m_hTerminalRoot;

	HTREEITEM m_hAirsideRoot;
	HTREEITEM m_hLandsideRoot;
	HTREEITEM m_hOnboardRoot;

	std::vector<ItemData *> m_vNodeData;

protected:
	void LoadTerminal();
	void LoadAirside();
	void LoadLandside();
	void LoadOnboard();

	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	void LoadObjects(HTREEITEM hRoot,enum ALTOBJECT_TYPE enumType,CString strTypeName, int nAirportID);
protected:
	virtual void OnOK();

public:

};
