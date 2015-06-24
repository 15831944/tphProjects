#pragma once
#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"

class LandsideRoundabout;
class TreeCtrlItemInDlg;
class StretchSide;

class StretchSideTreeItem : public TreeCtrlItemInDlg
{
public:
	DLG_TREEIREM_CONVERSION_DEF(StretchSideTreeItem,TreeCtrlItemInDlg)
	int getItemIdx()const;
	void setStretchSide(const StretchSide& s,int idx);	
};
class LandsideRoundaboutPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:

	LandsideRoundaboutPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString			getTitle();
	virtual void			LoadTree();
	virtual int				getBitmapResourceID();
	virtual FallbackReason	GetFallBackReason();	

	virtual void			OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void			OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void			DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL			OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//bool					OnOK( CString& errorMsg );
	virtual void OnPropDelete();
	LandsideRoundabout* getRoundabout();
protected:
	HTREEITEM m_hPosition;
	HTREEITEM m_hInRadius;
	HTREEITEM m_hOutRadius;
	HTREEITEM m_hLaneNum;
	HTREEITEM m_hClockwise;
	HTREEITEM m_hlinkStetches;

	void loadLinkStretch(TreeCtrlItemInDlg& parentItem);
};
