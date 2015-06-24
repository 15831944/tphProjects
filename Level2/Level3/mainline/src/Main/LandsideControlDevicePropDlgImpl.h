#pragma once

#include "LayoutObjectPropDlgImpl.h"
#include "TreeCtrlItemInDlg.h"
class LandsideControlDevice;
class LandsideControlDevicePropDlgImpl : public ILayoutObjectPropDlgImpl
{
public:
	LandsideControlDevicePropDlgImpl(LandsideFacilityLayoutObject* pObj,CARCLayoutObjectDlg* pDlg );
	virtual CString			getTitle();
	virtual void			LoadTree();
	virtual int				getBitmapResourceID();
	virtual FallbackReason	GetFallBackReason();	

	virtual void			OnContextMenu(CMenu& menuPopup, CMenu *&pMenu);	
	virtual void			OnDoubleClickPropTree(HTREEITEM hTreeItem);

	virtual void			DoFallBackFinished( WPARAM wParam, LPARAM lPara);
	virtual BOOL			OnDefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	bool					OnOK( CString& errorMsg );


	LandsideControlDevice* getDevice();

	virtual void InitUnitPiece(CUnitPiece* unit);
protected:
	HTREEITEM m_hPath;
	HTREEITEM m_hStretch;
	HTREEITEM m_hWidth;
	HTREEITEM m_hType;
	HTREEITEM m_hIntersection;
	HTREEITEM m_hGroup;
	HTREEITEM m_hSpdLimit;

};

