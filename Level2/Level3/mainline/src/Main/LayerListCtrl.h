#pragma once
#include <common/CADInfo.h>
/////////////////////////////////////////////////////////////////////////////
// CLayerListCtrl window
#include <OpenDWGToolKit/CADFileLayer.h>



class CLayerListCtrl : public CListCtrl
{
// Construction
public:
	CLayerListCtrl();

// Attributes
public:

protected:
	struct _layerInfo {
		_layerInfo(CCADLayer* pLayer);
		CCADLayer* pFloorLayer;
		CString sName;
		COLORREF cColor;
		BOOL bIsOn;
	};
	
	HBITMAP m_hBitmapCheck;
	CSize m_sizeCheck;

// Operations
public:
	void PopulateLayerList(CCADFileContent::CFloorLayerList& vList);
	BOOL UpdateFloorData(); //return TRUE if the data has changed

	void PopulateLayerList(const LayerInfoList& list);
	void GetLayersData(LayerInfoList& list);

protected:
	void FreeItemMemory();
	void GetColorBoxRect(CRect& r);
	void GetVisibleRect(CRect& r);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayerListCtrl)
	virtual void DrawItem( LPDRAWITEMSTRUCT lpdis);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLayerListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLayerListCtrl)
	afx_msg void OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

