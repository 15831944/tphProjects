#pragma once

#include "CoolTree.h"
// CARCTreeCtrl

class MFCEXCONTROL_API CARCTreeCtrl : public CCoolTree
{
	DECLARE_DYNAMIC(CARCTreeCtrl)

public:
	CARCTreeCtrl();
	virtual ~CARCTreeCtrl();

protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
};


