#pragma once

#include "..\Inputs\PipeDataSet.h"
#include "TermPlanDoc.h"
#include "mschart.h"
#include <vector>
#include "..\MFCExControl\CoolTree.h"

// CDlgProcCongArea dialog

class CDlgProcCongArea : public CDialog
{
	DECLARE_DYNAMIC(CDlgProcCongArea)

public:
	CDlgProcCongArea(Terminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgProcCongArea();

// Dialog Data
	enum { IDD = IDD_DIALOG_PROCCONGAREA };
	double m_dLambda;
	double m_dTrigger;
	CMSChart1 m_msChart;
	CCoolTree m_treeTargetAreas;
	CCoolTree m_treeTargetPipes;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedButtonAdvanced();
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

protected:
	CAreas* m_pAreas;
	CPipeDataSet* m_pPipeDataSet;
	CTermPlanDoc* m_pDoc;
	std::vector<int>* m_pAreasIndex;
	std::vector<int>* m_pPipesIndex;

	HTREEITEM m_hAllAreas;
	HTREEITEM m_hAllPipes;

	void InitializeChart();
	void InitTreeAreas();
	void InitTreePipes();

	void InitInputData();
	void SaveInputData();

	void UpdateChart();

public:
	const std::vector<int>* GetAreasIndex() const { return m_pAreasIndex; }
	const std::vector<int>* GetPipesIndex() const { return m_pPipesIndex; }

	void GetTreeData();
	afx_msg void OnBnClickedOk();
};
