#pragma once

#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"

class LandsideCrosswalk;
class LandsideIntersectionNode;
class LandsideCrosswalkPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideCrosswalkPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString			getTitle();
	virtual void			LoadTree();
	virtual int				getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_CROSSWALK; }
	virtual FallbackReason	GetFallBackReason();	

	virtual void			OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void			OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void			DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL			OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );
	LandsideCrosswalk*		getCrosswalk();
protected:
	HTREEITEM m_hPath;
	HTREEITEM m_hStretch;
	HTREEITEM m_hWidth;
	HTREEITEM m_hType;
	HTREEITEM m_hIntersection;
	HTREEITEM m_hGroup;

	std::vector<LandsideIntersectionNode *> m_vIntersection;

	void OnEditWdith(HTREEITEM hItem);
	void OnSelectType(HTREEITEM hItem);
	void OnSelectIntersection(HTREEITEM hItem);
	void OnSelectGroup(HTREEITEM hItem);
};

