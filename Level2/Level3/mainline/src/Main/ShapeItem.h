#pragma once
#include "afxwin.h"


// CShapeItem dialog

class CShapeItem : public CDialog
{
	DECLARE_DYNAMIC(CShapeItem)
	enum Item_Style{ NEW = 0, NAME, PICTURE, MODEL };
public:
	CShapeItem(CString folderpath,CWnd* pParent = NULL);   // standard constructor
	CShapeItem::CShapeItem(CString folderpath,CString name, CString picture, CString model, Item_Style style  = NEW ,CWnd* pParent  = NULL );
	virtual ~CShapeItem();

// Dialog Data
	enum { IDD = IDD_SHAPEITEM };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString folderPath;
	CEdit m_itemName;
	CEdit m_itemModel;
	CString itemName;
	CString itemPicture;
	CString itemModel;
	Item_Style m_style;
	static int item_id;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	CStatic m_itemPicture;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
};
