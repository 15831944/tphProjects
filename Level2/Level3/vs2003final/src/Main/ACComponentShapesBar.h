// ACComponentShapesBar.h: interface for the CACComponentShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ShapesBarBase.h"

class CAircraftComponentModelDatabase;
class CACComponentShapesBar : public CShapesBarBase  
{
// Construction
public:
	CACComponentShapesBar();
	virtual ~CACComponentShapesBar();

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation

	void ReloadContent(CAircraftComponentModelDatabase* pDB);

	// Generated message map functions
protected:
	//{{AFX_MSG(CACComponentShapesBar)
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
