#pragma once


class CWnd;
class CClientDC;
class C3DSceneBase;
#include <map>
#include "RenderEngineAPI.h"

namespace Ogre
{
	class Viewport;
	class Camera;
	class MovableObject;
	class Vector2;
	class Vector3;
	class Ray;
}

enum VPAlignment
{
	VPA_Left,
	VPA_Right,
	VPA_Bottom,
	VPA_Top,
	VPA_Center,
};
class VPAlignPos
{
public:
	VPAlignPos(){ x=VPA_Left; y= VPA_Top; }
	VPAlignPos(VPAlignment _x, VPAlignment _y) { x= _x; y = _y; }
	VPAlignment x;
	VPAlignment y;
};

class RenderWnd;
class RENDERENGINE_API ViewPort
{
public:
	ViewPort(){  mvp = NULL; };
	ViewPort(Ogre::Viewport* pvp){ mvp = pvp;  }

	void SetClear( DWORD color, bool bClearColor = true, bool bClearDepth = true, bool bStencile = true);	//set clear color
	void SetCamera(Ogre::Camera* pcam); //set camera
	Ogre::MovableObject* PickObject( const CPoint& winPt); //pick object in the viewport
	bool IsPointIn(const CPoint& pt)const; //is the point in this view port

	int GetHeight()const;
	bool IsValid()const{ return mvp!=NULL; }
	Ogre::Vector2 GetRealPos(const CPoint& winPt)const; //get the point pos in viewport [0 -1]
	Ogre::Camera* getCamera()const;
	Ogre::Ray GetWorldMouseRay(const CPoint& winPt)const;//get mouse ray
	void SetVisibleFlag(const Ogre::uint32& flag);

	void addVisibilityFlags( const Ogre::uint32& flags );

	void removeVisibilityFlags( const Ogre::uint32& flags );

	Ogre::Viewport* mvp;
	//RenderWnd* mpParent;

	Ogre::SceneManager* getScene()const;
protected:
	//Ogre::Vector2 GetViewportPos(const CPoint& winPos)const; //get  the real position in viewport normalized in [0 1]
	bool GetWorldPosition(const CPoint& winPt, const Ogre::Plane& worldPlan, Ogre::Vector3& outWorldPos);
};




//represent a render wnd
class RENDERENGINE_API RenderWnd
{
public:
	RenderWnd(void);
	~RenderWnd(void);
	//add a new view port, from the scene camera, clear : whether clear color 
	int AddViewport(Ogre::Camera* pcam , bool bShowOverlay = false);


	//get world pos from the view port 
	ViewPort GetViewport(int idx)const;
	int GetViewportCount()const;

//normal window operations
	bool Create( HWND hwnd );
	bool IsValid()const{ return m_Window!=NULL; }
	void Destory();
	void OnSize();
	void Render();



	//Ogre::Vector2 GetViewportPos( const CPoint& winPos, Ogre::Viewport* pVp ) const; //normalize to [0,1]
	Ogre::Vector2 GetRealPos(const CPoint& winPos)const;
	void SetViewportDim(int vpIdx , const CRect& rect, const VPAlignPos& alg = VPAlignPos(), bool bFix = false); //set dimension 


	void RegFixSizeViewport(int vpIdx, const CRect& rect, const VPAlignPos& alg);
	void UnRegFixSizeViewport(int vpIdx);

	Ogre::RenderWindow* m_Window;	
	void SetActiveViewport(int idx){ m_nActiveVP = idx; };
	ViewPort GetActiveViewport()const{ return GetViewport(m_nActiveVP); }
	
protected:
	//fix windows size
	typedef std::map<int,std::pair<CRect,VPAlignPos> > FixSizeVPList; 
	FixSizeVPList mFixSizsVps;
	int m_nActiveVP;  //index of active viewport
};


 