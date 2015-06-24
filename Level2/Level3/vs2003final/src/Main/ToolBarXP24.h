// ToolBarXP24.h: interface for the CToolBarXP24 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARXP24_H__91FEAFDF_5B92_4B24_B8B5_BA6D3D7BAFDB__INCLUDED_)
#define AFX_TOOLBARXP24_H__91FEAFDF_5B92_4B24_B8B5_BA6D3D7BAFDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XPSTYLE\ToolbarXP.h"

class CToolBarXP24 : public CToolBarXP  
{
public:
	CToolBarXP24();
	virtual ~CToolBarXP24();
protected:
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CToolBarXP24)
    //}}AFX_VIRTUAL

    // Message Handlers
protected:
    //{{AFX_MSG(CToolBarXP24)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP();
    DECLARE_DYNAMIC(CToolBarXP24);

};

#endif // !defined(AFX_TOOLBARXP24_H__91FEAFDF_5B92_4B24_B8B5_BA6D3D7BAFDB__INCLUDED_)
