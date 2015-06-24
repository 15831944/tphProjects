#pragma once
#include "../MFCExControl/ListCtrlEx.h"

// CDlgAircraftFurnishingDB dialog
class CAircraftFurnishingModel ;
class CAircraftFurnishingSectionManager ;
class CAircraftFurnishingModel;
class CModel;
class CDlgAircraftFurnishingDB : public CDialog
{
	DECLARE_DYNAMIC(CDlgAircraftFurnishingDB)

public:
	CDlgAircraftFurnishingDB(CAircraftFurnishingSectionManager* _FurnishMan ,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgAircraftFurnishingDB();;

	CAircraftFurnishingModel* GetEidtToShape()const {return m_pFurnishEditModel;}


	enum { IDD = IDD_DIALOG_FURNISHING_DB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnAddFurnishingSection();
	afx_msg void OnRemoveFurnishingSection();
	afx_msg void OnEditFurnishingSection();

	afx_msg void OnSelChangeFurnishingSection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPaint();
	afx_msg void OnBnClickedOk();

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);

	void LoadBmp( CModel* pModel );
	
    afx_msg void OnCancel() ;
	afx_msg LRESULT OnDBClick(WPARAM wParam, LPARAM lParam);
	LRESULT OnTempFallbackFinished(WPARAM wParam, LPARAM lParam);

	void OnInitToolbar();
	void OnInitListCtrl();
	int  InsertItemToList(CAircraftFurnishingModel* _AirsideFuenishing,int index) ;
	void UpdateToolbarState();
	void InitListData();
	void ShowDialog(CWnd* parentWnd);
	void HideDialog(CWnd* parentWnd);
	void DisplayAllBmpInCtrl();
	void DisplayBmpInCtrl(HBITMAP hBmp,UINT nID);

	void DisplayEmptyBmp( UINT nID );
	int GetCurSelIndex() ;

	void EditItemToList(CAircraftFurnishingModel* _AirsideFuenishing,int item);
	
	HBITMAP m_hBmp[4];
	void ClearBmps(bool bDelete);
protected:
	CAircraftFurnishingSectionManager*	m_pFurnishishManger;
	CToolBar m_wndACTypeModel;
	CAircraftFurnishingModel* m_pFurnishEditModel;
public:
	CListCtrlEx m_wndListCtrl;
	CStatic m_wndPicture;
};
