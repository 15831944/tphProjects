#pragma once
#include "..\InputOnBoard\CarryonVolumeList.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../inputs/in_term.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/ProbDistManager.h"
#include "TermPlanDoc.h"
#include "../Inputs/probab.h"
#include "DlgProbDist.h"
#include "../InputOnboard/Carryon.h"


// CDlgCarryonVolume dialog



class CDlgCarryonVolume : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CDlgCarryonVolume)

public:
	CDlgCarryonVolume(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgCarryonVolume();

// Dialog Data
	enum { IDD = IDD_DIALOG_MANTENANCE };

	void InitToolBar();
	void InitList();
	void SetListContent();
	CProbDistEntry* GetDefaultPDEntry();
	void OnAddCarryonVolume();
	void OnDelCarryonVolume();
	InputTerminal* GetInputTerminal();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSelComboBox(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnItemChangeList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSave();

	DECLARE_MESSAGE_MAP()
private:
	//char* ItemName[CARRYON_NUM];
// 	std::vector<CString> ItemName;
	CCarryonVolumeList m_CarryonVolumeList;
// 	CCarryonOwnership m_CarryonOwnership;

	CToolBar m_toolbar;
	CListCtrlEx m_listCarryonVolume;
};
