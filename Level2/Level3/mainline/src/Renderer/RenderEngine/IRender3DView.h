#pragma once
#include "AfxHeader.h"
#include "commondata/3DViewCommon.h"
#include "RenderEngineAPI.h"
class CCameraData;

struct CPictureInfo;
class RENDERENGINE_API IRender3DView :
	public CView
{
	DECLARE_DYNCREATE(IRender3DView)
protected:
	DECLARE_MESSAGE_MAP()

public:
	IRender3DView(); 
	virtual void OnDraw(CDC* pDC);

	NS3DViewCommon::EMouseState m_eMouseState;
	LPARAM m_lFallbackParam;
	HWND m_hFallbackWnd;

	BOOL m_bAnimDirectionChanged;
	int m_nAnimDirection;

	virtual void OnNewMouseState(){}
	//virtual void SetWorkHeight(double d){}

	virtual CCameraData* GetCameraData() const { ASSERT(FALSE); return NULL; }
	virtual void SetCameraData(const CCameraData& cam){ ASSERT(FALSE); }

	//is busy rendering
	inline BOOL isBusy()const{ return m_bBusy; }
	inline void SetBusy(BOOL bBusy) {m_bBusy = bBusy;}

	virtual void UpdateFloorOverlay( int nLevelID, CPictureInfo& picInfo );

protected:
	BOOL m_bBusy;	//this flag is true while view is rendering
};
