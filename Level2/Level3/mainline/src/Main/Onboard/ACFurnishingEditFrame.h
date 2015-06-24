#pragma once

#include "3DObjModelEditFrame.h"
// CACFurnishingEditFrame frame with splitter
class CAircraftFurnishingSectionManager;
class CACFurnishingEditFrame : public C3DObjModelEditFrame
{
	DECLARE_DYNCREATE(CACFurnishingEditFrame)
protected:
	CACFurnishingEditFrame();           // protected constructor used by dynamic creation
	virtual ~CACFurnishingEditFrame();

	virtual CRuntimeClass* Get3DViewClass() const;
	virtual C3DObjModelEditContext* CreateEditContext() const;

	CSplitterWnd m_wndSplitter;

public:
	CAircraftFurnishingSectionManager* GetACFurnishingManager() const { return m_pACFurnishingManager; }
	void SetACFurnishingManager(CAircraftFurnishingSectionManager* val) { m_pACFurnishingManager = val; }
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	DECLARE_MESSAGE_MAP()

private:
	CAircraftFurnishingSectionManager* m_pACFurnishingManager;
};


