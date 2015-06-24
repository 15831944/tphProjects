#pragma once
// CAirsideObjectBaseDlg dialog
#include <CommonData/Fallback.h>
#include <common/ALTObjectID.h>
#include "../AirsideGUI/UnitPiece.h"
#include "AirsideObjectTreeCtrl.h"
#include "LayoutObjectPropDlgImpl.h"
#include "../MFCExControl/XTResizeDialog.h"


class CTermPlanDoc;
class CPoint2008;
class CPath2008;
class ILayoutObject;
class InputLandside;


class CDlgCallBack
{
public:
	virtual void OnOk(){}
	virtual void OnCancel(){}
};
//class ALTObject;
class CARCLayoutObjectDlg : public CXTResizeDialog,public CUnitPiece
{
	DECLARE_DYNAMIC(CARCLayoutObjectDlg)

public:
	CARCLayoutObjectDlg(LandsideFacilityLayoutObject* pObj,CDlgCallBack* pCallback = NULL, CWnd* pParent = NULL,QueryData* pData = NULL);   // standard constructor	
	virtual ~CARCLayoutObjectDlg();
	virtual BOOL CARCLayoutObjectDlg::PreTranslateMessage(MSG* pMsg); 
	// Dialog Data
	enum { IDD = IDD_DIALOG_OBJECTAIRSIDE };
	CAirsideObjectTreeCtrl& GetTreeCtrl(){ return m_treeProp; }
	CButton *GetCheckBoxCellPhone();
	int GetProjectID();
	InputLandside* getInputLandside();
	CTermPlanDoc * GetDocument();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();


	void AdjustCombox(int nWidth);
	void HideDialog(CWnd* parentWnd);
	void ShowDialog(CWnd* parentWnd);
	void InitComboBox();		

	
	//unit piece
	void InitUnitPiece();
	bool ConvertUnitFromDBtoGUI(void);
	bool RefreshGUI(void);
	bool ConvertUnitFromGUItoDB(void);


	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbName1;
	CComboBox m_cmbName2;
	CComboBox m_cmbName3;
	CComboBox m_cmbName4;

	CAirsideObjectTreeCtrl m_treeProp;

	//except In constraint, out constraint, service location		
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
	afx_msg void OnControlPathRevese();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedCheckLocked();
	//afx_msg LRESULT OnPropTreeLButtonDown(WPARAM, LPARAM);
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void HandleChangeVerticalProfile();
	afx_msg void OnDestroy();
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnOK();
	virtual void OnCancel(); 
	ILayoutObjectPropDlgImpl* m_pImpl;
	CDlgCallBack* m_pCallBack;
public:
	CStatic m_ImageStatic;
	CButton m_btnCheckLocked;
	CButton m_btnCheckCellPhone;
};


