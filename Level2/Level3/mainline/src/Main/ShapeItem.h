#pragma once
#include "afxwin.h"


// CShapeItem dialog

class CShapeItem : public CDialog
{
	DECLARE_DYNAMIC(CShapeItem)
	enum Item_Style{ NEW = 0, NAME, PICTURE, MODEL };
    typedef enum {TopLeft, TopRight, BottomLeft, BottomRight} LayoutRef;
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
	afx_msg void OnLoadShapePicture();
	afx_msg void OnLoadShapeModel();
	virtual BOOL OnInitDialog();
	afx_msg void OnCancel();
	afx_msg void OnOk();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);


    CString GetFolderLocation() const { return m_folderLocation; }
    void SetFolderLocation(CString str) { m_folderLocation = str; }

    CString GetShapeName() const { return m_shapeName; }
    void SetShapeName(CString str) { m_shapeName = str; }

    CString GetShapePicture() const { return m_shapePicture; }
    void SetShapePicture(CString str) { m_shapePicture = str; }

    CString GetShapeModel() const { return m_shapeModel; }
    void SetShapeModel(CString str) { m_shapeModel = str; }

    CShapeItem::Item_Style GetStyle() const { return m_style; }
    void SetStyle(CShapeItem::Item_Style style) { m_style = style; }

private:
    void LayoutControl(CWnd* pCtrl, LayoutRef refTopLeft, LayoutRef refBottomRight, int cx, int cy);

protected:
    CEdit m_editShapeName;
    CEdit m_editShapeModel;
    CStatic m_staticPic;

    CString m_folderLocation;
    CString m_shapeName;
    CString m_shapePicture;
    CString m_shapeModel;
    Item_Style m_style;
private:
    int m_oldClientWidth;
    int m_oldClientHeight;

};
