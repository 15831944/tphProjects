#pragma once


// CAirsideObjectBaseDlg dialog
#include "TreeCtrlEx.h"
#include <CommonData/Fallback.h>
#include "../InputAirside/ALTObject.h"
#include "afxwin.h"
#include "../Common/LatLongConvert.h"
#include "AirsideObjectTreeCtrl.h"
#include "../Common/CARCUnit.h"

class CTermPlanDoc;
class CPath2008;
class CPoint2008;
//class ALTObject;
class CAirsideObjectBaseDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CAirsideObjectBaseDlg)

public:
	CAirsideObjectBaseDlg(int nObjID,int nAirportID,int nProjID,CWnd* pParent = NULL);   // standard constructor
	//CAirsideObjectBaseDlg(ALTObject * pObj,int nProjID, CWnd * pParent = NULL);
	virtual ~CAirsideObjectBaseDlg();
	//nParentID is used for contour
	static CXTResizeDialog *  NewObjectDlg(int nObjID,ALTOBJECT_TYPE objType, int nParentID, int nAirportID,int nProjID, CWnd *pParent = NULL);
// 	static CXTResizeDialog *  NewObjectDlg(ALTObject * pObj,int nProjID,CWnd *pParent = NULL);
// Dialog Data
	enum { IDD = IDD_DIALOG_OBJECTAIRSIDE };
	static bool OnPropModifyLatLong(CLatitude& lat, CLongitude& longt);
	static bool OnPropModifyPosition(CPoint2008& pt,ARCUnit_Length emType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);
	virtual FallbackReason GetFallBackReason() = 0;
	virtual void SetObjectPath(CPath2008& path) = 0;
	virtual bool UpdateOtherData() = 0;
	virtual bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);
	HTREEITEM m_hRClickItem;

	int m_nObjID;
	int m_nAirportID;
	int m_nProjID;
	int GetAirportID()
	{ 
		return m_nAirportID;
	};

	ALTObjectIDList m_lstExistObjectName;

	void AdjustCombox(int nWidth);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void InitComboBox();

	void UpdateObjectViews();
	void UpdateAddandRemoveObject();
	void UpdateNewViews();
	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbName1;
	CComboBox m_cmbName2;
	CComboBox m_cmbName3;
	CComboBox m_cmbName4;
	CAirsideObjectTreeCtrl m_treeProp;
	ALTObjectID m_objName;
	ref_ptr<ALTObject> m_pObject;
	bool m_bNameModified;
	//except In constraint, out constraint, service location
	bool m_bPropModified;
	bool m_bPathModified;
	bool m_bTypeModify;
	inline ALTObject * GetObject(){ return m_pObject.get(); } 
	afx_msg void OnCbnKillfocusComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel1();
	afx_msg void OnCbnDropdownComboNameLevel2();
	afx_msg void OnCbnDropdownComboNameLevel3();
	afx_msg void OnCbnDropdownComboNameLevel4();
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnProcpropPickfrommap();
	afx_msg LRESULT OnTempFallbackFinished(WPARAM, LPARAM);
	afx_msg LRESULT OnPropTreeDoubleClick(WPARAM, LPARAM);
	afx_msg void OnProcpropDelete();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheckLocked();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
	CTermPlanDoc * GetDocument();
	virtual void UpdateTempObjectInfo();
	virtual void DoneEditTempObject();
public:
	CStatic m_ImageStatic;
	CButton m_btnCheckLocked;
};
