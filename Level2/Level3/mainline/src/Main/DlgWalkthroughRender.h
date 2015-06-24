#pragma once
// DlgWalkthroughRender.h : header file
//

// this class is the new version of CDlgWalkthrough for new render system
// 

class CTermPlanDoc;
class CRender3DView;

class CMovie;
class CWalkthroughs;
class CWalkthrough;

#include <Inputs/Walkthrough.h>
#include "../MFCExControl/XTResizeDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgWalkthroughRender dialog

class CDlgWalkthroughRender : public CXTResizeDialog
{
public:
	CDlgWalkthroughRender(CTermPlanDoc* pTPDoc, CWnd* pParent = NULL);   // standard constructor

	enum { IDD = IDD_DIALOG_WALKTHROUGH };

	void SelectMob(CWalkthrough::WalkthroughMobType mobType, int nMobID, const CString& sDesc);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();

	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeComboViewpoint();
	afx_msg void OnDatetimepickerChangeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDatetimepickerChangeEnd(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddMovie();
	afx_msg void OnDeleteMovie();
	afx_msg void OnRecordMovie();
	afx_msg void OnPreviewMovie();
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

protected:

	typedef std::pair<CWalkthrough::WalkthroughMobType, int/*MobID*/> SelMobID;
	SelMobID m_selMobID;

	struct WalkthroughListData 
	{
		WalkthroughListData(CWalkthrough::WalkthroughMobType _mobType = CWalkthrough::Mob_None, CWalkthrough* _pWalkthrough = NULL)
			: mobType(_mobType)
			, pWalkthrough(_pWalkthrough)
		{

		}

		CWalkthrough::WalkthroughMobType mobType;
		CWalkthrough* pWalkthrough;
	};
	typedef std::vector<WalkthroughListData> WalkthroughListDataPool;
	WalkthroughListDataPool m_vListData;

	void UpdateList();
	void AddWalkthroughs2List(CWalkthroughs* walkthroughs, CWalkthrough::WalkthroughMobType mobType);
	void UpdatePAXData();
	CWalkthroughs* GetWalkthroughsByMobType(CWalkthrough::WalkthroughMobType mobType) const;
	WalkthroughListData* GetSelWalkthroughData();
	void ConfigControlsBySelMobType( CWalkthrough::WalkthroughMobType mobType );
	void EnableControls(BOOL bEnable);
	void SetDataFromID(SelMobID selMobID);

	CStatic m_staticPax;
	CEdit	m_editPax;
	CListCtrl	m_lstWalkthrough;
	CComboBox	m_cmbViewpoint;
	CToolBar m_ToolBar;

	COleDateTime	m_endTime;
	COleDateTime	m_startTime;

	CTermPlanDoc* m_pDoc;
	CRender3DView* m_pView;
	CWnd* m_pParent;

	BOOL m_bNewMovie;
	int m_nSelWalkthrough;
    BOOL m_bIsSetData;
	long m_nMinTime;
	long m_nMaxTime;

};

