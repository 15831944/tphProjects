#pragma once
#include "afxwin.h"
#include "..\Inputs\ProductStockingList.h"
#include "..\Inputs\productstocking.h"
#include "../MFCExControl/ListCtrlEx.h"


// CDlgRetailProductsStocking dialog

#include <algorithm>

class CDlgRetailProductsStocking : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgRetailProductsStocking)

public:
	CDlgRetailProductsStocking(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgRetailProductsStocking();

// Dialog Data
	enum { IDD = IDD_DIALOG_RETAIL_PRODUCTS_STOCKING };

protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

private:
	CToolBar m_toolbarProduct;
	CToolBar m_toolbarDetail;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkListProductName();
	afx_msg LONG OnEndEdit( WPARAM p_wParam, LPARAM p_lParam );

	void InitStockingDetailList();
	void AddProduct();
	void AddStockingDetail();
	void OnEditProduct();
	void OnDelProduct();
	void OnDelDetail();
	void EditItem( int _nIdx );
	void SetProductNameList();
	void SetStockingDetailList();

	bool ReadFile(const CString &strFileName);
	void ReadStockingList();


private:
	CListBox m_listboxProduct;
	CListCtrlEx m_listctrlDetail;

	bool m_bResize;
	int m_nSelIdx;
	CWnd *m_pParentView;

	CStringArray m_vFileData;

	ProductStockingList m_ProductStockingList;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
public:
	afx_msg void OnLbnSelchangeListProduct();
	afx_msg void OnNMDblclkListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnLvnEndlabeleditListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnLvnItemchangedListStockingdetail(NMHDR *pNMHDR, LRESULT *pResult);
};
