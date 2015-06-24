#pragma once
#include "layoutobjectpropdlgimpl.h"
#include "TreeCtrlItemInDlg.h"

class LandsideStretchSegment;

class LandsideStretchSegmentPropDlgImpl :
	public ILayoutObjectPropDlgImpl
{
public:
	LandsideStretchSegmentPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );

	virtual CString getTitle();
	virtual void LoadTree();
	virtual int getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_CURBSIDE; }
	virtual FallbackReason GetFallBackReason();	

	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);;

	bool OnOK( CString& errorMsg );

private:
	LandsideStretchSegment* getStretchSegment();

	void OnEditLength(HTREEITEM hItem);
	void OnEditLaneFrom(HTREEITEM hItem);
	void OnEditLaneTo(HTREEITEM hItem);

private:
	HTREEITEM m_hLinkStretch;
	HTREEITEM m_hLength;

	HTREEITEM m_hLaneFrom;
	HTREEITEM m_hLaneTo;
};
