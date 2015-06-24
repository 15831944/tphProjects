#pragma once
#include "layoutobjectpropdlgimpl.h"
#include "TreeCtrlItemInDlg.h"
class LandsideDecisionPoint;

class LandsideDecisionPointPropDlgImpl :
	public ILayoutObjectPropDlgImpl
{
public:
	LandsideDecisionPointPropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg);
	~LandsideDecisionPointPropDlgImpl(void);

	virtual CString			getTitle();
	virtual void			LoadTree();
	virtual int				getBitmapResourceID();
	virtual FallbackReason	GetFallBackReason();	

	virtual void			OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void			OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void			DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL			OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );


	LandsideDecisionPoint* getDecisionPoint();

	virtual void InitUnitPiece(CUnitPiece* unit);
protected:
	HTREEITEM m_hPos;
	HTREEITEM m_hStretch;
	HTREEITEM m_hDist;
};
