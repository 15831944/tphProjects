#pragma once
#include "afxcmn.h"
#include ".\MFCExControl\ListCtrlEx.h"
#define  ACTYPEBMP_COUNT 4

// CDlgAircraftModelDefine dialog
class CAircraftComponentModelDatabase;
class CComponentMeshModel;
class CDlgComponentDBDefine : public CDialog
{
	DECLARE_DYNAMIC(CDlgComponentDBDefine)

public:
	CDlgComponentDBDefine(CAircraftComponentModelDatabase* pAircraftComponentModelDB,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgComponentDBDefine();

// Dialog Data
	enum { IDD = IDD_DIALOG_COMPONENTDB_DEFINE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	virtual void OnCancel();

	DECLARE_MESSAGE_MAP()
public:
	void OnInitToolbar();
	void OnInitListCtrl();
	void DisplayBmpInCtrl(HBITMAP hBmp,UINT nID);
	void UpdateToolbarState();
	void PopulateACModelItem();
	void DisplayAllBmpInCtrl();
	void SetListCtrlItemText(int nItem,const CComponentMeshModel* pModel);
	CComponentMeshModel* GetModelToEditShape() const { return m_pModelToEditShape; }

	void LoadBmp(CComponentMeshModel*);

	HBITMAP m_hBmp[ACTYPEBMP_COUNT];
	void ClearBmps(bool bDelete);



// 	void ShowDialog(CWnd* parentWnd);
// 	void HideDialog(CWnd* parentWnd);
protected:
	CToolBar m_wndToolbar;
	CListCtrlEx m_wndListCtrl;

	CAircraftComponentModelDatabase* m_pAircraftComponentModelDB;

	CComponentMeshModel* m_pModelToEditShape; //  the model whose shape will be edited after dialog closed
protected:
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnPaint();
	afx_msg void OnAddAircraftComponentItem();
	afx_msg void OnRemoveAircraftComponentItem();
	afx_msg void OnEditAircraftComponentItem();
	afx_msg void OnSelChangeACTypeModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditAircraftComponentShape();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
