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
    virtual void PreSubclassWindow();
    afx_msg BOOL OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg BOOL OnLvnItemchanged(NMHDR *pNMHDR, LRESULT *pResult);
};


