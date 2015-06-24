#pragma once
#include "Inputs/StandAdjacencyConstraint.h"

// DlgAssignmentConstraintSpec dialog

class DlgAssignmentConstraintSpec : public CDialog
{
	DECLARE_DYNAMIC(DlgAssignmentConstraintSpec)

public:
	DlgAssignmentConstraintSpec(StandAdjacencyConstraint* adjaconstrain,CWnd* pParent = NULL);   // standard constructor
	 ~DlgAssignmentConstraintSpec();

// Dialog Data
	enum { IDD = IDD_DIALOG_ASSCONSPE };

private:
	int m_RadioName;
	StandAdjacencyConstraint* m_standAdjConstraint;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnContinue();
	virtual void OnCancel();
	

	DECLARE_MESSAGE_MAP()
};
