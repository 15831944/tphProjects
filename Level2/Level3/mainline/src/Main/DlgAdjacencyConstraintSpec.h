#pragma once
#include "../MFCExControl/ToolTipDialog.h"
#include "../MFCExControl/ListCtrlEx.h"
#include "../Inputs/AdjacencyConstraintSpec.h"
#include "TermPlanDoc.h"
#include "../AirsideGUI/UnitPiece.h"
#include "../MFCExControl/XTResizeDialog.h"
class InputTerminal;


// DlgAdjacencyConstraintSpec dialog

class DlgAdjacencyConstraintSpec : public CXTResizeDialog , public CUnitPiece
{
	DECLARE_DYNAMIC(DlgAdjacencyConstraintSpec)

public:
	DlgAdjacencyConstraintSpec(InputTerminal* pInTerm, AdjacencyConstraintSpecList* pAdjGateConstraints, int nProjectID,CWnd* pParent);
	virtual ~DlgAdjacencyConstraintSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_ADJACENCYCONSTRAINTASPEC };

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
 	virtual bool ConvertUnitFromDBtoGUI(void);
	virtual bool RefreshGUI(void);
 	virtual bool ConvertUnitFromGUItoDB(void);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNewItem();
	afx_msg void OnDeleteItem();
	afx_msg void OnOK();
	afx_msg void OnCancel();
	afx_msg void OnLvnItemchangedList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnDoubleClick(WPARAM wParam,  LPARAM lParam);

protected:
	void InitToolBar();
	void InitListControl();
	void InitListItem();
	CString GetProjPath();
	void AddConstraintItem(AdjacencyConstraintSpecDate& AdjConSpeItem);
	void SetListItemContent(int nIndex,AdjacencyConstraintSpecDate& AdjConSpeItem);

protected:
	int				 m_nProjectID;
	CStringList		 strList;
	CToolBar		 m_wndToolBar;
	CListCtrlEx		 m_wndListCtrl;
	InputTerminal*	 m_pInputTerminal;
	AdjacencyConstraintSpecList* m_pAdjConstraintSpec;

};
