#include "StdAfx.h"
#include ".\nonpaxrelativeposeditctrl.h"
#include <common\Grid.h>
#include <common\util.h>
#include <inputs\NonPaxRelativePosSpec.h>
#include <algorithm>
#include "XPStyle/NewMenu.h"
#include "resource.h"
using namespace Ogre;
IMPLEMENT_DYNCREATE(CNonPaxRelativePosEditCtrl,CWnd)

CNonPaxRelativePosEditCtrl::CNonPaxRelativePosEditCtrl(void)
{
	RegisterWndClass();
	mbSceneWndCreated = false;
	m_pNonPaxSpec = NULL;	
}

CNonPaxRelativePosEditCtrl::~CNonPaxRelativePosEditCtrl(void)
{

}
BEGIN_MESSAGE_MAP(CNonPaxRelativePosEditCtrl, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_NONPAXPICK_DELETE, OnNonpaxpickDelete)
	ON_COMMAND(ID_ROTATE_X, OnRotateX)
	ON_COMMAND(ID_ROTATE_Y, OnRotateY)
	ON_COMMAND(ID_ROTATE_Z, OnRotateZ)
END_MESSAGE_MAP()

BOOL CNonPaxRelativePosEditCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}


void CNonPaxRelativePosEditCtrl::OnDestroy()
{
	m_rdc.Destory();
	CWnd::OnDestroy();
	// TODO: Add your message handler code here
}

void CNonPaxRelativePosEditCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	mnSizeType = nType;
	mnSizex = cx;
	mnSizeY = cy;

	// TODO: Add your message handler code here
	m_rdc.OnSize();
}

void CNonPaxRelativePosEditCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CWnd::OnPaint() for painting messages	

	// TODO:  Add your specialized creation code here
	if(!m_rdc.IsValid())
	{
		CWaitCursor wc;
		m_rdc.Create(GetSafeHwnd());
		SetupViewport();
	}

	if(m_rdc.IsValid())
	{
		m_rdc.Render();
	}
	
}



BOOL CNonPaxRelativePosEditCtrl::RegisterWndClass()
{
	const static CString MYWNDCLASS = _T("NonPaxPosEditWnd");
	WNDCLASS windowclass;
	HINSTANCE hInst = AfxGetInstanceHandle();

	//Check weather the class is registered already

	if (!(::GetClassInfo(hInst, MYWNDCLASS, &windowclass)))
	{
		//If not then we have to register the new class

		windowclass.style = CS_DBLCLKS;// | CS_HREDRAW | CS_VREDRAW;

		windowclass.lpfnWndProc = ::DefWindowProc;
		windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
		windowclass.hInstance = hInst;
		windowclass.hIcon = NULL;
		windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		windowclass.hbrBackground = ::GetSysColorBrush(COLOR_WINDOW);
		windowclass.lpszMenuName = NULL;
		windowclass.lpszClassName = MYWNDCLASS;

		if (!AfxRegisterClass(&windowclass))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}
void CNonPaxRelativePosEditCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ViewPort vp = m_rdc.GetActiveViewport();
	if(!vp.IsValid())
		return;


	
	mpSelectShape = m_scene.GetSelectNode(	vp.PickObject(point) );
	
	if(mpSelectShape)
		mMouseState = _moveShape;

	CWnd::OnLButtonDown(nFlags, point);
}

void CNonPaxRelativePosEditCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	DoneEdit();
	
	m_lastMousePt = point;
	CWnd::OnLButtonUp(nFlags, point);
}



void CNonPaxRelativePosEditCtrl::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if(::GetAsyncKeyState(VK_MENU) < 0 )
	{
		return ;
	}

	if(mpSelectShape )
	{
		//down cast menu
		
		CPoint pt(point);
		ClientToScreen(&pt);		
		CNewMenu menu, *pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_NONPAX_PICK);
		pCtxMenu =DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pCtxMenu->SetMenuTitle(_T("NonPax Edit"),MFT_GRADIENT|MFT_TOP_TITLE);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, this );

	}
	
	CWnd::OnRButtonUp(nFlags, point);
}

void CNonPaxRelativePosEditCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ViewPort vp = m_rdc.GetActiveViewport();
	if(!vp.IsValid())
		return;
	mpSelectShape = m_scene.GetSelectNode(	vp.PickObject(point) );
	CWnd::OnRButtonDown(nFlags, point);
}

void CNonPaxRelativePosEditCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CPoint ptoff = point - m_lastMousePt;
	
	if(::GetAsyncKeyState(VK_MENU) < 0 )
	{
		MoveCamera(nFlags,point,0);		
		Invalidate(FALSE);
	}
	else
	{		
		
		if(mMouseState == _rotateshapeX)
		{
			RotateSelectShape(ptoff.x/100.0,ARCVector3(1,0,0));
			Invalidate(FALSE);
		}
		else if(mMouseState == _rotateshapeY)
		{
			RotateSelectShape(ptoff.x/100.0,ARCVector3(0,1,0));
			Invalidate(FALSE);
		}
		else if(mMouseState == _rotateshapeZ)
		{
			RotateSelectShape(ptoff.x/100.0,ARCVector3(0,0,1));
			Invalidate(FALSE);
		}
		else if(mMouseState == _moveShape && (nFlags&MK_LBUTTON) )
		{
			MoveShape(m_lastMousePt,point);
			Invalidate(FALSE);
		}
		else
		{

		}	
		
	}	

	m_lastMousePt = point;
	CWnd::OnMouseMove(nFlags, point);
}



