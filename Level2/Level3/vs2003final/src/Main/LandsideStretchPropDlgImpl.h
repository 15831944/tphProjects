#pragma once

#include "LayoutObjectPropDlgImpl.h"

class LandsideStretch;
class ControlPath;

class LandsideStretchPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideStretchPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString getTitle();
	virtual void LoadTree();
	virtual getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_STRETCH; }
	virtual FallbackReason GetFallBackReason();	

	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void OnControlPathReverse();
	void DoFallBackFinished( WPARAM wParam, LPARAM lParam );

	LandsideStretch* getStretch();


	bool OnOK( CString& errorMsg );

protected:
	HTREEITEM m_hCtrlPoints;
	HTREEITEM m_hVerticalProfile;
	HTREEITEM m_hLaneNumber;
	HTREEITEM m_hLaneWidth;
	HTREEITEM m_hType;
	HTREEITEM m_hSideWalkWidth;
	virtual void DonePickPoints(const CPath2008& pickpoints );

	HTREEITEM m_hStretchSort;
	HTREEITEM m_hSortDirect;
	HTREEITEM m_hSortCircle;

	void OnDefineNumber(HTREEITEM hTreeItem);
	void OnDefineWidth(HTREEITEM hTreeItem);
	void OnDefineType(HTREEITEM hTreeItem);
	void OnDefineSideWalk(HTREEITEM hTreeItem);
	BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};