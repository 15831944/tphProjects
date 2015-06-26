#pragma once
#include "MFCExControlAPI.h"
// CSimpleToolTipListBox

class MFCEXCONTROL_API CSimpleToolTipListBox : public CListBox
{
	DECLARE_DYNAMIC(CSimpleToolTipListBox)

public:
	CSimpleToolTipListBox();
	virtual ~CSimpleToolTipListBox();


public:
	int OnToolHitTest(CPoint point, TOOLINFO * pTI) const;
	BOOL OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult );


protected:
	virtual void PreSubclassWindow();
	DECLARE_MESSAGE_MAP()
};


