// ACFurnishingShapesBar.h: interface for the CACFurnishingShapesBar class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ShapesBarBase.h"

class CAircraftFurnishingSectionManager;
class CACFurnishingShapesBar : public CShapesBarBase  
{
// Construction
public:
	CACFurnishingShapesBar();
	virtual ~CACFurnishingShapesBar();

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProjectSelectBar)
	//}}AFX_VIRTUAL

// Implementation

	void ReloadContent(CAircraftFurnishingSectionManager* pManager);

	// Generated message map functions
protected:
	//{{AFX_MSG(CACFurnishingShapesBar)
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
