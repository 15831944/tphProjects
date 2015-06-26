#pragma once
#include "CoolTree.h"

class MFCEXCONTROL_API CCoolTreeKeepHighlight : public CCoolTree
{
    DECLARE_DYNAMIC(CCoolTreeKeepHighlight)

public:
    CCoolTreeKeepHighlight();
    virtual ~CCoolTreeKeepHighlight();

protected:
    DECLARE_MESSAGE_MAP()
    BOOL OnEraseBkgnd(CDC* pDC);
    virtual void PreSubclassWindow();
    afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};


