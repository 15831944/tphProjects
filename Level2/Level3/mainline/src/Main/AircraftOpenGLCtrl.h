#pragma once
//#include "../Main/Onboard/3DSceneView.h"

///#include <Onboard3d/AircraftLayouScene.h>
#include "../InputOnboard/Door.h"
#define CREW_DRAW				WM_USER + 1001
// CAircraftOpenGLCtrl
class CAircaftLayOut;
class CAircraftLayoutEditContext;
class CDeck;
class CDeckManager;
class CSeatDataSet;
class CDoor;
class CDoorDataSet;
class CAircraftOpenGLCtrl;
class CCrewPosition;
class CabinCrewGeneration;
class CAircraftLayoutCtrl: public CWnd
{
	DECLARE_DYNAMIC(CAircraftLayoutCtrl)

public:
	CAircraftLayoutCtrl(CabinCrewGeneration* CrewPaxList,BOOL bInit,int nFlightID);
	virtual ~CAircraftLayoutCtrl();

	void UpdateDeckLayoutOutLine();
	void CreateDeckLayout2D(CRect rect, CWnd *parent);

	void addItem(CAircraftOpenGLCtrl* openGLCtrl);
	const CAircraftOpenGLCtrl* findItem(CDeck* pDeck);

	void OnUpdate();
protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	std::vector<CAircraftOpenGLCtrl*>m_vGLCtrl;
	CDeckManager* m_pDeckManager;
	CAircaftLayOut* m_layout;
	BOOL m_bInit;
	CabinCrewGeneration* m_CrewPaxList;
};

class CAircraftOpenGLCtrl : public CView
{
	DECLARE_DYNAMIC(CAircraftOpenGLCtrl)

public:
	CAircraftOpenGLCtrl(CDeckManager* pDeckManage,CDeck* pDeck,CAircaftLayOut* _layout,CabinCrewGeneration* CrewPaxList,BOOL bInit);
	virtual ~CAircraftOpenGLCtrl();

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg void OnRemovePostion();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC );

	virtual void OnDraw(CDC* pDC);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

public:
	void Create(CRect rect, CWnd *parent);
	void SetupViewport(int cx,int cy);
	int SetupScene();
	CAircraftLayoutEditContext* GetEditContext()const;
	ARCVector3 GetWorldMousePos(const CPoint& pt);
	void RenderSecen();
	BOOL SetupPixelFormat(HDC hDC, DWORD dwFlags);
	CSeatDataSet* getSeatSet(CAircaftLayOut* _layout);
	CDoorDataSet& getDoorSet(CAircaftLayOut* _layout);
	LRESULT DrawCrewPostion(WPARAM wParam, LPARAM lParam);
	CDeck* getDeck()const{return m_pDeck;}
	float getDeckScale(int cx,int cy);
private:
	void RenderText();
	void RenderDeck(CDeck* pDeck);
	void RenderSeat(CSeat* pSeat);
	void RenderDoor(CDoor* pDoor);
	void DrawPosition(CCrewPosition* pCrewPosition);
	void SelectScene(int x,int y);
private:
	HGLRC m_hRC;	//opengl RC
	CDC* m_pDC;		//windows DC
	int m_iPixelFormat;	//pixel format used
	double m_dAspect;	//aspect ratio of view
	CDeck* m_pDeck;
	CAircaftLayOut* m_layout;
	CSeatDataSet* m_pSeatSet;
	CDoorDataSet* m_DoorSet;
	CPoint	m_ptFrom;	// screen point where mouse button was last clicked
	ARCVector3 m_ptMousePos;
	ARCVector3 m_ptMouseWorldPos;
	BOOL m_bLeftButtonDown;
	CCrewPosition* m_pCrewPostion;
	CCrewPosition* m_pDragShape;
	COleDropTarget m_oleDropTarget;	//to support drag & drop of shapes
	BOOL m_bInit;
	CabinCrewGeneration* m_CrewPaxList;
	CDeckManager* m_pDeckManager;
};


