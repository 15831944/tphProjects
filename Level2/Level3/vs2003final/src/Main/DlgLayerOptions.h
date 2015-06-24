#if !defined(AFX_DLGLAYEROPTIONS_H__170757F7_ECC7_4419_9DCF_2D5F4619F270__INCLUDED_)
#define AFX_DLGLAYEROPTIONS_H__170757F7_ECC7_4419_9DCF_2D5F4619F270__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgLayerOptions.h : header file
//

#include "LayerListCtrl.h"
#include "Common/BaseFloor.h"
/////////////////////////////////////////////////////////////////////////////
// CDlgLayerOptions dialog
class CCADLayer;
typedef std::vector<CCADLayer*> CFloorLayerList;
class CDlgLayerOptions : public CDialog
{
// Construction
public:
	CDlgLayerOptions(CFloorLayerList* pLayerList, CWnd* pParent = NULL);   // standard constructor
	BOOL IsLayerDirty() { return m_bLayersChanged; }

// Dialog Data
	//{{AFX_DATA(CDlgLayerOptions)
	enum { IDD = IDD_LAYEROPTIONS };
	CLayerListCtrl	m_lstLayers;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgLayerOptions)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CFloorLayerList* m_pFloorLayerList;
	BOOL m_bLayersChanged;
     CBaseFloor *floor;
	// Generated message map functions
	//{{AFX_MSG(CDlgLayerOptions)
	virtual BOOL OnInitDialog();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGLAYEROPTIONS_H__170757F7_ECC7_4419_9DCF_2D5F4619F270__INCLUDED_)
\
//////////////////////////////////////////////////////////////////////////
//layer option new
//////////////////////////////////////////////////////////////////////////

