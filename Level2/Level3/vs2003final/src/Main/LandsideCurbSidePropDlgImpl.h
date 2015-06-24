#pragma once
#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"

//////////////////////////////////////////////////////////////////////////
class LandsideCurbSide;
class LandsideCurbSidePropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideCurbSidePropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString getTitle();
	virtual void LoadTree();
	virtual int getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_CURBSIDE; }
	virtual FallbackReason GetFallBackReason();	

	virtual void OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void OnDoubleClickPropTree(HTREEITEM hTreeItem);
	
	virtual void DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void LoadTreeSubItemPickArea(HTREEITEM preItem,const CPath2008& path );

	virtual void GetFallBackReasonData(FallbackData& data);
	bool OnOK( CString& errorMsg );

protected:
	HTREEITEM m_hFloor;
	HTREEITEM m_hPickArea;
	
	HTREEITEM m_hLinkStretch;
	HTREEITEM m_hLength;

	HTREEITEM m_hLaneFrom;
	HTREEITEM m_hLaneTo;
	HTREEITEM m_hSpotLength;

	HTREEITEM m_hDecisionLine;

	LandsideCurbSide* getCurbside();

	void OnComboFloors(HTREEITEM hItem);
	void OnEditLength(HTREEITEM hItem);
	void OnEditLaneFrom(HTREEITEM hItem);
	void OnEditLaneTo(HTREEITEM hItem);
	void OnEditSpotlength(HTREEITEM hItem);
	
};