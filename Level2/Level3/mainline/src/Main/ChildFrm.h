// ChildFrm.h : interface of the CChildFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDFRM_H__5D301069_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
#define AFX_CHILDFRM_H__5D301069_30BE_11D4_92F6_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "IRenderFrame.h"
#include "SelectionMap.h"
#include "SplitterWndEx.h"



class C3DView;
class C3DCamera;
struct CViewDesc;
class CTermPlanDoc;
class IHyperCam;
class CPaxDispProps;
class CResourceDispProps;
class CSinglePaxTypeFlow;
class CAircraftDispProps;
class CTextureResource;
class CglShapeResource;
class CAirside3D;


class CChildFrame : public IRenderFrame
{
	DECLARE_DYNCREATE(CChildFrame)
public:
	CChildFrame();
	virtual ~CChildFrame();
	
friend class C3DView;

	CAirside3D  * GetAirside3D(){ return m_pAirside3D; }
	SelectableList& GetSelectableList(){return m_vSelected;}
	CTextureResource * GetTextureResoure(){ return m_pTextureResource; }
	CTextureResource * GetAirlineTextures(){ return m_pAirlineTextures; }
	CglShapeResource * GetShapeResource(){ return m_pShapeResource; }
	CglShapeResource * GetAirsideShapeResource(){ return m_pAirsideShapeResource; }
// Attributes
protected:
	IHyperCam* m_pHyperCam;
	BOOL m_bRecord;
	std::vector<GLuint> m_vPaxShapeDLIDs;
	std::vector<GLuint> m_vResShapeDLIDs;
	std::vector<GLuint> m_vRwyTexIDs;
	std::vector<GLuint> m_vAircraftShapeDLIDs;
	std::vector<GLuint> m_vAllAircraftShapeDLIDs;

	std::vector<GLuint> m_vAllTexIDs;//add by Ben in 2005-8.18,to record all the textures id

	GLuint m_nDefaultPaxShapeDLID;
	GLuint m_nOverlapPaxShapeDLID;
	GLuint m_nDefaultACShapeDLID;
	GLuint m_nOverlapACShapeDLID;
	GLuint m_nCompassDispListId;
	mutable GLuint m_nLogoTexID;
	GLuint m_nMSTexId;
	GLuint m_nRampTexId;
	GLuint m_nFlowDLID;
	GLuint m_nSelectionDLID; //red sqaure
	GLuint m_nSphereDLID;
	GLuint m_nConeDLID;
	GLuint m_nDiskDLID;
	CSplitterWndEx m_wndSplitter;
	std::vector<C3DCamera*> m_vWorkingCameras;
	BOOL m_bInfoActive;
	BOOL m_bInLoadingModel;
	float m_fLoadingPercent;


	//CMovieWriter* m_pMovieWriter;
	//UINT m_nRecordTimerID;
	//UINT m_nRecordFrameIdx;
	//CRect m_rcRecord;
	CTextureResource * m_pTextureResource;
	CTextureResource * m_pAirlineTextures;
	CglShapeResource * m_pShapeResource; 
	CglShapeResource * m_pAirsideShapeResource;
	
	CAirside3D * m_pAirside3D;       // Airside View to 3DView

	SelectableList m_vSelected;

public:
	void Select(Selectable * pSel);
	void SelectObject(int objID);
	void UnSelectAll();
	void UnSelectAllExceptObject();

	void StorePanInfo();	//store pane info (num rows, num cols, sizes)



// Operations
public:
	void GenPaxShapeDLIDs(CPaxDispProps* pPDP);
	void GenResShapeDLIDs(CResourceDispProps* pRDP);
	void GenAircraftShapeDLIDs(CAircraftDispProps* pADP);
	void GenGlobalTextures();	//generates texture objects for MS
	void GenGlobalDLs();		//generates display lists (compass)
	void UpdateFlowGeometry(const CSinglePaxTypeFlow* _pSPTF,const CSinglePaxTypeFlow* _pSPTFForGateAssaign);
	GLuint SphereDLID() const { return m_nSphereDLID; }
	GLuint ConeDLID() const { return m_nConeDLID; }
	GLuint DiskDLID() const { return m_nDiskDLID; }
	GLuint MSTextureID() const { return m_nMSTexId; }
	GLuint LogoTexID() const;
	GLuint RampTextureID() const { return m_nRampTexId; }
	GLuint FlowDLID() const { return m_nFlowDLID; }
	GLuint CompassDLID() const { return m_nCompassDispListId; }
	GLuint DefaultPaxShapeDLID() const { return m_nDefaultPaxShapeDLID; }
	GLuint OverlapPaxShapeDLID() const { return m_nOverlapPaxShapeDLID; }
	GLuint DefaultACShapeDLID() const { return m_nDefaultACShapeDLID; }
	GLuint OverlapACShapeDLID() const { return m_nOverlapACShapeDLID; }
	GLuint SelectionDLID() const { return m_nSelectionDLID; }
	const std::vector<GLuint>& PaxShapeDLIDs() const { return m_vPaxShapeDLIDs; }
	const std::vector<GLuint>& ResShapeDLIDs() const { return m_vResShapeDLIDs; }
	const std::vector<GLuint>& RunwayTexIDs() const { return m_vRwyTexIDs; }
	//const std::vector<GLuint>& AircraftShapeDLIDs() const { return m_vAircraftShapeDLIDs; }
	const std::vector<GLuint>& AircraftShapeDLIDs() const { return m_vAllAircraftShapeDLIDs; }

	const std::vector<GLuint>& AllTexIDs() const { return  m_vAllTexIDs; }


	C3DView* GetPane(int row, int col);
	C3DView* GetActivePane(int* _pRow=NULL, int* _pCol=NULL);
	BOOL IsChildPane(CWnd* _pWnd, int& _row, int& _col) { return m_wndSplitter.IsChildPane(_pWnd,&_row,&_col); }
	BOOL SplitRow(int _cyBefore);
	BOOL SplitColumn(int _cxBefore);
	BOOL SplitRow(); //splits evenly in to 2 rows.  if already 2 rows, does nothing
	BOOL SplitColumn(); //splits evenly in to 2 columns.  if already 2 columns, does nothing
	BOOL UnSplitRow();
	BOOL UnSplitColumn();
	void RefreshSplitBars() { m_wndSplitter.RefreshSplitBars(); }
	CTermPlanDoc* GetDocument();
	void SetPaneInfo(CViewDesc& vd);

 	C3DCamera* GetWorkingCamera(int row, int col) { return m_vWorkingCameras[col+2*row]; }

	void SaveWorkingViews(CViewDesc* pvd);
	void LoadWorkingViews(CViewDesc* pvd);

	BOOL IsInfoBtnActive()const { return m_bInfoActive; }
	void ToggleInfoBtnActive() { m_bInfoActive = !m_bInfoActive; }
	
	BOOL IsLoadingModel()const{ return m_bInLoadingModel; }
	float GetLoadingPercent()const{ return m_fLoadingPercent;	}

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//afx_msg void OnAnimationRecord();
	//afx_msg void OnUpdateAnimationRecord(CCmdUI* pCmdUI);
	//afx_msg void OnAnimationRecsettings();
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdate3dMouseMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);
	afx_msg void OnViewSaveNamedView();
	afx_msg void OnViewToggle2D3D();
	afx_msg void OnSplitHorizontal();
	afx_msg void OnSplitVertical();
	afx_msg void OnUnSplit();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDFRM_H__5D301069_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
