#ifndef __LandsideWalkwayPropDlgImpl_H__
#define __LandsideWalkwayPropDlgImpl_H__

#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"

class LandsideWalkway;
class LandsideWalkwayPropDlgImpl : public ILayoutObjectPropDlgImpl
{
protected:
	HTREEITEM m_hPath;
	HTREEITEM m_hWidth;
	HTREEITEM m_defaultWidth;
	std::vector<HTREEITEM> m_widthItems;
	std::vector<HTREEITEM> m_hightItems;
public:
	LandsideWalkwayPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString			getTitle();
	virtual void			LoadTree();
	virtual int getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_WALKWAY; }
	virtual FallbackReason	GetFallBackReason();	

	virtual void			OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void			OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void			DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL			OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );
	LandsideWalkway*		getWalkway();

private:
	void					LoadSubItemWalkwayPath(HTREEITEM& item);

protected:
	void					OnEditWdith(HTREEITEM hItem);
	void					OnEditDefaultWidth(HTREEITEM hItem);
	void					OnEditHight(HTREEITEM hItem);
	
};

#endif