void CNonPaxRelativePosEditCtrl::SetPosSpec( CNonPaxRelativePosSpec* pSpec )
{
	m_pNonPaxSpec = pSpec;	
}
void CNonPaxRelativePosEditCtrl::AddNewNonPaxPos( int nTypeIdx, const CPoint& pos )
{
	CNonPaxRelativePos* pNewPos = new CNonPaxRelativePos;
	pNewPos->m_nTypeIdx = nTypeIdx;

	ARCVector3 worldPos;
	if(GetMouseWorldPos(pos,worldPos))
	{
		worldPos.z = 0.0;
		pNewPos->m_relatePos = CPoint2008(worldPos.x,worldPos.y,worldPos.z);
		m_pNonPaxSpec->AddData(pNewPos);
		m_scene.AddUpdate(pNewPos);
		Invalidate(FALSE);
	}
}

void CNonPaxRelativePosEditCtrl::OnRotateX()
{
	mMouseState = _rotateshapeX;
	//SetCapture();

	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);	
}

void CNonPaxRelativePosEditCtrl::OnRotateY()
{
	mMouseState = _rotateshapeY;
	//SetCapture();

	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);	
}

void CNonPaxRelativePosEditCtrl::OnRotateZ()
{
	mMouseState = _rotateshapeZ;
	//SetCapture();

	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);	
}


void CNonPaxRelativePosEditCtrl::RotateSelectShape( double dAngle, const ARCVector3& axis )
{
	if(mpSelectShape)
	{
		mpSelectShape.DoRotate(dAngle,axis);
	}
}

void CNonPaxRelativePosEditCtrl::DoneEdit()
{
	if(mMouseState != _default  && mpSelectShape)
	{
		mpSelectShape.FlushChangeToData();
	}
}



void CNonPaxRelativePosEditCtrl::OnNonpaxpickDelete()
{
	// TODO: Add your command handler code here
	if(mpSelectShape && m_pNonPaxSpec)
	{
		CNonPaxRelativePos* pData = mpSelectShape.mpData;
		m_scene.RemoveShape3D(mpSelectShape);
		m_pNonPaxSpec->RemoveData(pData);
		mpSelectShape = CNonPaxShape3D();
		Invalidate(FALSE);
		if( GetParent() )
			GetParent()->SendMessage(ID_NONPAXPICK_DELETE);
	}
}

void CNonPaxRelativePosEditCtrl::MoveShape( CPoint ptFrom, CPoint ptTo )
{
	if(mpSelectShape)
	{
		ARCVector3 shapePos = mpSelectShape.GetPosition();
		ARCVector3 worldPosF,worldPosT;

		if( GetMouseWorldPos(ptFrom,worldPosF) && GetMouseWorldPos(ptTo,worldPosT) )
		{
			ARCVector3 offset = worldPosT - worldPosF;
			mpSelectShape.Move(offset);
		}
	}
}

void CNonPaxRelativePosEditCtrl::SetSceneCameType( emCameraType camType )
{
	m_scene.SetCamera(camType);
	OnSize(mnSizeType,mnSizex,mnSizeY);
	Invalidate(FALSE);
}


bool CNonPaxRelativePosEditCtrl::MoveCamera( UINT nFlags, const CPoint& ptTo,int vpIdx )
{
	ViewPort vp =  m_rdc.GetActiveViewport();
	CCameraNode camera = vp.getCamera();	
	return camera.MoveCamera(nFlags,ptTo-m_lastMousePt,&m_scene);		
}

void CNonPaxRelativePosEditCtrl::SetupViewport()
{
	if(!m_scene.IsValid())
	{
		m_scene.SetupScene();
		for(int i=0;i<m_pNonPaxSpec->GetCount();i++)
		{
			CNonPaxRelativePos* pPos = m_pNonPaxSpec->GetItem(i);
			m_scene.AddUpdate(pPos);
		}
	}

	CCameraNode& camera = m_scene.mCamera3D;
	Ogre::Camera* pcam = camera.mpCamera;
	//ASSERT(m_pCamera);
	//camera.Load(*m_pCamera);


	int nVpIndex = m_rdc.AddViewport( pcam, true );
	ViewPort m_vpCamera = m_rdc.GetViewport(nVpIndex);
	m_vpCamera.SetVisibleFlag(NormalVisibleFlag);
	m_vpCamera.SetClear(RGB(164,164,164));
	

	//set up coord displace scene
	//m_coordDisplayScene.Setup();
	//Ogre::Camera* pCoordCam = m_coordDisplayScene.GetLocalCamera();
	//m_coordDisplayScene.UpdateCamera( pcam);
	//nVpIndex = m_rdc.AddViewport( pCoordCam,false);
	//m_rdc.GetViewport(nVpIndex).SetClear(0,false);
	//m_rdc.SetViewportDim(nVpIndex,CRect(0,0,120,120),VPAlignPos(VPA_Left,VPA_Bottom),true);    	
	//m_coordDisplayScene.UpdateCamera(pcam);


	Invalidate(FALSE);
}

C3DSceneBase* CNonPaxRelativePosEditCtrl::GetScene( Ogre::SceneManager* pScene )
{
	return &m_scene;
}