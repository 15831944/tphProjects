#pragma once

#include "3DObjModelEditFrame.h"
// CACModelEditFrame frame with splitter

class CACModelEditFrame : public C3DObjModelEditFrame
{
	DECLARE_DYNCREATE(CACModelEditFrame)
protected:
	CACModelEditFrame();           // protected constructor used by dynamic creation
	virtual ~CACModelEditFrame();

	virtual CRuntimeClass* Get3DViewClass() const;
	virtual C3DObjModelEditContext* CreateEditContext() const;

	CSplitterWnd m_wndSplitter;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

};


