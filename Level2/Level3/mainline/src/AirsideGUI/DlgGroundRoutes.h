#pragma once
#include "Resource.h"
#include "MFCExControl/EditListBox.h"
#include "..\MFCExControl\CoolTree.h"
#include "InputAirside\ALTObject.h"
#include "..\MFCExControl\ARCTreeCtrl.h"
#include "../MFCExControl/XTResizeDialog.h"


class GroundRoute;
class GroundRoutes;
class GroundRouteItem;
// CDlgGroundRoutes dialog

class AIRSIDEGUI_API CDlgGroundRoutes : public CXTResizeDialog
{
public:
	CDlgGroundRoutes(int nProjectID, int nAirportID, CWnd* pParent = NULL);
	virtual ~CDlgGroundRoutes();

	enum { IDD = IDD_GROUNDROUTE };

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	int m_nProjectID;
    int m_nAirPortID;
	GroundRoutes *m_groundRoutes;
	GroundRoute  *m_pCurGroundRoute;
	HTREEITEM    m_curParentItem;

	CEditListBox  m_wndGroundRouteListBox;    
	CARCTreeCtrl     m_wndTreeGroundRouteItem;
	CToolBar      m_wndGroundRouteItemToolbar;
	CToolBar      m_wndGroundRouteToolbar;

private:
	void InitGroundRoutesListBox();
	void InitToolbar();
	void UpdateGroundRouteItemToolBar(BOOL bGroundRouteChange);
	void UpdateGroundRouteItemTree();
	void SetComboBoxItem(CComboBox* pCB);
	//return TRUE:Exist, return FALSE: doesn't exist
	BOOL IsIntersectObjExistInComboBox(CComboBox* pCB, int nObjID);
	void AddGroundRouteItem();
	void AddGroundRouteItem(GroundRouteItem *pGroundRouteItem, HTREEITEM hParentItem);

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSave();
	afx_msg void OnNewGroundRoute();
	afx_msg void OnDelGroundRoute();
	afx_msg void OnNewGroundRouteItem();
	afx_msg void OnDelGroundRouteItem();
	afx_msg void OnEditGroundRouteItem();
	afx_msg void OnDestroy();
	afx_msg void OnLbnSelchangeListboxGroundroute();
	afx_msg void OnTvnSelchangedGroundRouteItem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnMessageModifyGroundRouteName( WPARAM wParam, LPARAM lParam );
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
};
