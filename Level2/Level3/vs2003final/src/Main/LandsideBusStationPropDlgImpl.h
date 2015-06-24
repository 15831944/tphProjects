#ifndef __LandsideBusStationPropDlgImpl_H__
#define __LandsideBusStationPropDlgImpl_H__

#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"

class LandsideBusStation;
class LandsideBusStationPropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideBusStationPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	CString					getTitle();
	void					LoadTree();
	virtual int				getBitmapResourceID() { return IDB_BITMAP_LANDSIDE_BUSSTATION; }
	virtual FallbackReason	GetFallBackReason();	

	void					OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	void					OnDoubleClickPropTree(HTREEITEM hTreeItem);

	void					DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	BOOL					OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );
	LandsideBusStation*		getBusStation();
protected:
	HTREEITEM m_hPath;
	HTREEITEM m_hStretch;
	HTREEITEM m_hWaitingPath;
	HTREEITEM m_hLaneFrom;
	HTREEITEM m_hLaneTo;
	HTREEITEM m_hEmbedParkinglot;

	void OnEditLaneFrom(HTREEITEM hItem);
	void OnEditLaneTo(HTREEITEM hItem);

private:
	std::vector<LandsideFacilityLayoutObject*> m_lstParkinglot;
};

#endif