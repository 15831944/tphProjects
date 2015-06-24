#pragma once

#include "AirsideObjectTreeCtrl.h"
#include "Resource.h"
#include "TreeCtrlEx.h"
#include <CommonData/3DViewCommon.h>
#include <Common/ALTObjectID.h>
#include <Controls/XTResizeDialog.h>
#include "AirsideGUI/UnitPiece.h"

// COnBoardObjectBaseDlg dialog
class CAircraftElement;
class CPath2008;
class COnboardBaseDlgHlper;
class CTermPlanDoc;
class CPoint2008;
class ARCPath3;
class COnBoardObjectBaseDlg : public CXTResizeDialog
{
	DECLARE_DYNAMIC(COnBoardObjectBaseDlg)

public:
	COnBoardObjectBaseDlg();
	COnBoardObjectBaseDlg(CAircraftElement *pAircraftElement, CWnd* pParent = NULL);   // standard constructor
	virtual ~COnBoardObjectBaseDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG_OBJECTONBOARD };
	//static CXTResizeDialog *  NewObjectDlg(int nObjID,ALTOBJECT_TYPE objType, int nParentID, int nAirportID,int nProjID, CWnd *pParent = NULL);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);
	virtual FallbackReason GetFallBackReason() = 0;
	virtual void SetObjectPath(CPath2008& path){}
	virtual void SetObjectPath(const ARCPath3& path){}
	virtual bool UpdateOtherData(){ return true; }
	virtual bool DoTempFallBackFinished(WPARAM wParam, LPARAM lParam);
	HTREEITEM m_hRClickItem;

	ALTObjectIDList m_lstExistObjectName;

	void AdjustCombox(int nWidth);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void InitComboBox();
	void InitTypeComboBox();


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
	CAircraftElement *m_pObject;

	bool m_bNameModified;
	//except In constraint, out constraint, service location
	bool m_bPropModified;
	bool m_bPathModified;
	bool m_bTypeModify;


	//if new  = true ; update = false
	//bool m_bNewOrUpdateObject;

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

	afx_msg void OnToggleQueueFixed();
	afx_msg void OnDefineZPos();

protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel();
	CTermPlanDoc * GetDocument();
	virtual void UpdateTempObjectInfo();
	virtual void DoneEditTempObject();
	bool SetObjectName();
	
	virtual void OnDeletePath();

	virtual void ToggleQueueFixed();
	virtual void DefineZPos();
public:
	CStatic m_ImageStatic;

	COnboardBaseDlgHlper* mpCurHelper;  //switch between different elements types


};

class COnboardBaseDlgHlper
{
public:

};


class CSeatDlgHlper : public COnboardBaseDlgHlper
{
public:

};























