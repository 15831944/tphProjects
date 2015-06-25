// 3DView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "3dview.h"
#include "TermPlanDoc.h"
#include "ALTObject3D.h"
#include "TermPlan.h"
#include "Results\EventLogBufManager.h"
#include "Common\WinMsg.h"
#include "PBuffer.h"
#include "ChildFrm.h"
#include "Floor2.h"
#include "AirsideGround.h"
#include "Airside3D.h"
#include "AnimationTimeManager.h"
#include "SelectionHandler.h"
#include "ProcessorTagWnd.h"
#include "3DUtility.h"
#include "CommonData\3DTextManager.h"
#include "Stretch3D.h"
#include "Common\Ray.h"
#include "TracerTagWnd.h"
#include "MoveProcessorsCommand.h"
#include "Inputs\AllCarSchedule.h"
#include "Common\DATASET.H"
#include "XPStyle\MenuCH.h"
#include "Common\UnitsManager.h"
#include "Common\CARCUnit.h"
#include "Inputs\RailWayData.h"
#include "Common\ViewMsg.h"
#include "MoveALTObjectCommand.h"
#include "LandsideDView.h"
#include "HillProc.h"
#include "MainFrm.h"
#include "INI.h"
#include "Landside\LandsideCrosswalk.h"
#include "InputAirside\ARCUnitManager.h"
#include "CommonData\Textures.h"
#include "Common\OleDragDropManager.h"
#include "ProcPropDlg.h"
#include "NodeView.h"
#include "Airspace3D.h"
#include "FAlignFloor.h"
#include "UsedProcInfo.h"
#include "Inputs\SubFlowList.h"
#include "DlgWallShapeProp.h"
#include "UsedProcInfoBox.h"
#include "Inputs\PaxFlowConvertor.h"
#include "Inputs\AllPaxTypeFlow.h"
#include "24bitBMP.h"
#include "AFTEMSBitmap.h"
#include "glrender\FlightShape.h"
#include "glrender\glShapeResource.h"
#include "Inputs\SubFlow.h"
#include "Common\AircraftAliasManager.h"
#include "ModelessDlgManager.h"
#include "PickConveyorTree.h"
#include "Render3DView.h"
#include "RotateALTObjectCommand.h"
#include "DlgStructureProp.h"
#include "AirsideObjectBaseDlg.h"
#include "DlgAirsideObjectDisplayProperties.h"
#include "ScaleProcessorsCommand.h"
#include "RotateShapesCommand.h"
#include "RotateProcessorsCommand.h"
#include "DlgPro2Name.h"
#include "DlgPipeAttachCrosswalk.h"
#include "Landside\InputLandside.h"
#include "DlgSelectLandsideObject.h"
#include "DlgWallDisplayProperties.h"
#include "Engine\Airside\AirsideFlightStairsLog.h"



#ifdef _DEBUG  
#define new DEBUG_NEW
#endif

static const CString strAirsideAnimDebug = _T("AirsideAnimDebug");

#define MENU_PROJECT_ID	0x04FE
#define MENU_FLOOR_ID	0x04FF
#define MENU_MULTIPLESELECT_ID	0x05FF

#define MAX_FLOOR	64
static const UINT	PANZOOMTIMEOUT = 1000;
static const UINT	ONSIZETIMEOUT = 1000;
static const UINT	MOUSEMOVETIMEOUT = 1000;
static const double   HEAVYSPEED = 9.8;
static const double FLIGHTEPSION = 0.1;
static const double CORNEREPSION = 0.0035;

const UINT C3DView::SEL_FLIGHT_OFFSET = 20000;
const UINT C3DView::SEL_VEHICLE_OFFSET =40000;

#define ID_BGEIN_CROSSWALK  10000
#define ID_END_CROSSWALK  10001
#define ID_DETACH_CROSSWALK_START 10005

static int m_nLastSelRailWay,m_nLastSelRailPt;
ARCVector3 LastRailPtPos;

static void inline RenderTempObjectInfo(CTermPlanDoc * pDoc, C3DView * pView){
	return;
	ALTObject * pTempObj = pDoc->GetTempObject();
	if(!pTempObj) return;
	ref_ptr<ALTObject3D>  pObj3D = ALTObject3D::NewALTObject3D(pTempObj);	
	if(pObj3D.get())
	{			
		glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(-4.0f,-4.0f);
		pObj3D->DrawOGL(pView);	
		glDisable(GL_POLYGON_OFFSET_FILL);
	}
}
/////////////////////////////////////////////////////////////////////////////


// C3DView

IMPLEMENT_DYNCREATE(C3DView, IRender3DView)

int C3DView::m_nViewIDCount=0;
int m_nViewIDCur=-1;
C3DView::C3DView()
{
	m_pPickConveyorTree=NULL;
	m_nViewID=C3DView::m_nViewIDCount;
	C3DView::m_nViewIDCount++;
	m_nTickTotal=0;
	m_bAnimPax=FALSE;
	m_nPaxSeledID=-1;
	m_bBusy = FALSE;
	m_bMovingProc = FALSE;
	m_bProcHasMoved = FALSE;
	m_iMaxTexSize = ((CTermPlanApp*)AfxGetApp())->GetProfileInt("Settings", "MaxTexSize", 1024); // 0=max possible; suggested: 1024-2048
	m_pDragShape = NULL;
	m_eMouseState = NS3DViewCommon::_default;
	m_hcurDefault = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	m_bTrackLButton = m_bTrackMButton = m_bTrackRButton = FALSE;
	m_ptMousePos = ARCVector3(0.0, 0.0, 0.0);
	m_nSharpTexId = 0;
	m_bShowRadar = TRUE;
	m_bUseSharpTex = FALSE;
	m_bAutoSharp = FALSE;
	m_bMovingRailPoint = FALSE;
	m_bSelRailEdit = FALSE;
	m_nSelectedRailWay = -1;
	m_nSelectedRailPt = -1;
	m_bMovingPipePoint = FALSE;
	m_bMovingPipeWidthPoint = FALSE;
	m_bSelPipeEdit = FALSE;
	m_nSelectedPipe = -1;
	m_nSelectedPipePt = -1;
	m_nSelectedPipeWidthPt = -1;
	m_nPanZoomTimeout = 3;
	m_nOnSizeTimeout = 4;
	m_nMouseMoveTimeout = 5;
	m_nRedSquareBlinkTimer = 6;
	m_pCamera = NULL;
	m_pProcessorTagWnd=NULL;
	m_pTracerTagWnd=NULL;
	//m_pDlgMovie = NULL;
	m_bPickConveyor=FALSE;
	m_bAnimDirectionChanged=FALSE;
	m_nAnimDirection=CEventLogBufManager::ANIMATION_FORWARD;

	m_bDragSelect = FALSE;

	m_ptRMouseDown = CPoint(0, 0);
    
	//2550-9-2 Ben
	m_nSelectedStructure=-1;
	m_bSelectStructureEdit=FALSE;
	m_nSelectedStructurePoint=-1;
	m_bMovingStructure=FALSE;
	m_nSelectedStructurePoint=FALSE;
	m_bMovingStructurePoint=FALSE;
	//Taxiway
	m_bMovingSelectTaxiwayMark=FALSE;
	m_bMovingSelectTaxiwayMainPt=FALSE;
	m_bMovingSelectTaxiwayWidthPt=FALSE;
	m_nSelectedTaxiwayPoint=-1;
	m_nSelectedTaxiwayWidthPoint=-1;
	m_nSelectedTaxiway=-1;

	m_nSelectWallShape=-1;
	m_nSelectWallShapePoint=-1;
	m_bMovingSelectWallShape=FALSE;
	m_bMovingSelectWallShapePoint=FALSE;

	_currentColorMode=_vivid;

	m_bCoutourEditMode = FALSE;
	m_bHillDirtflag=TRUE;
	m_hillDList=0;

	m_pViewPBuffer=NULL;
	m_pFloorPBuffer=NULL;
	
	
	m_nSelectedAircraft = -1;
	m_nSelectedAirRout = -1;

	m_procCurEditCtrlPoint = 0;

	m_nSelectHoldshortLine=-1;
	m_bMovingSelectHoldshortLine=FALSE;
	m_bMovingCamera = FALSE;
}




BEGIN_MESSAGE_MAP(C3DView, IRender3DView)
	//{{AFX_MSG_MAP(C3DView)
	ON_WM_CREATE()
	ON_WM_LBUTTONDOWN()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_RBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_CTX_PROCPROPERTIES, OnCtxProcproperties)
	ON_COMMAND(ID_CTX_COPYPROCESSOR, OnCtxCopyProcessor)
	ON_COMMAND(ID_CTX_ROTATEPROCESSORS, OnCtxRotateProcessors)
	ON_COMMAND(ID_CTX_SCALEPROCESSORS, OnCtxScaleProcessors)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSX, OnCtxScaleProcessorsX)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSY, OnCtxScaleProcessorsY)
	ON_COMMAND(ID_CTX_SCALEPROCESSORSZ, OnCtxScaleProcessorsZ)
	ON_COMMAND(ID_CTX_MOVEPROCESSORS, OnCtxMoveProcessors)
	ON_COMMAND(ID_CTX_DELETEPROC, OnCtxDeleteProc)
	ON_COMMAND(ID_CTX_DELETEPROCESSORS,OnCtxDeletePipe)
	ON_COMMAND(ID_CTX_MOVESHAPE, OnCtxMoveShapes)
	ON_COMMAND(ID_CTX_ROTATESHAPE, OnCtxRotateShapes)
	ON_COMMAND(ID_CTX_GROUP, OnCtxGroup)
	ON_COMMAND(ID_CTX_ALIGNPROCS,OnCtxAlignProcessors)
	ON_UPDATE_COMMAND_UI(ID_CTX_ALIGNPROCS,OnUpdateAlign)
	ON_UPDATE_COMMAND_UI(ID_CTX_GROUP, OnUpdateCtxGroup)
	ON_UPDATE_COMMAND_UI(ID_CTXPRO2_RENAME,OnUpdatePro2Name)
	ON_COMMAND(ID_CTXPRO2_RENAME,OnRenamePro2)
	ON_COMMAND(ID_CTX_UNGROUP, OnCtxUngroup)
	ON_UPDATE_COMMAND_UI(ID_CTX_UNGROUP, OnUpdateCtxUngroup)
	ON_COMMAND(ID_CTX_MOVESHAPE_Z, OnCtxMoveshapeZ)
	ON_COMMAND(ID_CTX_MOVESHAPE_Z_0, OnCtxMoveshapeZ0)
	ON_COMMAND(ID_COPY_TO_CLIPBOARD, OnCopyToClipboard)
	ON_COMMAND(ID_CUT_TO_CLIPBOARD, OnCutToClipboard)
	ON_COMMAND(ID_PICKCONVEYOR_ADDDESTINATION, OnPickconveyorAdddestination)
	ON_COMMAND(ID_PICKCONVEYOR_INSERTAFTER, OnPickconveyorInsertafter)
	ON_COMMAND(ID_PICKCONVEYOR_INSERTBEFORE, OnPickconveyorInsertbefore)
	ON_COMMAND(ID_PROCESS_PASTE, OnProcessPaste)
	ON_COMMAND(ID_CLEAR_CLIPBOARD,OnClearClipboard)
	ON_COMMAND(ID_BGEIN_CROSSWALK,OnBeginCrossWalk)
	ON_COMMAND(ID_END_CROSSWALK,OnEndCrossWalk)
	ON_COMMAND_RANGE(ID_DETACH_CROSSWALK_START,ID_DETACH_CROSSWALK_START+0X0FFF,OnDetachCrossWalk)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_VIEW_PRINT, OnViewPrint)
	ON_COMMAND(ID_VIEW_EXPORT, OnViewExport)
	//
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	
	ON_COMMAND(ID_CTX_TOGGLERAILPTEDIT, OnToggleRailPtEdit)
	ON_COMMAND(ID_CTX_INSERTNEWEDITPOINTHERE, OnInsertRailEditPt)
	ON_COMMAND(ID_CTX_DELETERAILPOINT, OnDeleteRailEditPt)
	ON_UPDATE_COMMAND_UI(ID_CTX_TOGGLERAILPTEDIT, OnUpdateToggleRailPtEdit)

	ON_UPDATE_COMMAND_UI(ID_INDICATOR_GRIDSPACING, OnUpdateGridSpacing)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_FPS, OnUpdateFPS)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMTIME, OnUpdateAnimTime)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_ANIMATIONSPEED , OnUpdateAnimationSpeed)
	ON_UPDATE_COMMAND_UI(ID_WALLSHAPEMENU_ADDPOINTHERE, OnUpdateAddPoint)

	ON_COMMAND_RANGE(CHANGE_SHAPES_MENU_LIST_ID_BEGIN,CHANGE_SHAPES_MENU_LIST_ID_END,OnChangeShapesMenuList)

	ON_COMMAND(ID_VIEW_SHARPENVIEW, OnSharpenView)
	ON_COMMAND(ID_VIEW_RADAR, OnViewRadar)

	ON_WM_TIMER()

	ON_COMMAND(ID_CTX_FLIPPROCS, OnCtxFlipProcessors)
	ON_COMMAND(ID_CTX_REFLECTPROCS, OnCtxReflectProcessors)
	ON_COMMAND(ID_CTX_ARRAYPROCS, OnCtxArrayProcessors)
	ON_COMMAND(ID_ANIMPAX,OnAnimPax)
	ON_UPDATE_COMMAND_UI(ID_ANIMPAX,OnUpdateAnimPax)
	ON_COMMAND_RANGE(ID_PASTE_TO_FLOOR1,ID_PASTE_TO_FLOOR4,OnPasteToFloor)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PASTE_TO_FLOOR1,ID_PASTE_TO_FLOOR4,OnUpdatePasteToFloor)

	ON_MESSAGE(TPWM_DESTROY_DLGWALKTHROUGH, OnMsgDestroyDlgWalkthrough)

	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
//	ON_WM_CHANGEUISTATE()
	ON_COMMAND_RANGE(MENU_FLOOR_ID, MENU_FLOOR_ID + 255, OnCopyProcessors)
	ON_COMMAND_RANGE(MENU_MULTIPLESELECT_ID, MENU_MULTIPLESELECT_ID + 255, OnChooseMultipleSelectItem)
	
	ON_COMMAND(ID_CTX_TOGGLESTRUCTUREPTEDIT, OnCtxTogglestructureptedit)
	ON_UPDATE_COMMAND_UI(ID_CTX_TOGGLESTRUCTUREPTEDIT, OnUpdateCtxTogglestructureptedit)
	
	ON_COMMAND(ID_CTX_TOGGLEPIPEPTEDIT, OnCtxTogglepipeptedit)
	ON_UPDATE_COMMAND_UI(ID_CTX_TOGGLEPIPEPTEDIT, OnUpdateCtxTogglepipeptedit)
	ON_COMMAND(ID_SUBMENU_REMOVESURFACE, OnSubmenuRemovesurface)
	ON_COMMAND(ID_SUBMENU_PROPRERTIES, OnSubmenuProprerties)
	ON_COMMAND(ID_COLORMODE_VIVID, OnColormodeVivid)
	ON_COMMAND(ID_COLORMODE_DESATURATED, OnColormodeDesaturated)
	ON_WM_CLOSE()
	
	ON_COMMAND(ID_ALTOBJECT_ALIGN,OnStandAlign)
	ON_UPDATE_COMMAND_UI(ID_ALTOBJECT_ALIGN,OnUpdateStandAlign)
	ON_COMMAND(ID_TAIXWAY_ENABLE, OnTaxiwayPointEnableEdit)
	ON_UPDATE_COMMAND_UI(ID_TAIXWAY_ENABLE, OnUpdateTaxiwayEnable)
	ON_COMMAND(ID_TAIXWAY_ADDPOINTHERE, OnTaxiwayAddpointhere)
	ON_COMMAND(ID_TAIXWAY_DELETEPOINT, OnTaxiwayDeletepoint)
	ON_COMMAND(ID_WALLSHAPE_POINTEDIT, OnWallshapePointedit)
	ON_UPDATE_COMMAND_UI(ID_WALLSHAPE_POINTEDIT, OnUpdateWallshapePointedit)
	ON_COMMAND(ID_WALLSHAPEMENU_ADDPOINTHERE, OnWallshapemenuAddpointhere)
	ON_COMMAND(ID_WALLSHAPEPOINTMENU_DELETE, OnWallshapepointmenuDelete)
	ON_COMMAND(ID_WALLSHAPE_DELETE, OnWallshapeDelete)
	ON_COMMAND(ID_WALLSHAPEMENU_WALLSHAPEPROPERTY, OnWallshapemenuWallshapeproperty)
	ON_COMMAND(ID_WALLSHAPEMENU_DISPLAYPROPERTIES, OnCtxWallshapeDispProperties)
	ON_COMMAND(ID_VIEW_CLOSE, OnViewClose)
	ON_COMMAND(ID_AIRCRAFTDISPLAY_TOGGLEONTAG, OnAircraftdisplayToggleontag)
	ON_UPDATE_COMMAND_UI(ID_AIRCRAFTDISPLAY_TOGGLEONTAG, OnUpdateAircraftdisplayToggleontag)
//	ON_COMMAND(ID_AIRROUTE_DISPLAYPROPERTIES, OnAirrouteDisplayproperties)
	ON_COMMAND(ID_DIRECTION_BIDIRECTION,OnCtxDirectionBidirection)
	ON_COMMAND(ID_DIRECTION_POSITIVE,OnCtxDirectionPositive)
	ON_COMMAND(ID_DIRECTION_NEGATIVE,OnCtxDirectionNegative)
	ON_UPDATE_COMMAND_UI_RANGE(ID_DIRECTION_BIDIRECTION, ID_DIRECTION_NEGATIVE, OnUpdateTaxiwayDir)
	
	ON_COMMAND(ID_ALTOBJECT_OBJECTPROPERTY, OnALTObjectProperty)
	ON_COMMAND(ID_ALTOBJECT_LOCK, OnALTObjectLock)
	ON_COMMAND(ID_ALTOBJECT_DISPLAYPROPERTIES, OnALTObjectDisplayProperty)
	ON_COMMAND(ID_ALTOBJECT_COPY, OnAltobjectCopy)
	ON_COMMAND(ID_ALTOBJECT_MOVE, OnAltobjectMove)
	ON_COMMAND(ID_ALTOBJECT_ROTATE, OnAltobjectRotate)
	ON_COMMAND(ID_ALTOBJECT_DELETE, OnAltobjectDelete)
	//ON_COMMAND(ID_ALTOBJECT_COPY_CLIPBOARD, OnAltobjectCopyClipboard)
	//ON_COMMAND(ID_ALTOBJECT_REFLECT, OnAltobjectReflect)
	//ON_COMMAND(ID_ALTOBJECT_FLIP, OnAltobjectFlip)
	ON_COMMAND(ID_ALTOBJECT_EDITCONTROLPOINT, OnAltobjectEditcontrolpoint)	
	ON_COMMAND(ID_SUBMENU_ENABLEEDITCONTROLPOINT, OnSubmenuEnableeditcontrolpoint)
	ON_UPDATE_COMMAND_UI(ID_SUBMENU_ENABLEEDITCONTROLPOINT, OnUpdateSubmenuEnableeditcontrolpoint)
	//ON_UPDATE_COMMAND_UI(ID_ALTOBJECT_EDITCONTROLPOINT, OnUpdateAltobjectEditcontrolpoint)
	ON_COMMAND(ID_FILLETTAXIWAY_EDITCONTROLPOINT, OnFillettaxiwayEditcontrolpoint)
	ON_COMMAND(ID_WALLSHAPE_COPY, OnWallshapeCopy)
	ON_COMMAND(ID_SUBMENU_COPYSURFACE, OnSubmenuCopysurface)
	ON_COMMAND(ID_ALTOBJECT_ADDEDITPOINT, OnAltobjectAddeditpoint)
	ON_COMMAND(ID_REMOVE_POINT, OnRemovePoint)
	ON_COMMAND(ID_INTERSECTEDSTRETCH_EDITPOINT, OnIntersectedstretchEditpoint)
END_MESSAGE_MAP()

BOOL C3DView::PreCreateWindow(CREATESTRUCT& cs)
{
	// An OpenGL window must be created with the following flags and must not
    // include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	//if (!CView::PreCreateWindow(cs))
	//	return FALSE;
	cs.lpszClass = AfxRegisterWndClass(CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, AfxGetApp()->LoadStandardCursor(IDC_ARROW), 0, 0);
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// C3DView drawing

void C3DView::OnDraw(CDC* pDC)
{
	//// TRACE("*OnDraw\n");
	Draw();	
	
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanView printing
bool bAfterMovingObject = false;

BOOL C3DView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void C3DView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void C3DView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// C3DView diagnostics

#ifdef _DEBUG
void C3DView::AssertValid() const
{
	CView::AssertValid();
}

void C3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTermPlanDoc* C3DView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTermPlanDoc)));
	return (CTermPlanDoc*)m_pDocument;
}
C3DCamera* C3DView::GetCamera()
{
	return m_pCamera;
}

const C3DCamera* C3DView::GetCamera() const
{
	return m_pCamera;
}

#endif //_DEBUG

BOOL C3DView::InitOGL()
{
	GLfloat lightAmbient[] =	{ 0.3f, 0.3f, 0.3f, 1.0f };
	GLfloat lightDiffuse[] =	{ 0.9f, 0.9f, 0.9f, 1.0f };
	GLfloat lightPosition[] =	{ 0.0f, 0.0f, 2.0f, 0.0f };

	glClearColor(ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, 1.0f);	// specify gray as clear color
	glClearDepth(1.0f);						// back of buffer is clear depth
    glClearStencil(0);
	glEnable(GL_DEPTH_TEST);				// enable depth testing
	glShadeModel(GL_SMOOTH);				// enable smooth shading
	glEnable(GL_BLEND);						// for transparency
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_NORMALIZE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glClearStencil(0);	
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);		// Setup The Ambient Light
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,lightPosition);	// Position The Light
	glEnable(GL_LIGHT1); // Enable Light One

	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_AUTO_NORMAL);

	return TRUE;


}

void C3DView::CalculateFrameRate()
{
	// Below we create a bunch of static variables because we want to keep the information
	// in these variables after the function quits.  We could make these global but that would
	// be somewhat messy and superfluous.  Note, that normally you don't want to display this to
	// the window title bar.  This is because it's slow and doesn't work in full screen.
	// Try using the 3D/2D font's.  You can check out the tutorials at www.gametutorials.com.

	static float framesPerSecond    = 0.0f;							// This will store our fps
    static float lastTime			= 0.0f;							// This will hold the time from the last frame

	// Here we get the current tick count and multiply it by 0.001 to convert it from milliseconds to seconds.
	// GetTickCount() returns milliseconds (1000 ms = 1 second) so we want something more intuitive to work with.
    float currentTime = GetTickCount() * 0.001f;				

	// Increase the frame counter
    ++framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored.  If it is greater than 1
	// that means a second has passed and we need to display the new frame rate.  Of course, the first time
	// will always be greater than 1 because lastTime = 0.  The first second will NOT be true, but the remaining
	// ones will.  The 1.0 represents 1 second.  Let's say we got 12031 (12.031) from GetTickCount for the currentTime,
	// and the lastTime had 11230 (11.230).  Well, 12.031 - 11.230 = 0.801, which is NOT a full second.  So we try again
	// the next frame.  Once the currentTime - lastTime comes out to be greater than a second (> 1), we calculate the
	// frames for this last second.
    if( currentTime - lastTime > 1.0f )
    {
		// Here we set the lastTime to the currentTime.  This will be used as the starting point for the next second.
		// This is because GetTickCount() counts up, so we need to create a delta that subtract the current time from.
	    lastTime = currentTime;

		m_dFrameRate = framesPerSecond;
		
		// Reset the frames per second
        framesPerSecond = 0;
    }
}


void C3DView::RenderOffScreen( PBufferPointer & Hpbuff, int nWidth, int nHeight, BYTE* pBits )
{
	wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC );
	HGLRC hglrc = wglGetCurrentContext(); //save the current context and dc
	HDC hdc = wglGetCurrentDC();
	
	if(Hpbuff){
		if(Hpbuff->width != nWidth || Hpbuff->height != nHeight ){
			delete Hpbuff;
			Hpbuff = new PBuffer(nWidth,nHeight,0);
			if( !Hpbuff->Initialize(NULL,true) )
			{
				delete Hpbuff;
				Hpbuff= NULL;
				return;
			}
		}
	}
	else  //m_pBuffer not exist;
	{
		Hpbuff = new PBuffer(nWidth,nHeight,0);
		if( !Hpbuff->Initialize(NULL,true) )
		{
			delete Hpbuff;
			Hpbuff = NULL;
			return ;
		}
	}	
	Hpbuff->MakeCurrent();

	static const GLfloat defaultmaterial[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	static const GLfloat lightPosition[] =	{ -1.0f, 0.0f, 2.0f, 0.0f };

	InitOGL();
	
	//GLint oldviewport[4];
	//glGetIntegerv(GL_VIEWPORT, oldviewport);	//get the current viewport

	//double dOldAspect = m_dAspect;
	//m_dAspect = (double) nWidth / (double) nHeight;
	glViewport(0, 0, nWidth, nHeight);

	//render scene with sharp floor
	
	glClearColor( ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//apply view projection and transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();
	glDisable(GL_LIGHTING);
	if(GetCamera()->GetProjectionType() == C3DCamera::parallel)
	{
		CFloor2 * pFloor = NULL;
		if(GetDocument()->GetCurrentMode() == EnvMode_AirSide)
		{
			pFloor = GetParentFrame()->GetAirside3D()->GetActiveLevel();
		}
		else 
			pFloor = GetDocument()->GetCurModeFloor().m_vFloors[GetDocument()->m_nActiveFloor];

		//2d view, draw active floor as lines
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFFL );
		glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(2.0f,2.0f);
		(pFloor)->DrawOGLNoTexture(false,0.0);
		glDisable(GL_POLYGON_OFFSET_FILL);

		glDisable(GL_DEPTH_TEST);
		glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFFL );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
		(pFloor)->DrawOGLasLines(0.0);
		glDisable(GL_STENCIL_TEST);	
		glEnable(GL_DEPTH_TEST);
	}
	else 
	{
		
		std::vector<CFloor2*> vSortedFloors;
		std::vector<CFloor2*> teminalFloors = GetFloorsByMode(EnvMode_Terminal);
		vSortedFloors.insert(vSortedFloors.end(),teminalFloors.begin(), teminalFloors.end() );
		CAirsideGround *pAirsideFloor = GetAirside3D()->GetActiveLevel();
		if(pAirsideFloor)
			vSortedFloors.push_back(pAirsideFloor);


		std::sort(vSortedFloors.begin(), vSortedFloors.end(), CBaseFloor::CompareFloors);		
		int nFloorCount=vSortedFloors.size();
		for(int i=0; i<nFloorCount; i++) 
		{
			if((vSortedFloors[i])->IsVisible())
			{
				DistanceUnit dAlt;
				dAlt = vSortedFloors[i]->Altitude();
				
				glEnable(GL_STENCIL_TEST);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );				
				(vSortedFloors[i])->DrawOGLNoTexture(false,dAlt);


				glDisable(GL_DEPTH_TEST);				
				glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );				
				(vSortedFloors[i])->DrawOGLasLines(dAlt);
				glDisable(GL_STENCIL_TEST);
				glEnable(GL_DEPTH_TEST);			
				//vSortedFloors[i]->DrawGrid();
			}
		}
	}	
	
	DrawGrids(GetDocument(),this);
	//render scene without floors
	RenderScene(FALSE);
	//render text
	RenderText();
	//Draw Grid
	
	//DrawScene();
	//set proj and m-v matrices
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, nWidth, 0.0, nHeight,-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
 
	//draw ARCTerm logo in top left
	glEnable(GL_TEXTURE_RECTANGLE_NV);
	glBindTexture( GL_TEXTURE_RECTANGLE_NV, GetParentFrame()->LogoTexID() );
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	glColor3ubv(ARCColor3::WHITE);
	glBegin(GL_QUADS);
	glNormal3d(0.0,0.0,1.0);
	glTexCoord2f(0.0,18.0); glVertex2f(0.0,0.0);
	glTexCoord2f((float)nWidth,18.0); glVertex2f((float)nWidth,0.0);
	glTexCoord2f((float)nWidth,0.0); glVertex2f((float)nWidth,18.0);
	glTexCoord2f(0.0,0.0); glVertex2f(0.0,18.0);
	glEnd();
	glDisable(GL_TEXTURE_RECTANGLE_NV);

	if(GetDocument()->m_eAnimState != CTermPlanDoc::anim_none) {
		//draw anim time in bottom right corner
		long nTime = GetDocument()->m_animData.nLastAnimTime;
		int nHour = nTime / 360000;
		int nMin = (nTime - nHour*360000)/6000;
		int nSec = (nTime - nHour*360000 - nMin*6000)/100;
		CString s(_T(""));
		s.Format("%02d:%02d:%02d", nHour, nMin, nSec);
		glColor3ubv(ARCColor3::BLACK);
		TEXTMANAGER3D->DrawBitmapText(s,CPoint(nWidth-50, 5));
	}


	glEnable(GL_DEPTH_TEST);

	glEnable(GL_LIGHTING);
	RenderCompass();
	glDisable(GL_LIGHTING);

	glFlush();

	glReadPixels(0, 0, nWidth, nHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (GLvoid*) pBits);

	//wglMakeCurrent(NULL, NULL);

	//m_dAspect = dOldAspect;
	wglMakeCurrent(hdc, hglrc);
	//m_dAspect = (double) oldviewport[2] / (double) oldviewport[3];
	//glViewport(0,0,oldviewport[2],oldviewport[3]);
}

void C3DView::DestroySharpTexture()
{
	if(glIsTexture(m_nSharpTexId))
		glDeleteTextures(1, &m_nSharpTexId);
	m_nSharpTexId = 0;
	m_bUseSharpTex = FALSE;
}

void C3DView::GenerateSharpTexture()
{
	CWaitCursor waitCursor;	
	
	wglMakeCurrent( m_pDC->GetSafeHdc(), m_hRC );

	//destroy texture if it exists
	if(glIsTexture(m_nSharpTexId))
	{
		glDeleteTextures(1, &m_nSharpTexId);
	}
	
	glGenTextures(1, &m_nSharpTexId);		
	
	//generate texture id
	//save the current context and dc
	HGLRC hglrc = wglGetCurrentContext(); 
	HDC hdc = wglGetCurrentDC();	
	
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	//viewport[2] is width, viewport[3] is height
	int nTexWidth = viewport[2];
	int nTexHeight = viewport[3];

	if(!m_pViewPBuffer){
		m_pViewPBuffer=new PBuffer(nTexWidth, nTexHeight, 0);
		m_pViewPBuffer->Initialize(NULL,true);
	}
	else if(m_pViewPBuffer->width !=  nTexWidth || m_pViewPBuffer->height != nTexHeight )
	{
		delete m_pViewPBuffer;
		m_pViewPBuffer = new PBuffer(nTexWidth , nTexHeight , 0);		
		m_pViewPBuffer->Initialize(NULL, true);
	}
	//pbuff.Initialize(NULL, true);
	m_pViewPBuffer->MakeCurrent();	//get pBuffer info
	
	nTexWidth=m_pViewPBuffer->width;
	nTexHeight=m_pViewPBuffer->height;
	
	GLenum TexFormat=GL_RGBA;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//////////////////
	//allocate texture memory
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_nSharpTexId);
	//set texture filtering params
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri(GL_TEXTURE_RECTANGLE_NV, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	
	//glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, GL_RGBA4, nTexWidth, nTexHeight, 0, GL_RGBA, GL_FLOAT, 0);
	glTexImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 3, nTexWidth, nTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	//clear the background
	glClearColor( ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, ARCColor3::GREY_BACKGROUND_FLOAT, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//apply view projection and transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();
	glDisable(GL_LIGHTING);
	if(GetCamera()->GetProjectionType() == C3DCamera::parallel)
	{
		CFloor2 * pFloor = NULL;
		
		if(GetDocument()->GetCurrentMode() == EnvMode_AirSide)
		{
			pFloor = GetParentFrame()->GetAirside3D()->GetActiveLevel();
		}
		else 
			pFloor = GetDocument()->GetCurModeFloor().m_vFloors[GetDocument()->m_nActiveFloor];

		//2d view, draw active floor as lines
		glEnable(GL_STENCIL_TEST);
		glClear(GL_STENCIL_BUFFER_BIT);
		glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFFL );
		glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );		
		(pFloor)->DrawOGLNoTexture(false,0.0);		
		
		glDisable(GL_DEPTH_TEST);
		glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFFL );
		glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
		(pFloor)->DrawOGLasLines(0.0);
		glDisable(GL_STENCIL_TEST);	
		glEnable(GL_DEPTH_TEST);		
	}
	else 
	{
		//3d view, draw all floors as lines		
		std::vector<CFloor2*> vSortedFloors;
		std::vector<CFloor2*> teminalFloors = GetFloorsByMode(EnvMode_Terminal);
        vSortedFloors.insert(vSortedFloors.end(),teminalFloors.begin(), teminalFloors.end() );
		CAirsideGround *pAirsideFloor = GetAirside3D()->GetActiveLevel();
		if(pAirsideFloor)
			vSortedFloors.push_back(pAirsideFloor);

		
		std::sort(vSortedFloors.begin(), vSortedFloors.end(), CBaseFloor::CompareFloors);		
		int nFloorCount=vSortedFloors.size();
		for(int i=0; i<nFloorCount; i++) 
		{
			if((vSortedFloors[i])->IsVisible())
			{
				DistanceUnit dAlt;
				/*if(GetDocument()->GetCurrentMode() == EnvMode_AirSide)
				{
					dAlt = vSortedFloors[i]->RealAltitude()*GetDocument()->m_iScale;
				}
				else*/
				{
					dAlt = vSortedFloors[i]->Altitude();
				}
				glEnable(GL_STENCIL_TEST);
				glClear(GL_STENCIL_BUFFER_BIT);
				glStencilFunc( GL_ALWAYS, 1, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );				
				(vSortedFloors[i])->DrawOGLNoTexture(false,dAlt);
				

				glDisable(GL_DEPTH_TEST);				
				glStencilFunc( GL_EQUAL, 1, 0xFFFFFFFFL );
				glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );				
				(vSortedFloors[i])->DrawOGLasLines(dAlt);
				glDisable(GL_STENCIL_TEST);
				glEnable(GL_DEPTH_TEST);			
				//vSortedFloors[i]->DrawGrid();
			}
		}
	}		
	//copy p-buffer to texture
	glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_nSharpTexId);
	glCopyTexSubImage2D(GL_TEXTURE_RECTANGLE_NV, 0, 0, 0, 0, 0, nTexWidth, nTexHeight);	
	

	//store world coords of sharp text bl and tr corners
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	double wx,wy,wz;
	gluUnProject(0.0,0.0,0.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
	m_ptSharpBL[VX] = wx;
	m_ptSharpBL[VY] = wy;
	gluUnProject((GLdouble)viewport[2],(GLdouble)viewport[3],0.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
	m_ptSharpTR[VX] = wx;
	m_ptSharpTR[VY] = wy;
	gluUnProject(0.0,(GLdouble)viewport[3],0.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
	m_ptSharpTL[VX] = wx;
	m_ptSharpTL[VY] = wy;
	gluUnProject((GLdouble)viewport[2],0.0,0.0,mvmatrix,projmatrix,viewport,&wx,&wy,&wz);
	m_ptSharpBR[VX] = wx;
	m_ptSharpBR[VY] = wy;
	m_SharpTexSize.cx = viewport[2];
	m_SharpTexSize.cy = viewport[3];

	//restore dc and glrc
	wglMakeCurrent( hdc, hglrc );
	
	
	m_bUseSharpTex = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// C3DView message handlers

int C3DView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{	
	
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pDC = new CClientDC(this);
	if(!m_pDC)
		return -1;
	
	if(!SetupPixelFormat(m_pDC->GetSafeHdc(), PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER /*| PFD_SWAP_COPY*/))
		return -1;
	
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if(!m_hRC)
		return -1;

	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))	{// make RC current 
		return FALSE;
	}
	
	C3DView* pBase3DView = GetParentFrame()->GetPane(0,0);
	if(this != pBase3DView){
		BOOL bshareOk = ::wglShareLists(pBase3DView->GetHGLRC(), m_hRC);
		ASSERT(bshareOk);
	}
	else
	{
		TEXTMANAGER3D->Initialize(m_pDC);
	}
	//share lists
	glewInit();		
						
	InitOGL(); // Initialize OpenGL

#if _USECHLOE_ONBOARD
	//chloe
	CHLOE_SYSTEM_R2(ARCRSystem, m_OnBoradScene, OnBoradScene*)->create_scene("OnBoradScene", "OnBoradScene");
	SAFE_CALL(m_OnBoradScene)->create_device((void*)m_pDC->GetSafeHdc(), (void*)m_hRC);
	SAFE_CALL(m_OnBoradScene)->show_test_flight(true);
	//
#endif	

	//set the camera
	int row, col;
	if(GetParentFrame()->IsChildPane(this,row,col)) {
		
		m_pCamera = GetParentFrame()->GetWorkingCamera(row, col);
	}
	else
		ASSERT(0);
	
	m_oleDropTarget.Register(this);

	//enroll view
	ANIMTIMEMGR->Enroll3DView(this);
	
	return 0;
}

ARCVector3 C3DView::Get3DMousePos(const CPoint& p,BOOL bWithZ,int nWithFloorIndex)
{
	//	Find the 3d point under the mouse
	// 
	// here we get the 3d point in space under the mouse by using the line between the points at depth = 0 and depth =1
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];

	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);

	GLint realy = viewport[3] - p.y;
	ARCVector3 pt1, vDir;
	gluUnProject((GLdouble) p.x, (GLdouble)realy, 0.0, mvmatrix, projmatrix, viewport, &pt1[VX], &pt1[VY], &pt1[VZ]);
	gluUnProject((GLdouble) p.x, (GLdouble)realy, 1.0, mvmatrix, projmatrix, viewport, &vDir[VX], &vDir[VY], &vDir[VZ]);
	::wglMakeCurrent(NULL, NULL);
	vDir = vDir - pt1;
	double dAlt = GetDocument()->GetActiveFloorAltitude(); // altitude at which we want the point
	if(nWithFloorIndex>=0&&nWithFloorIndex<=3)
	{
		//if(GetDocument()->m_systemMode == EnvMode_AirSide)
		//{
		//	dAlt = GetDocument()->GetCurModeFloor().m_vFloors[nWithFloorIndex]->RealAltitude()*GetDocument()->m_iScale;
		//}
		//else
		{
			dAlt = GetDocument()->GetCurModeFloor().m_vFloors[nWithFloorIndex]->Altitude();
		}

	}
	//// TRACE("vp = {%d,%d,%d,%d}, alt=%.2f\n", viewport[0], viewport[1], viewport[2], viewport[3], dAlt);

	//chloe begin delete the under code
	//pt1[VZ]-=dAlt;
	//float fZValue=0;
	//if(bWithZ&&GetDocument()->GetSelectProcessors().GetCount()!=0)
	//{
	//	ARCVector3 vTemp =	GetDocument()->GetSelectedObjectDisplay(0)->GetLocation();
	//	fZValue=static_cast<float>(vTemp[VZ]);
	//	return (pt1 + ((/*dAlt+*/fZValue-pt1[VZ]) / vDir[VZ]) * vDir);
	//}
	//pt1[VZ]+=dAlt;
	//chloe end

	return(pt1 + ((dAlt-pt1[VZ]) / vDir[VZ]) * vDir);

}

ARCVector3 C3DView::Get3DMousePos(const CPoint& p,double logicAlt)
{
	CFloorList & vfloors = GetDocument()->GetCurModeFloor().m_vFloors;
	int nFloorCount = vfloors.size();
	int _ifloor = (int)(logicAlt/SCALE_FACTOR);
	int _iUpfloor = _ifloor +1;
	double dAlt,diffAlt;
	
	if(_ifloor<nFloorCount-1){
		
		diffAlt =vfloors[_iUpfloor]->Altitude()- vfloors[_ifloor]->Altitude();
	}else{
		//_ifloor = 0;
		diffAlt = 1000.0;
	}
	
	dAlt = (logicAlt/SCALE_FACTOR - _ifloor)*diffAlt + vfloors[_ifloor]->Altitude();
	
	::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
	GLint realy = viewport[3] - p.y;
	ARCVector3 pt1, vDir;
	gluUnProject((GLdouble) p.x, (GLdouble) realy, 0.0, mvmatrix, projmatrix, viewport, &pt1[VX], &pt1[VY], &pt1[VZ]);
	gluUnProject((GLdouble) p.x, (GLdouble) realy, 1.0, mvmatrix, projmatrix, viewport, &vDir[VX], &vDir[VY], &vDir[VZ]);
	::wglMakeCurrent(NULL, NULL);
	vDir = vDir - pt1;		
	
	return pt1 + ((dAlt-pt1[VZ]) / vDir[VZ]) * vDir;
	
	 
}

void C3DView::OnMouseHover(CPoint point)
{
	if(GetDocument()->m_eAnimState != CTermPlanDoc::anim_none) {
		//check to see if hit on flight tag
		SelectACTags(0, point.x, point.y);
		GetDocument()->UpdateAllViews(NULL);
	}

	if(GetParentFrame()->IsInfoBtnActive()) {
		GLuint vidx[32];
		int nSelectCount=SelectScene(0, point.x, point.y, vidx, CSize(2,2), FALSE, -1, FALSE);
		if(nSelectCount>0) {

			int idx = GetSelectIdx(vidx[0]);
			BYTE seltype = GetSelectType(vidx[0]);
			BYTE selsubtype = GetSelectSubType(vidx[0]);

			if( SELTYPE_PROCESSOR == seltype ) {
				
				//find closest non UC processor
				// TRACE("Hover on processor %d\n", idx);
				if(m_pProcessorTagWnd) {
					m_pProcessorTagWnd->DestroyWindow();
					delete m_pProcessorTagWnd;
				}
				m_pProcessorTagWnd = new CProcessorTagWnd(idx, GetDocument());
				m_pProcessorTagWnd->CreateEx(WS_EX_TOOLWINDOW, NULL, NULL, WS_CHILD, CRect(0,0,250,200), this, IDD_DIALOG_PROCESSORTAGS );
				CRect rc, rcClient, rcMain;
				this->GetWindowRect(&rcClient);
				AfxGetMainWnd()->GetWindowRect(&rcMain);
				m_pProcessorTagWnd->GetWindowRect(&rc);
		
				if( point.y > rc.Height() ) {
					point.y -= rc.Height();
				}
				if( point.x > (rcClient.Width()-rc.Width()) ) {
					point.x -= rc.Width();
				}

				//ClientToScreen(&point);
					
				m_pProcessorTagWnd->MoveWindow(point.x, point.y, rc.Width(), rc.Height());
				m_pProcessorTagWnd->ShowWindow(SW_SHOW);
			}
			else if( SELTYPE_AIRSIDETRACER == seltype ) {
				// TRACE("Hover on airside tracer %d\n", idx);
			}
		}
		GetDocument()->UpdateAllViews(NULL);
	}
	else {
		// TRACE("OnMouseHover(CPoint point);\n");
	}
	
}

void C3DView::OnMouseMove(UINT nFlags, CPoint point) 
{
    // Variables...
    POINT   ScreenPoint = {0, 0};
    
    if(point == m_ptFrom)
		return;

	ARCVector3 ptDiffPos = m_ptMousePosWithZ;
	ARCVector3 ptDiffPos2= m_ptMousePos;
		
	
	
	m_ptMousePosWithZ = Get3DMousePos(point,TRUE);
	m_ptMousePos = Get3DMousePos(point);

	
	ptDiffPos = m_ptMousePosWithZ - ptDiffPos;
	ptDiffPos2= m_ptMousePos - ptDiffPos2;
	//TRACE("MMove, point = {%d,%d}, m_ptMousePos = {%.2f, %.2f}, ptDiffPos = {%.2f, %.2f}\n", point.x, point.y, m_ptMousePos[VX], m_ptMousePos[VY], ptDiffPos[VX],ptDiffPos[VY]);
	//TRACE("MMove, point = {%d,%d}, m_ptMousePosWithZ = {%.2f, %.2f}, ptDiffPos2 = {%.2f, %.2f}\n", point.x, point.y, m_ptMousePosWithZ[VX], m_ptMousePosWithZ[VY], ptDiffPos2[VX],ptDiffPos2[VY]);

	if(m_pProcessorTagWnd)
		m_pProcessorTagWnd->ShowWindow(SW_HIDE);

	if(m_pTracerTagWnd)
		m_pTracerTagWnd->ShowWindow(SW_HIDE);

	m_nMouseMoveTimeout = SetTimer(m_nMouseMoveTimeout, MOUSEMOVETIMEOUT, NULL);
	//// TRACE("SetTimer\n");

	//force status bar update
	CChildFrame* pFrame = (CChildFrame*)GetParentFrame();
	pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);
	//// TRACE("OnMouseMove\n");

	CPoint _diff = point - m_ptFrom;
	ARCVector2 diff(_diff.x, _diff.y);
	diff[VY] = -diff[VY];
	m_ptFrom = point;

	double dAlt = GetDocument()->GetActiveFloorAltitude();

	if(::GetAsyncKeyState(VK_MENU)<0) {//ALT is pressed
		if(nFlags&MK_LBUTTON) {//L-button = pan
			diff = 2*diff;
			GetCamera()->Pan(diff);
			GetCamera()->AutoAdjustCameraFocusDistance(dAlt);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			UseSharpTexture(FALSE);
			m_bMovingCamera = TRUE;
			Invalidate(FALSE);
		}
		
        // Right mouse button zooms.
        else if(nFlags&MK_RBUTTON)
        {
			GetCamera()->Zoom(2*diff[VX]);
			GetCamera()->AutoAdjustCameraFocusDistance(dAlt);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			UseSharpTexture(FALSE);
			m_bMovingCamera = TRUE;
			Invalidate(FALSE);

            /* Zoom helper code...
                
                // Get cursor position in screen coordinates...
                if(!GetCursorPos(&ScreenPoint))
                    return;

                // Cursor at right edge, move to left...
                if(ScreenPoint.x >= (GetSystemMetrics(SM_CXSCREEN) - 1))
                    SetCursorPos(1, ScreenPoint.y);

                // Cursor at left edge, move to right...
                if(ScreenPoint.x == 0)
                    SetCursorPos(GetSystemMetrics(SM_CXSCREEN) - 2, ScreenPoint.y);

                // Cursor at bottom edge, move to top...
                if(ScreenPoint.y >= (GetSystemMetrics(SM_CYSCREEN) - 1))
                    SetCursorPos(ScreenPoint.x, 2);

                // Cursor at top edge, move to bottom...
                if(ScreenPoint.y == 0)
                    SetCursorPos(ScreenPoint.x, GetSystemMetrics(SM_CYSCREEN) - 2);*/

        }

        else if(nFlags&MK_MBUTTON) {//M-button = tumble
			diff = 2*diff;
			GetCamera()->Tumble(diff);
			m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
			UseSharpTexture(FALSE);
			m_bMovingCamera = TRUE;
			Invalidate(FALSE);
		}
	}
	else {
		switch(m_eMouseState)
		{
		case NS3DViewCommon::_pan:
			if(m_bTrackLButton) {
				diff = 2*diff;
				GetCamera()->Pan(diff);
				GetCamera()->AutoAdjustCameraFocusDistance(dAlt);
				m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
				UseSharpTexture(FALSE);
				m_bMovingCamera = TRUE;
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_dolly:
			if(m_bTrackLButton) {
				GetCamera()->Zoom(2*diff[VX]);
				GetCamera()->AutoAdjustCameraFocusDistance(dAlt);
				m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
				UseSharpTexture(FALSE);
				m_bMovingCamera = TRUE;
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_tumble:
			if(m_bTrackLButton) {
				diff = 2*diff;
				GetCamera()->Tumble(diff);
				m_nPanZoomTimeout = SetTimer(m_nPanZoomTimeout, PANZOOMTIMEOUT, NULL);
				UseSharpTexture(FALSE);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_scaleproc:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnScaleProc(diff[VX], diff[VY]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_scaleprocX:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnScaleProcX(diff[VX], diff[VY]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_scaleprocY:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnScaleProcY(diff[VX], diff[VY]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_scaleprocZ:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnScaleProcZ(diff[VX], diff[VY]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_rotateproc:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnRotateProc(diff[VX]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_rotateairsideobject:
			if(!(nFlags&MK_CONTROL))
			{
				RotateSelectObject(diff[VX]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_moveprocs:
			GetDocument()->OnMoveProcs(ptDiffPos[VX],ptDiffPos[VY],point,this);
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_moveshapes:
			GetDocument()->OnMoveShape(ptDiffPos[VX],ptDiffPos[VY]);
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_moveshapesZ:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnMoveShapeZ(ptDiffPos2[VX],ptDiffPos2[VY]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_rotateshapes:
			if(!(nFlags&MK_CONTROL))
			{
				GetDocument()->OnRotateShape(diff[VX]);
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_getarrayline:
		case NS3DViewCommon::_getalignline:
		case NS3DViewCommon::_getmirrorline:
		case NS3DViewCommon::_getonepoint:
		case NS3DViewCommon::_gettwopoints:
		case NS3DViewCommon::_getmanypoints:
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_getonerect:
			if(nFlags & MK_RBUTTON) {
				for(int i=0; i<static_cast<int>(m_vMousePosList.size()); i++) {
					m_vMousePosList[i]+=ptDiffPos;
				}
			}
			else {	
				if(m_vMousePosList.size() == 1)
					m_vMousePosList.push_back(m_ptMousePos);
				else if(m_vMousePosList.size() > 1)
					m_vMousePosList[1] = m_ptMousePos;
			}
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_distancemeasure:
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_default:
			if(nFlags&MK_LBUTTON) {
				if(m_bMovingProc) {
					m_bProcHasMoved = TRUE;
					GetDocument()->OnMoveProcs(ptDiffPos[VX],ptDiffPos[VY],point,this);
					Invalidate(FALSE);
				}
				else if(m_bMovingRailPoint) {
								
					std::vector<RailwayInfo*> railWayVect;
					GetDocument()->m_trainTraffic.GetAllRailWayInfo(railWayVect);
					if(m_nSelectedRailWay >=0 && m_nSelectedRailWay < static_cast<int>(railWayVect.size())){
						int numPts = railWayVect[m_nSelectedRailWay]->GetRailWayPath().getCount();	
						if(m_nSelectedRailPt >= 0 && m_nSelectedRailPt < numPts){
							Point ptRailWay;							
							railWayVect[m_nSelectedRailWay]->GetPoint(m_nSelectedRailPt,ptRailWay);							
							if(ptRailWay.getZ()<0)break;
							if(m_nLastSelRailPt==m_nSelectedRailPt && m_nLastSelRailWay==m_nSelectedRailWay )
							{
								ARCVector3 newPos = Get3DMousePos(point,ptRailWay.getZ());
								ARCVector3 ptDiffPosLogicZ = newPos - LastRailPtPos;
								LastRailPtPos = newPos;
								if(ptDiffPosLogicZ[VX]+ptDiffPosLogicZ[VY] < 10 * SCALE_FACTOR)
									GetDocument()->OnMoveRailPt(m_nSelectedRailWay,m_nSelectedRailPt,ptDiffPosLogicZ[VX],ptDiffPosLogicZ[VY]);
							}else
							{
								LastRailPtPos = Get3DMousePos(point,ptRailWay.getZ());
								m_nLastSelRailWay = m_nSelectedRailWay;
								m_nLastSelRailPt = m_nSelectedRailPt;

							}
							
						}
					}
					
					Invalidate(FALSE);
				}
				else if(m_bMovingPipePoint) 
				{
					GetDocument()->OnMovePipePt(m_nSelectedPipe,m_nSelectedPipePt,ptDiffPos[VX],ptDiffPos[VY]);
					Invalidate(FALSE);
				}
				else if(m_bMovingPipeWidthPoint)
				{					
					GetDocument()->OnMovePipeWidthPt(m_nSelectedPipe,m_nSelectedPipeWidthPt,ptDiffPos[VX],ptDiffPos[VY]);
					Invalidate(FALSE);
				}
				else if(m_bMovingSelectWallShape || m_bMovingSelectWallShapePoint)
				{
					if(m_bMovingSelectWallShapePoint && m_nSelectWallShape >= 0)
					{
						GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape)->MovePoint(m_nSelectWallShapePoint,ptDiffPos[VX],ptDiffPos[VY],0.0);
						Invalidate(FALSE);
					}
					else if(m_bMovingSelectWallShape&& m_nSelectWallShape >= 0)
					{						
						WallShape * pWall  =GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
						if(pWall && !pWall->IsEditMode() )
						{
							//pWall->Move(ptDiffPos[VX],ptDiffPos[VY],0.0);
							GetDocument()->OnMoveProcs(ptDiffPos[VX],ptDiffPos[VY],point,this);
							Invalidate(FALSE);
						}						
					}
				}
				else if (m_bMovingSelectHoldshortLine)
				{
					std::vector<CTaxiInterruptLine3D* > pHoldshortLine3DList=GetAirside3D()->GetHoldShortLine3DList();
					pHoldshortLine3DList[m_nSelectHoldshortLine]->MoveLocation(ptDiffPos[VX],ptDiffPos[VY],0.0);
					Invalidate(FALSE);
				}
				else if (m_bMovingStructure || m_bMovingStructurePoint  ){
					//Move SelectedStruct 2005-9-1
					CStructure* pStrucuture = NULL;
					CStructureList * pSlist=NULL;
					pSlist=&(GetDocument()->GetCurStructurelist());
					if( (m_nSelectedStructure <(int) pSlist->getStructureNum())&& m_nSelectedStructure>=0 )
						pStrucuture =pSlist->getStructureAt(m_nSelectedStructure);
					else pStrucuture=NULL;
					
					if (pStrucuture != NULL)
					{
						if(m_bMovingStructurePoint)
						{
							pStrucuture->MovePoint(m_nSelectedStructurePoint,ptDiffPos[VX],ptDiffPos[VY]);
							
						}else if(m_bMovingStructure)
						{
							//pStrucuture->Move(ptDiffPos[VX],ptDiffPos[VY]);
							GetDocument()->OnMoveProcs(ptDiffPos[VX],ptDiffPos[VY],point,this);
							
						}
						Invalidate(FALSE);
					}
					
				}
				/*else if(GetDocument()->GetLandsideView()->m_bMovingCtrlPoint == true)
				{
					if(GetDocument()->GetSelectProcessors().GetCount() > 0) 
					{
						CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
						if(pObjectDisplay&& pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
						{
							CProcessor2 * Proc2 = (CProcessor2*)pObjectDisplay;
							LandfieldProcessor * proc =(LandfieldProcessor *) Proc2->GetProcessor();
							proc->OffsetControlPoint(ptDiffPos[VX],ptDiffPos[VY],GetDocument()->GetLandsideView()->m_nCtrlPointsId);
							Proc2->GetRenderer()->Update();	
						}

					}
					
				}*/
				else if(!GetDocument()->m_bLayoutLocked )
				{
					for(SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin();itr!=GetParentFrame()->m_vSelected.end();itr++){
						(*itr)->OnMove(ptDiffPos[VX],ptDiffPos[VY],0);	
						bAfterMovingObject =true;
						
					}
					m_ptMove += ptDiffPos;
				}			
				
				//move selected control point of processor
				if(!GetDocument()->m_bLayoutLocked )
				{
					for(SelectableList::iterator itrProcCtrlPoint = m_listProcCtrlPoint.begin();itrProcCtrlPoint != m_listProcCtrlPoint.end();itrProcCtrlPoint++)
						(*itrProcCtrlPoint)->OnMove(ptDiffPos[VX],ptDiffPos[VY],0);	
				}
			
				Invalidate(FALSE);
			}
			break;
		}
	}
	CView::OnMouseMove(nFlags, point);
}

void C3DView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDown(nFlags, point);
	m_ptMousePos = Get3DMousePos(point);
	m_ptFrom = point;

	// TRACE("LB Down, point = {%d,%d}, m_ptMousePos = {%.2f, %.2f}\n", point.x, point.y, m_ptMousePos[VX], m_ptMousePos[VY]);					
	if(::GetAsyncKeyState(VK_MENU)>=0) 
	{//ALT is NOT pressed 
		switch(m_eMouseState)
		{
		case NS3DViewCommon::_pan:
		case NS3DViewCommon::_dolly:
		case NS3DViewCommon::_tumble:
			SetCapture();
			m_bTrackLButton = TRUE;
			break;
		case NS3DViewCommon::_floormanip:
			SetCapture();
			m_bTrackLButton = TRUE;
			break;
		case NS3DViewCommon::_getonerect:
			SetCapture();
			m_vMousePosList.push_back(m_ptMousePos);
			break;
		case NS3DViewCommon::_placemarker:
		case NS3DViewCommon::_placeproc:
		case NS3DViewCommon::_scaleproc:
		case NS3DViewCommon::_scaleprocX:
		case NS3DViewCommon::_scaleprocY:
		case NS3DViewCommon::_scaleprocZ:
		case NS3DViewCommon::_rotateproc:
		case NS3DViewCommon::_rotateairsideobject:
		case NS3DViewCommon::_moveprocs:
		case NS3DViewCommon::_rotateshapes:
		case NS3DViewCommon::_moveshapes:
		case NS3DViewCommon::_getonepoint:
		case NS3DViewCommon::_getmanypoints:
			SetCapture();
			break;
		case NS3DViewCommon::_pickgate:
		case NS3DViewCommon::_pickproc:		
		case NS3DViewCommon::_default:
			GLuint idx[SELECTION_MAX_HITS];
			memset(idx,-1,SELECTION_MAX_HITS);
			//Added by Tim In 2/9/2003*********************************************
			CSize sizeSelection=CSize(2,2);
			m_bDragSelect = TRUE;
			BOOL bDragSelect=FALSE;
			CTermPlanDoc* pDoc=GetDocument();
			BeginDragSelect(nFlags,pDoc,bDragSelect,point,sizeSelection);
			//*******************************************************************
			if(!m_bDragSelect)
				SetCapture();
			int nSelectCount;
			m_SelectionMap.Clear();
			SelectProc(nSelectCount,nFlags,point,idx,pDoc,(m_eMouseState==NS3DViewCommon::_pickgate)?GateProc:-1,sizeSelection,bDragSelect);		
			

			if(!(nSelectCount>0)&&!(nFlags&MK_CONTROL))
			{
				GetDocument()->UnSelectAllProcs();
				m_bMovingProc = FALSE;
			}


			//new code		
			if(! (nFlags& MK_CONTROL) ) GetParentFrame()->UnSelectAll();

			//add selected control point of processor.
			if(GetDocument()->m_systemMode == EnvMode_Terminal && GetDocument()->m_eAnimState == CTermPlanDoc::anim_none )
			{		
				m_listProcCtrlPoint.clear();
				for(int i = 0;i < nSelectCount;i++)
				{
					int id = idx[i];
					Selectable * pSel = m_SelectionMap.GetSelectable(id);
					if(pSel)
					{
						m_listProcCtrlPoint.insert( pSel );
						pSel->Selected(true);
						/*CProcessorPathControlPoint * pProcPathCtrlPointSel = (CProcessorPathControlPoint *)pSel;
						if(pProcPathCtrlPointSel)
						{
							if(pProcPathCtrlPointSel->m_pOwner)
								pProcPathCtrlPointSel->m_pOwner->Select(TRUE);
						}*/
					}
				}
			}

			if(GetDocument()->m_systemMode == EnvMode_AirSide && GetDocument()->m_eAnimState == CTermPlanDoc::anim_none ){
				SelectableList selected;
				int nSelectedCnt = SelectAirsideScene(nFlags,point.x,point.y,sizeSelection,GetDocument()->m_SelectSettings, selected);
				{	
					//if(selected.size()>0)
					//	m_ptAltObjectCmdPosStart = selected[0]->GetLocation();
					//ARCPoint3 arcpoint3(0.0,0.0,0.0);
					//m_ptMove = arcpoint3;
					for(SelectableList::iterator itr= selected.begin();itr!=selected.end();itr++){
						GetParentFrame()->Select( (*itr).get() );							
					}
				}
			}
			


			Invalidate(FALSE);
			GetDocument()->UpdateAllViews(this);
			m_bDragSelect = FALSE;
			if(bDragSelect)
			{
				if(m_eMouseState==NS3DViewCommon::_pickproc||m_eMouseState==NS3DViewCommon::_pickgate)
				{
					OnLButtonUp(nFlags,point);
				}
			}
			break;
		};
	}
	else 
	{
		SetCapture();
	}
	m_ptMousePosWithZ = Get3DMousePos(point,TRUE);//have to call this line again with TRUE param.
}

static void inline SelectLandsideProcessor(C3DView * pView,UINT nFlags, CPoint point)
{
	GLuint selidx[SELECTION_MAX_HITS];
	memset(selidx,-1,SELECTION_MAX_HITS);
	CSize sizeSelection=CSize(2,2);
	int nSelectCount = pView->SelectScene(nFlags, point.x, point.y, selidx);
	if(nSelectCount > 0)
	{
		int idx = NewGetSelectIdx(selidx[0]);
		int subidx = NewGetSubSelectIdx(selidx[0]);
		BYTE seltype = GetSelectType(selidx[0]);
		BYTE selsubtype = GetSelectSubType(selidx[0]);
		if(SELTYPE_LANDSIDEPROCESSOR == seltype && SELSUBTYPE_MAIN == selsubtype)
		{
			Processor * pProc = pView->GetDocument()->GetProcWithSelName(idx)->GetProcessor();
			if(pProc->getProcessorType()!=StretchProcessor) return;
			 
			LandProcPart newProcpart;
			newProcpart.pProc = pProc;
			newProcpart.part = subidx;
			newProcpart.pos = 0;
			pView->m_vSelectLandProcs.push_back(newProcpart);
		}
	}
}

static void inline PickPositionOnStretch(C3DView * pView,UINT nFlags, CPoint point )
{
	GLuint selidx[SELECTION_MAX_HITS];
	memset(selidx,-1,SELECTION_MAX_HITS);
	CSize sizeSelection = CSize(2,2);

	SelectableSettings selsetting;
	selsetting.SetSelectable(ALT_UNKNOWN, FALSE);
	selsetting.SetSelectable(ALT_STRETCH, TRUE);
	SelectableList reslt; 
	pView->SelectAirsideScene(nFlags, point.x, point.y, CSize(2,2), selsetting, reslt);
	
	if(reslt.size())
	{
		Selectable  *pSel = reslt.at(0).get();
		if(pSel && pSel->GetSelectType() == Selectable::ALT_OBJECT )
		{
			ALTObject3D * pObj3D = (ALTObject3D*)pSel;
			if(pObj3D->GetObjectType() == ALT_STRETCH)
			{
				CStretch3D  *pStretch3D = (CStretch3D* ) pObj3D;
				Path stretchPath = pStretch3D->GetStretch()->GetPath();
				
				ARCRay pickRay = GetMousePickRay(pView,point);

				DistanceRay3Path3 distrayPath(pickRay,stretchPath);
				
				DistanceUnit stretchWidth = pStretch3D->GetStretch()->GetLaneWidth() * pStretch3D->GetStretch()->GetLaneNumber();
				if( distrayPath.GetSquared(0) < stretchWidth * stretchWidth ) // 
				{
					InsecObjectPart newPart;
					newPart.SetObject(pStretch3D->GetStretch());
					newPart.pos = distrayPath.m_fPathParameter;
					ARCVector3 vPath = stretchPath.GetDirection((float)newPart.pos);
					ARCVector3 vPoint = pickRay.GetDistPoint(distrayPath.m_fRayParameter) - stretchPath.GetRelateDistPoint((float)newPart.pos);
					
					if( ( vPath ^ vPoint )[VZ] >0 )
					{
						newPart.part = 0;
					}
					else
					{
						newPart.part = 1;
					}

					pView->m_vSelectedStretchPos.push_back(newPart);	
				}				
			}
		}
	}
	
	

}

void C3DView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//release 
	m_nLastSelRailPt = -1;
	m_nLastSelRailWay = -1;
	//
	if(::GetAsyncKeyState(VK_MENU)>=0) {//ALT is NOT pressed 
		switch(m_eMouseState)
		{
		case NS3DViewCommon::_pan:
		case NS3DViewCommon::_dolly:
		case NS3DViewCommon::_tumble:
			ReleaseCapture();
			break;
		case NS3DViewCommon::_floormanip:
			ReleaseCapture();
			break;
		case NS3DViewCommon::_placeproc:
		case NS3DViewCommon::_scaleproc:
		case NS3DViewCommon::_scaleprocX:
		case NS3DViewCommon::_scaleprocY:
		case NS3DViewCommon::_scaleprocZ:
		case NS3DViewCommon::_rotateproc:
		case NS3DViewCommon::_rotateairsideobject:
		case NS3DViewCommon::_moveprocs:
		case NS3DViewCommon::_moveshapes:
		case NS3DViewCommon::_moveshapesZ:
		case NS3DViewCommon::_rotateshapes:
			ReleaseCapture();
			OnAltObjectEditFinished();
			OnProcEditFinished();
			m_eMouseState = NS3DViewCommon::_default;
			break;
		case NS3DViewCommon::_placemarker:
			ReleaseCapture();
			m_eMouseState = NS3DViewCommon::_default;
			//set marker location to active floor
			GetDocument()->SetActiveFloorMarker(m_ptMousePos);
			if(GetDocument()->GetCurrentMode() == EnvMode_Terminal)
			    GetDocument()->ActivateFloor(GetDocument()->m_nLastActiveFloor);
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_distancemeasure:
			m_vMousePosList.push_back(m_ptMousePos);
			break;
		case NS3DViewCommon::_gettwopoints:
		case NS3DViewCommon::_getmirrorline:
		case NS3DViewCommon::_getalignline:
		case NS3DViewCommon::_getarrayline:
			m_vMousePosList.push_back(m_ptMousePos);
			if(m_vMousePosList.size() == 2) {
				if(m_eMouseState==NS3DViewCommon::_gettwopoints)
					::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
				else if(m_eMouseState==NS3DViewCommon::_getmirrorline)
					GetDocument()->MirrorSelectedProcessors(m_vMousePosList);
				else if(m_eMouseState==NS3DViewCommon::_getarrayline) {
					GetDocument()->ArraySelectedProcessors(m_vMousePosList, 5);
				}
				else if(m_eMouseState == NS3DViewCommon::_getalignline)
				{
					GetDocument()->AlignSelectedProcessors(m_vMousePosList);
				}
				m_eMouseState = NS3DViewCommon::_default;
			}
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_getonepoint:
			ReleaseCapture();
			m_vMousePosList.push_back(m_ptMousePos);
			::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = NS3DViewCommon::_default;
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_getmanypoints:
			ReleaseCapture();
			m_vMousePosList.push_back(m_ptMousePos);
			SelectLandsideProcessor(this,nFlags,point);
			Invalidate(FALSE);
			
			break;
		case NS3DViewCommon::_selectlandproc:
			{
				m_vMousePosList.push_back(m_ptMousePos);				
				SelectLandsideProcessor(this,nFlags,point);
			}
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_pickstretchpos:
			{
				m_vMousePosList.push_back(m_ptMousePos);
				PickPositionOnStretch(this, nFlags, point);
			}
			Invalidate(FALSE);
			break;		
		case NS3DViewCommon::_pickHoldLine:
			{
				SelectableList selected;
				SelectAirsideScene(nFlags,point.x,point.y,CSize(2,2),GetDocument()->m_SelectSettings, selected);
				{
					for(size_t i=0;i<selected.size();i++)
					{
						Selectable* pSel = selected[i].get();
						if( pSel->GetSelectType() == Selectable::HOLD_LINE )
						{
							HoldPosition3D* pHold = (HoldPosition3D*)pSel;
							::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM)&pHold->GetHoldPosition(), m_lFallbackParam);
							m_eMouseState = NS3DViewCommon::_default;
							Invalidate(FALSE);
							break;
						}
					}
				}
			}
			break;
		case NS3DViewCommon::_pickAltobject:
			{
				SelectableList selected;
				SelectAirsideScene(nFlags,point.x,point.y,CSize(2,2),GetDocument()->m_SelectSettings, selected);
				{
					for(size_t i=0;i<selected.size();i++)
					{
						Selectable* pSel = selected[i].get();
						if( pSel->GetSelectType() == Selectable::ALT_OBJECT )
						{
							ALTObject3D* pObject3D = (ALTObject3D*)pSel;
							m_vMousePosList.push_back(m_ptMousePos);
							::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM)pObject3D->GetObject(), (LPARAM) &m_vMousePosList);
							m_eMouseState = NS3DViewCommon::_default;
							Invalidate(FALSE);
							break;
						}
					}
				}
			}
			break;
		case NS3DViewCommon::_getonerect:
			ReleaseCapture();
			::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = NS3DViewCommon::_default;
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_pickgate:
		case NS3DViewCommon::_pickproc:
			ReleaseCapture();
			::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = NS3DViewCommon::_default;
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_pasteproc:
			{
/*				COleDataObject dataObject;
				VERIFY(dataObject.AttachClipboard());
				ARCVector3 arcVector=Get3DMousePos(point,FALSE,m_nPasteToFloorIndex);
				LoadFromPaste(&dataObject,arcVector);
*/				m_eMouseState = NS3DViewCommon::_default;
				Invalidate(FALSE);
			}
			break;
		case NS3DViewCommon::_default:
			ReleaseCapture();
			if(m_bMovingProc && GetDocument()->GetSelectProcessors().GetCount() > 0) 
			{				

				const ARCVector3& vTo = GetDocument()->GetSelectedObjectDisplay(0)->GetLocation();
				const ARCVector3& vFrom = GetDocument()->m_ptProcCmdPosStart;
				const ARCVector3 vMove = GetDocument()->GetSelectedObjectDisplay(0)->GetLocation()-GetDocument()->m_ptProcCmdPosStart;
				// TRACE("Move Proc from (%.2f, %.2f) to (%.2f, %.2f)\n", vFrom[VX], vFrom[VY], vTo[VX], vTo[VY]);
				GetDocument()->m_CommandHistory.Add(new MoveProcessorsCommand(GetDocument()->GetSelectProcessors(), GetDocument(), vMove[VX], vMove[VY]));
				
				//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
				{
					CString strLog = _T("");
					strLog = _T("Move Processor;");
					ECHOLOG->Log(RailWayLog,strLog);
				}
				//save data set
				CString sPath = GetDocument()->m_ProjInfo.path;
				GetDocument()->GetCurrentPlacement()->saveDataSet( sPath, false);
				GetDocument()->GetProcessorList().saveDataSet(sPath, true);
				GetDocument()->GetTerminal().pRailWay->saveDataSet( sPath, false );
				GetDocument()->GetTerminal().m_pAllCarSchedule->saveDataSet( sPath,false );
				GetDocument()->m_portals.saveDataSet(sPath,false);
				GetDocument()->GetTerminal().m_pAreas->saveDataSet( sPath,false );
				GetDocument()->GetTerminal().m_pStructureList->saveDataSet( sPath,false );
				GetDocument()->GetTerminal().m_pPipeDataSet->saveDataSet( sPath,false );
				GetDocument()->GetTerminal().m_pWallShapeList->saveDataSet( sPath,false );


				
				m_bMovingProc = FALSE;			

				Invalidate(FALSE);
				GetDocument()->UpdateAllViews(this);
			}
			if(m_bMovingRailPoint) {
				//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
				{
					CString strLog = _T("");
					strLog = _T("Move Point;");
					ECHOLOG->Log(RailWayLog,strLog);
				}
				m_bMovingRailPoint = FALSE;
				GetDocument()->GetTerminal().m_pAllCarSchedule->AdjustRailWay( &(GetDocument()->m_trainTraffic));
				GetDocument()->GetTerminal().pRailWay->saveDataSet( GetDocument()->m_ProjInfo.path, false );
				GetDocument()->GetTerminal().m_pAllCarSchedule->saveDataSet( GetDocument()->m_ProjInfo.path,false );
			}
			if(m_bMovingPipePoint || m_bMovingPipeWidthPoint) {
				m_bMovingPipePoint = FALSE;
				m_bMovingPipeWidthPoint = FALSE;
				GetDocument()->GetTerminal().m_pPipeDataSet->saveDataSet( GetDocument()->m_ProjInfo.path, false );
			}
			//2005-9-1
			if(m_bMovingStructure||m_bMovingStructurePoint){
				m_bMovingStructure=FALSE;
				m_bMovingStructurePoint=FALSE;
				GetDocument()->UpdateStructureChange(false);
			}
			if(!m_bProcHasMoved && (nFlags&MK_CONTROL)) {
				GetDocument()->DoLButtonUpOnProc(m_nLastSelIdx,nFlags&MK_CONTROL);
				Invalidate(FALSE);
			}			
		
			
			//reset selectWallShape
			if(m_bMovingSelectWallShape || m_bMovingSelectWallShapePoint){
				m_bMovingSelectWallShapePoint=FALSE;
				m_bMovingSelectWallShape=FALSE;
				m_nSelectWallShape=-1;
				m_nSelectWallShapePoint=-1;
				CString sPath = GetDocument()->m_ProjInfo.path;				
				GetDocument()->GetCurWallShapeList().saveDataSet(sPath, true);	
			}	

			// reset hold short line
			if (m_bMovingSelectHoldshortLine)
			{
				m_bMovingSelectHoldshortLine=FALSE;
				std::vector<CTaxiInterruptLine3D* >pHoldShortLine3dList = GetAirside3D()->GetHoldShortLine3DList();
                pHoldShortLine3dList[m_nSelectHoldshortLine]->FlushChangeOf();
                m_nSelectHoldshortLine = -1;
			}

			//edit ALTObject
			if(bAfterMovingObject && !GetDocument()->m_bLayoutLocked ){
				
				ALTObject3DList vChangedObjs;

				for(SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin(); itr!= GetParentFrame()->m_vSelected.end(); ++itr){
										
					switch( (*itr)->GetSelectType() )
					{
					case Selectable::ALT_OBJECT :
						{
							ALTObject3D * pObject = (ALTObject3D * )(*itr).get();						
							if (pObject->GetObject() && !pObject->GetObject()->GetLocked())
							{
								vChangedObjs.push_back(pObject);
							}
						}							
						break;
					case Selectable::ALT_OBJECT_PATH_CONTROLPOINT : 
						{
							ALTObjectPathControlPoint * pCtrlPt = (ALTObjectPathControlPoint*) (*itr).get();
							ALTObject3D * pObject = pCtrlPt->GetObject();
							if (pObject->GetObject() && !pObject->GetObject()->GetLocked())
							{
								vChangedObjs.push_back(pObject);
							}
						}						
					default :
						NULL;						
					}		
					(*itr)->AfterMove();                    
				}	

				if( vChangedObjs.size() )
				{
					GetAirside3D()->ReflectChangeOf(vChangedObjs);
				}
				for(int i=0;i< (int)vChangedObjs.size();i++ )
				{
					ALTObject3D* pObject3D = vChangedObjs.at(i).get();
					if( pObject3D )
						pObject3D->FlushChange();
				}
				
				ALTObject3DList altobject3dlist;
				(GetParentFrame()->m_vSelected).GetSelectObjects(altobject3dlist);
				GetDocument()->m_CommandHistory.Add(new MoveALTObjectCommand(altobject3dlist, GetDocument(), m_ptMove[VX], m_ptMove[VY]));
				GetDocument()->UpdateAllViews(NULL,VM_MODIFY_HOLDSHORTLINE,NULL);
				Invalidate(FALSE);
			}
			bAfterMovingObject = false;			
			m_listProcCtrlPoint.clear();
			break;
		};
	}
	else {
		ReleaseCapture();
	}
	m_bTrackLButton = FALSE;
}


void C3DView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CView::OnLButtonDblClk(nFlags, point);
}

void C3DView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	SetCapture();
	CView::OnMButtonDown(nFlags, point);
}

void C3DView::OnMButtonUp(UINT nFlags, CPoint point) 
{
	ReleaseCapture();
	CView::OnMButtonUp(nFlags, point);
}

void C3DView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CView::OnRButtonUp(nFlags, point);
	ReleaseCapture();
	if(::GetAsyncKeyState(VK_MENU)>=0) {//ALT is NOT pressed
		switch(m_eMouseState)
		{
		case NS3DViewCommon::_gettwopoints:
		case NS3DViewCommon::_getmirrorline:
		case NS3DViewCommon::_getarrayline:
		case NS3DViewCommon::_getalignline:
			m_vMousePosList.push_back(m_ptMousePos);
			if(m_vMousePosList.size() == 2) {
				if(m_eMouseState==NS3DViewCommon::_gettwopoints)
					::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
				else if(m_eMouseState==NS3DViewCommon::_getmirrorline)
					GetDocument()->MirrorSelectedProcessors(m_vMousePosList);
				else if(m_eMouseState==NS3DViewCommon::_getarrayline) {
					GetDocument()->ArraySelectedProcessors(m_vMousePosList, 5);
				}
				else if(m_eMouseState == NS3DViewCommon::_getalignline)
				{
					GetDocument()->AlignSelectedProcessors(m_vMousePosList);
				}
				m_eMouseState = NS3DViewCommon::_default;
			}
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_distancemeasure:
			m_vMousePosList.push_back(m_ptMousePos);
			ShowDistanceDialog();
			m_eMouseState = NS3DViewCommon::_default;
			Invalidate(FALSE);
			break;
		case NS3DViewCommon::_getmanypoints:
			m_vMousePosList.push_back(m_ptMousePos);			
			SelectLandsideProcessor(this,nFlags,point);
			::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vMousePosList, m_lFallbackParam);
			m_eMouseState = NS3DViewCommon::_default;			
			Invalidate(FALSE);
			
			break;
		case NS3DViewCommon::_selectlandproc:
			{		
				SelectLandsideProcessor(this,nFlags,point);
				::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vSelectLandProcs, m_lFallbackParam);
				m_eMouseState = NS3DViewCommon::_default;
				Invalidate(FALSE);
				break;
			}
		case NS3DViewCommon::_pickstretchpos:
			{
				PickPositionOnStretch(this, nFlags,point);
				::SendMessage(m_hFallbackWnd, TP_DATA_BACK, (WPARAM) &m_vSelectedStretchPos, m_lFallbackParam);
				m_eMouseState = NS3DViewCommon::_default;
				Invalidate(FALSE);
				break;
			}
		};
	}
}

void C3DView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CView::OnRButtonDown(nFlags, point);
	m_ptPickForCopy=m_ptMousePos = Get3DMousePos(point);
	//// TRACE("RB Down, point = {%d,%d}, m_ptMousePos = {%.2f, %.2f}\n", point.x, point.y, m_ptMousePos[VX], m_ptMousePos[VY]);
	SetCapture();
	m_procCurEditCtrlPoint  = 0;
	if(::GetAsyncKeyState(VK_MENU)>=0) 
	{
		//ALT is NOT pre	ssed
		if(((CMainFrame*)AfxGetMainWnd())->m_bCanPasteProcess)
		{
			m_eMouseState = NS3DViewCommon::_pasteproc;
		}
		switch(m_eMouseState)
		{
		case NS3DViewCommon::_pan:
		case NS3DViewCommon::_dolly:
		case NS3DViewCommon::_tumble:
		case NS3DViewCommon::_floormanip:
		case NS3DViewCommon::_placeproc:
		case NS3DViewCommon::_scaleproc:
		case NS3DViewCommon::_scaleprocX:
		case NS3DViewCommon::_scaleprocY:
		case NS3DViewCommon::_scaleprocZ:
		case NS3DViewCommon::_rotateproc:
		case NS3DViewCommon::_rotateairsideobject:
		case NS3DViewCommon::_moveprocs:
		case NS3DViewCommon::_getmirrorline:
		case NS3DViewCommon::_getarrayline:
		case NS3DViewCommon::_getalignline:
		case NS3DViewCommon::_getonepoint:
		case NS3DViewCommon::_gettwopoints:
		case NS3DViewCommon::_getmanypoints:
		case NS3DViewCommon::_getonerect:
			break;
		case NS3DViewCommon::_pasteproc:
			{
				CPoint pt(point);
				m_ptRMouseDown = point;
				ClientToScreen(&pt);
				CNewMenu menu, *pCtxMenu = NULL;
		
				GLuint selidx[SELECTION_MAX_HITS];
				memset(selidx,-1,SELECTION_MAX_HITS);

				if( 0 == SelectScene(nFlags, point.x, point.y, selidx) ) 
				{
					menu.LoadMenu(IDR_CTX_RSELECTNONE);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME);
						pCtxMenu->SetMenuTitle(_T("Processor"), MFT_GRADIENT|MFT_TOP_TITLE);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
				
					pCtxMenu->EnableMenuItem(ID_PROCESS_PASTE, MF_ENABLED);
					pCtxMenu->EnableMenuItem(ID_CLEAR_CLIPBOARD, MF_ENABLED);
					break;
				}
			}
		case NS3DViewCommon::_default:
			GLuint selidx[SELECTION_MAX_HITS];
			memset(selidx,-1,SELECTION_MAX_HITS);
			int nHits;
			CPoint pt(point);
			ClientToScreen(&pt);
			CNewMenu menu, *pCtxMenu = NULL;
			if(m_bPickConveyor)
			{
				if(SelectScene(nFlags, point.x, point.y, selidx)==1)
				{
					Processor* pProc=GetDocument()->GetProcWithSelName(selidx[0])->GetProcessor();
					if(pProc->getProcessorType()==ConveyorProc) 
					{
						GetDocument()->SelectProc(selidx[0]);// Automatic Call UnSelectAllProc Inside
						menu.LoadMenu(IDR_MENU_POPUP);
						pCtxMenu =DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(43));
						pCtxMenu->SetMenuTitle(_T("Processor"),MFT_GRADIENT|MFT_TOP_TITLE);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
					
				}
				break;
			}
			if(GetDocument()->m_eAnimState != CTermPlanDoc::anim_none && GetDocument()->m_bMobileElementDisplay)
			{
				if( 0 < (nHits = SelectScene(nFlags, point.x, point.y, selidx)) )
				{
					m_nSelectedAircraft = GetSelectIdx(selidx[0]);
					menu.LoadMenu(IDR_CTX_AIRCRAFTDISPLAY);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
						pCtxMenu->SetMenuTitle(_T("Aircraft Display"),MFT_GRADIENT|MFT_TOP_TITLE);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
				}
				
				break;
			}
			if( 0 < (nHits = SelectScene(nFlags, point.x, point.y, selidx)) ) 
			{
				int idx = GetSelectIdx(selidx[0]);
				BYTE seltype = GetSelectType(selidx[0]);
				BYTE selsubtype = GetSelectSubType(selidx[0]);

				// TRACE("%d Hits, First sel: %u\n", nHits, selidx[0]);
				m_bMovingRailPoint = FALSE;
				m_bMovingPipePoint = FALSE;
				m_bMovingPipeWidthPoint = FALSE;
				//Modified by Tim In 2/8/2003*********************
				if(SELTYPE_PROCESSOR == seltype)	//selection is a processor
				{
					if(!(nFlags&MK_CONTROL)) 
					{
						//Modified by Tim In 2/8/2003****************************
						if(!GetDocument()->GetProcWithSelName(idx)->IsSelected()) 
						{
							GetDocument()->SelectProc(idx); //select proc at idx only and bring up ctx menu.
						}
						//****************************************************
						Invalidate(FALSE);
						GetDocument()->UpdateAllViews(this);
						CProcessor2 *pProc=GetDocument()->GetProcWithSelName(idx);
						m_procCurEditCtrlPoint = pProc;
						if(pProc==NULL)return;						
						else//other processors
						{
							//menu.LoadMenu(IDR_CTX_PROCESSORPICK);
							CMenu mnuTemp;
							mnuTemp.LoadMenu(IDR_CTX_PROCESSORPICK);

							CMenu* pmnuChild = mnuTemp.GetSubMenu(0);

							//	add projects to copy menu
							CMenu mnuPopup;
							CMenu* pmnuDocs = NULL;
							if (pmnuChild)
							{
								mnuPopup.CreateMenu();

								std::vector<CTermPlanDoc*>	vDocs;
								int nDocCount = m_cpmi.GetAppDocuments(vDocs);

								if (nDocCount)
								{
									pmnuDocs = new CMenu[nDocCount];
									int nIndex = 0;
									for (int i = 0; i < nDocCount; i++)
									{
										pmnuDocs[i].CreateMenu();
										//pmnuDocs[i].SetMenuType(MIT_XP);
										//pmnuDocs[i].SetBmpSize(0, 0);
										//pmnuDocs[i].SetMenuWidth(100);
										//pmnuDocs[i].SetMenuHeight(23);

										std::vector<CFloor2*> vFloors;
										int nFloorCount = m_cpmi.GetFloorsByDocument(vDocs[i], vFloors);
										for (int j = 0; j < nFloorCount; j++)
										{
											//pmnuDocs[i].AppendMenu(MF_ENABLED|MF_MENUBARBREAK|MF_STRING, CHANGE_SHAPES_MENU_LIST_ID_BEGIN + j,
											//	vFloors[j]->FloorName());
											pmnuDocs[i].AppendMenu(MF_ENABLED | MF_STRING,
												MENU_FLOOR_ID + nIndex, vFloors[j]->FloorName());
											nIndex++;
										}

										mnuPopup.AppendMenu(MF_POPUP, (UINT)pmnuDocs[i].m_hMenu, vDocs[i]->GetTitle());
									}

									//delete[] pmnuDocs;
								}

								pmnuChild->ModifyMenu(5, MF_BYPOSITION|MF_POPUP, (UINT)mnuPopup.m_hMenu, "Copy Processor to");
							}

							menu.LoadMenu(mnuTemp.m_hMenu);
							pCtxMenu =DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));

							if(pCtxMenu != NULL)
							{
								//Added by Tim In 2/13/2003*************************************************
								CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
								CString strIniDir=PROJMANAGER->GetAppPath()+"\\Databases\\Shapes\\";
								CIniReader IniReader(strIniDir+"Shapes.INI");
								CStringList* pStrL = IniReader.getSectionData("Sections");

								POSITION position;
								CString strLeft,strRight,str;
								int nCount=0;
								CNewMenu newSubMenu;
								newSubMenu.CreateMenu();

								CMenuCH* pMenuCH=new CMenuCH[ pStrL->GetCount()];

								for(position = pStrL->GetHeadPosition(); position != NULL; ) 
								{
									str=pStrL->GetNext(position);
									strLeft=IniReader.GetDataLeft(str) ;

									pMenuCH[nCount].CreateMenu();
									pMenuCH[nCount].SetMenuType(MIT_XP);
									pMenuCH[nCount].SetBmpSize(32,32);
									pMenuCH[nCount].SetMenuHeight(60);
									pMenuCH[nCount].SetMenuWidth(52);
									strRight=IniReader.GetDataRight(str);
									int nInt[128];
									int nIntCount=CIniReader::DispartStrToSomeInt(strRight,nInt);
									for(int i=0;i<nIntCount;i++)
									{
										HBITMAP hBitmap = (HBITMAP) ::LoadImage(NULL,pSL->at(nInt[i])->ImageFileName() , IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
										if(i%CHANGE_SHAPES_MENU_LIST_ITEMCOUNT_PER_COLUMN==0)
											pMenuCH[nCount].AppendMenu(MF_ENABLED|MF_MENUBARBREAK,
											CHANGE_SHAPES_MENU_LIST_ID_BEGIN+nInt[i],pSL->at(nInt[i])->Name(),
											hBitmap);

										else
											pMenuCH[nCount].AppendMenu(MF_ENABLED,
											CHANGE_SHAPES_MENU_LIST_ID_BEGIN+nInt[i],pSL->at(nInt[i])->Name(),
											hBitmap);
									}
									if(nIntCount>CHANGE_SHAPES_MENU_LIST_ITEMCOUNT_PER_COLUMN)
										for(int k=0;k<(CHANGE_SHAPES_MENU_LIST_ITEMCOUNT_PER_COLUMN-nIntCount%CHANGE_SHAPES_MENU_LIST_ITEMCOUNT_PER_COLUMN);k++)
											pMenuCH[nCount].AppendMenu(MF_DISABLED,0,"",(HBITMAP)NULL);



									newSubMenu.AppendMenu(MF_POPUP,(UINT)pMenuCH[nCount].m_hMenu,strLeft);
									nCount++;
								}	
								pCtxMenu->InsertMenu(9, MF_BYPOSITION|MF_POPUP,(UINT)newSubMenu.m_hMenu,"Change Shapes");
								//************************************************************************
								pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
								pCtxMenu->SetMenuTitle(_T("Processor"),MFT_GRADIENT|MFT_TOP_TITLE);
								pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
								delete[] pMenuCH;
								if (pmnuDocs)
									delete[] pmnuDocs;
							}
						}
					}
						
				}
				//else if(SELTYPE_LANDSIDEPROCESSOR == seltype)
				//{
				//	CProcessor2 * pProc2 = GetDocument()->GetLandsideDoc()->GetPlacementPtr()->m_vDefined[ NewGetSelectIdx(selidx[nHits-1]) ];
				//	//GetDocument()->GetSelectProcessors().push_back(pProc2);
				//	GetDocument()->GetLandsideDoc()->SelectProcessor(pProc2,false);
				//	menu.LoadMenu(IDR_CTX_LANDPROCESSORPICK);
				//	if(selsubtype == SELSUBTYPE_POINT)
				//	{
				//		pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(2));						
				//		if(pCtxMenu != NULL)
				//		{
				//			pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
				//			pCtxMenu->SetMenuTitle(_T("Point Edit"),MFT_GRADIENT|MFT_TOP_TITLE);							
				//			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
				//		}
				//	}
				//	else if(selsubtype == SELSUBTYPE_MAIN)
				//	{					
				//		
				//		pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
				//		if(pCtxMenu != NULL)
				//		{
				//			pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
				//			pCtxMenu->SetMenuTitle(_T("Processor"),MFT_GRADIENT|MFT_TOP_TITLE);							
				//			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
				//		}
				//	}
				//	

				//}
				else if(SELTYPE_RAILWAY == seltype) //selection is a railway or a railway point
				{
					int nSubmenu = 0;
					int nRailIdx =-1;
					//railway or railway point selected
					if(SELSUBTYPE_MAIN == selsubtype)
					{
						//railway... show menu to enable/diasable railway edit
						nRailIdx = idx;
						if(m_nSelectedRailWay != nRailIdx)
							m_bSelRailEdit = FALSE;
						if(m_bSelRailEdit)
							nSubmenu = 1;
						m_nSelectedRailWay = nRailIdx;
					}
					else 
					{
						ASSERT( SELSUBTYPE_POINT == selsubtype );
						//railway point... show menu to enable/diasable railway edit
						int nRailPtIdx = idx;
						nRailIdx = m_nSelectedRailWay;
						if(m_bSelRailEdit)
							nSubmenu = 2;
						m_nSelectedRailPt = nRailPtIdx;
					}
					menu.LoadMenu(IDR_CTX_RAILWAY);
					pCtxMenu =DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(nSubmenu));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
						pCtxMenu->SetMenuTitle(_T("Railway"),MFT_GRADIENT|MFT_TOP_TITLE);

						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
				}
				else if(SELTYPE_PIPE == seltype) //selection is a pipe or a pipe point
				{
					if(SELSUBTYPE_MAIN == selsubtype) //selected pipe
					{
						int nPipeIdx = idx;
						if(m_nSelectedPipe != nPipeIdx)
							m_bSelPipeEdit = FALSE;
						m_nSelectedPipe = nPipeIdx;
						menu.LoadMenu(IDR_CTX_PIPE);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
						if(pCtxMenu != NULL)
						{
							pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
							pCtxMenu->SetMenuTitle(_T("Pipe"),MFT_GRADIENT|MFT_TOP_TITLE);
							GenerateDetachSubMenu(pCtxMenu);
							// TRACE("Pipe menu at %d,%d\n", pt.x, pt.y);
							pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
						}

					}
					else if(SELSUBTYPE_POINT == selsubtype) //selected pipe point
					{
						m_nSelectedPipePt = idx;
						CNewMenu menuPopup; 
						menuPopup.CreatePopupMenu();
						
						menuPopup.LoadToolBar(IDR_MAINFRAME); 
						menuPopup.SetMenuTitle(_T("Pipe"),MFT_GRADIENT|MFT_TOP_TITLE);
						menuPopup.AppendMenu(MF_POPUP,ID_BGEIN_CROSSWALK,_T("Begin Cross Walk"));
						menuPopup.AppendMenu(MF_POPUP,ID_END_CROSSWALK,_T("End Cross Walk"));
						
						menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
					else if(SELSUBTYPE_WIDTHPOINT == selsubtype) //selected pipe width point
					{
						m_nSelectedPipeWidthPt = idx;
						menu.LoadMenu(IDR_CTX_PIPE);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
						if(pCtxMenu != NULL)
						{
							pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
							pCtxMenu->SetMenuTitle(_T("Pipe"),MFT_GRADIENT|MFT_TOP_TITLE);
							GenerateDetachSubMenu(pCtxMenu);
							// TRACE("Pipe menu at %d,%d\n", pt.x, pt.y);
							pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
						}

					}
					else {
						ASSERT(FALSE);
					}
					
				}				
				else if(SELTYPE_WALLSHAPE == seltype ){
					if( SELSUBTYPE_MAIN ==selsubtype){
						m_nSelectWallShape=idx;
						//pop up menu to edit shape
						menu.LoadMenu(IDR_3DVIEW_WALLSHAPE);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
						if(pCtxMenu != NULL)
						{
							pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
							pCtxMenu->SetMenuTitle(_T("WallShape"),MFT_GRADIENT|MFT_TOP_TITLE);							
// 							if(! GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape)->IsEditMode()){
// 								pCtxMenu->EnableMenuItem(ID_WALLSHAPEMENU_ADDPOINTHERE,MF_GRAYED |MF_BYCOMMAND |MF_DISABLED);
// 							}
							pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
							
						}
						//disable add point
						
	
						//
					}else if(SELSUBTYPE_POINT ==selsubtype){						
						WallShapeList & walllist=GetDocument()->GetCurWallShapeList();
						size_t nwallCount=walllist.getShapeNum();
						int ptCount=0;
						for(size_t j=0;j<nwallCount;++j){
							WallShape* wall=walllist.getShapeAt(j);
							int wallPtCount=wall->GetPointCount();
							if(  idx < ptCount+wallPtCount ){
								m_nSelectWallShape=j ;
								m_nSelectWallShapePoint=idx-ptCount;
								m_bMovingSelectWallShapePoint=TRUE;
								break;
							}						
							ptCount+=wallPtCount;
						}
						//pop up a menu to delete select point
						menu.LoadMenu(IDR_3DVIEW_WALLSHAPE);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(1));
						if(pCtxMenu != NULL)
						{
							pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
							pCtxMenu->SetMenuTitle(_T("WallShape Point"),MFT_GRADIENT|MFT_TOP_TITLE);							
							pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
						}
						//
					}
				}
				//ben 2005-9-1
				else if(SELTYPE_STRUCTURE == seltype)//selection is a structure or a structure point 
				{
					if(SELSUBTYPE_MAIN == selsubtype)
					{
						unsigned int nStructureIdx = idx;
						if(m_nSelectedStructure != nStructureIdx)
							m_bSelectStructureEdit = FALSE;
						m_nSelectedStructure = nStructureIdx;
						
					}
					/*
					else//selected Structure point
					{
						ASSERT( SELSUBTYPE_POINT == GetSelectSubType(idx[nHits-1]) );
						m_nSelectedStructurePoint=GetSelectIdx(selidx[nHits-1]);
					}
					*/
					//menu will be added
					menu.LoadMenu(IDR_CTX_STRUCTURE);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
						pCtxMenu->SetMenuTitle(_T("SurfaceArea"),MFT_GRADIENT|MFT_TOP_TITLE);
						//// TRACE("Pipe menu at %d,%d\n", pt.x, pt.y);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}
					
				}
				
				//end add
				else if(SELTYPE_TERMINALTRACER == seltype)	//selection is a processor
				{			
					Invalidate(FALSE);
					GetDocument()->UpdateAllViews(this);

					// idx is the index of the flight in the logs
					MobLogEntry mle = GetDocument()->GetMobLogEntry(idx);
					// TRACE("right-click on track for: %s\n", mle.GetMobDesc().id);
				}
				else if(SELTYPE_AIRSIDETRACER == seltype)	//selection is a processor
				{			
					Invalidate(FALSE);
					GetDocument()->UpdateAllViews(this);
					
					// idx is the index of the flight in the logs
					AirsideFlightLogEntry afle = GetDocument()->GetAirsideFlightLogEntry(idx);
					// TRACE("right-click on track for: %s\n", afle.GetAirsideDesc().flightID);

					
				}
				else if(SELTYPE_TERMINALAREA == seltype)	//selection is a processor
				{
					menu.LoadMenu(IDR_CTX_AREAPORTAL);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
						pCtxMenu->SetMenuTitle(_T("Area"),MFT_GRADIENT|MFT_TOP_TITLE);
						//// TRACE("Pipe menu at %d,%d\n", pt.x, pt.y);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}	
				}
				else if(SELTYPE_TERMINALPORTAL == seltype)	//selection is a processor
				{
						menu.LoadMenu(IDR_CTX_AREAPORTAL);
					pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
					if(pCtxMenu != NULL)
					{
						pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
						pCtxMenu->SetMenuTitle(_T("Portal"),MFT_GRADIENT|MFT_TOP_TITLE);
						//// TRACE("Pipe menu at %d,%d\n", pt.x, pt.y);
						pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
					}	
				}
				else if(SELTYPE_LANDSIDETRACER == seltype)	//selection is a processor
				{
					// idx is the index of the vehicle desc in the logs
				}
				else if( SELTYPE_AIRROUTE == seltype )
				{
					if( SELSUBTYPE_MAIN ==selsubtype )
					{
						m_nSelectedAirRout = idx;Invalidate(FALSE);
						menu.LoadMenu(IDR_CTX_AIRROUTE);
						pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
						if(pCtxMenu != NULL)
						{
							pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
							pCtxMenu->SetMenuTitle(_T("AirRoute"),MFT_GRADIENT|MFT_TOP_TITLE);								
							pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, pt.x, pt.y, AfxGetMainWnd() );
						}
					}
					
				}
			}
			else
			{
				Invalidate(FALSE);
			}

			//new design		
			//
			if(GetDocument()->m_eAnimState == CTermPlanDoc::anim_none && GetDocument()->m_systemMode == EnvMode_AirSide){
				SelectableList selected;
				if (!(nFlags&MK_CONTROL))
				{
					if( SelectAirsideScene(nFlags,point.x,point.y,CSize(2,2),GetDocument()->m_SelectSettings, selected) > 0 ){			
						if( selected.size() > 0) {					
							Selectable * pSel = selected.begin()->get();
							if(pSel){
								if (std::find(GetParentFrame()->m_vSelected.begin(),GetParentFrame()->m_vSelected.end(),pSel) == GetParentFrame()->m_vSelected.end())
								{
									GetParentFrame()->UnSelectAll();
									GetParentFrame()->Select(pSel);
								}
								
								SelectableMenuPop(pSel,pt);
							}
						}
					}
				}
			}
			break;
		};         
	}
}

void C3DView::GenerateDetachSubMenu(CNewMenu*pCtxMenu)
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	if (pDoc == NULL)
		return;

	CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(m_nSelectedPipe >=0 && m_nSelectedPipe < nPipeCount);
	CPipe* pPipe = pPipeDS->GetPipeAt(m_nSelectedPipe);
	ASSERT(pPipe);
	if (pPipe == NULL)
		return;

	int nCount = pCtxMenu->GetMenuItemCount();
	CNewMenu* pSubMenu = (CNewMenu*)pCtxMenu->GetSubMenu(10);

	if (pSubMenu)
	{
		pSubMenu->RemoveMenu(0,MF_BYPOSITION);
		for (int i = 0; i < pPipe->GetAttachCrosswalkCount(); i++)
		{
			CrossWalkAttachPipe& crossPipe = pPipe->GetAtachCrosswalk(i);
			LandsideCrosswalk* pCrosswalk = crossPipe.GetAttachWalk();
			if (pCrosswalk)
			{
				CString strCrosswalk = pCrosswalk->getName().GetIDString();
				pSubMenu->AppendMenu(MF_POPUP,ID_DETACH_CROSSWALK_START + i,strCrosswalk);
			}
		}
	}
}

void C3DView::OnCtxDirectionBidirection()
{
	if( GetDocument()->GetSelectProcessors().GetCount() > 0 )
	{
		CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
		if (pObjectDisplay &&pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 * pProc2 =	(CProcessor2 *)pObjectDisplay ;
			if( pProc2->GetProcessor()->getProcessorType() == TaxiwayProcessor )
			{
				TaxiwayProc * ptaxi = (TaxiwayProc * ) pProc2->GetProcessor();
				if((int)ptaxi->GetSegmentList().size() > m_nSelectTaxiSegment)
					ptaxi->GetSegmentList()[m_nSelectTaxiSegment].emType = TaxiwayProc::Bidirection;
			}
		}

	} 

	//AfxMessageBox("TODO:1");
}

void C3DView::OnCtxDirectionPositive()
{
	if( GetDocument()->GetSelectProcessors().GetCount() > 0 )
	{
		CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
		if (pObjectDisplay &&pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 * pProc2 =	(CProcessor2 *)pObjectDisplay ;

			if( pProc2->GetProcessor()->getProcessorType() == TaxiwayProcessor )
			{
				TaxiwayProc * ptaxi = (TaxiwayProc * ) pProc2->GetProcessor();
				if((int)ptaxi->GetSegmentList().size() > m_nSelectTaxiSegment)
					ptaxi->GetSegmentList()[m_nSelectTaxiSegment].emType = TaxiwayProc::PositiveDirection;
			}
		}
	} 
	//AfxMessageBox("TODO:2");
}

void C3DView::OnCtxDirectionNegative()
{
	if( GetDocument()->GetSelectProcessors().GetCount() > 0 )
	{
		CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
		if (pObjectDisplay &&pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 * pProc2 =	(CProcessor2 *)pObjectDisplay ;

			if( pProc2->GetProcessor()->getProcessorType() == TaxiwayProcessor )
			{
				TaxiwayProc * ptaxi = (TaxiwayProc * ) pProc2->GetProcessor();
				if((int)ptaxi->GetSegmentList().size() > m_nSelectTaxiSegment)
					ptaxi->GetSegmentList()[m_nSelectTaxiSegment].emType = TaxiwayProc::NegativeDirection;
			}
		}
	} 
	//AfxMessageBox("TODO:3");
}

void C3DView::OnUpdateTaxiwayDir(CCmdUI* pCmdUI)
{
	if( GetDocument()->GetSelectProcessors().GetCount() > 0 )
	{
		CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
		if (pObjectDisplay &&pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 * pProc2 =	(CProcessor2 *)pObjectDisplay ;

			if( pProc2->GetProcessor()->getProcessorType() == TaxiwayProcessor )
			{
				TaxiwayProc * ptaxi = (TaxiwayProc * ) pProc2->GetProcessor();
				if((int)ptaxi->GetSegmentList().size() > m_nSelectTaxiSegment)
				{
					TaxiwayProc::DirectionType dir = ptaxi->GetSegmentList()[m_nSelectTaxiSegment].emType;
					
					switch(pCmdUI->m_nID)
					{
					case ID_DIRECTION_BIDIRECTION:
						pCmdUI->SetCheck(dir == TaxiwayProc::Bidirection);
						break;

					case ID_DIRECTION_POSITIVE:
						pCmdUI->SetCheck(dir == TaxiwayProc::PositiveDirection);
						break;

					case ID_DIRECTION_NEGATIVE:
						pCmdUI->SetCheck(dir == TaxiwayProc::NegativeDirection);
						break;
					}
				}
			}
		}
	} 


}

void C3DView::OnDestroy() 
{
	ANIMTIMEMGR->Enroll3DView(NULL);
	GetDocument()->StopAnimation();

#if _USECHLOE_ONBOARD
	//chloe
	CHLOE_SYSTEM(ARCRSystem)->destory_scene(m_OnBoradScene);
	//
#endif

	CView::OnDestroy();
	wglMakeCurrent(NULL, NULL);		// make RC non-current
	delete m_pDC;
	m_pDC = NULL;
	
	wglDeleteContext(m_hRC);		// delete RC

	
#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
	CCodeTimeTest::TraceOutResultToFile();
#endif

}

void C3DView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if(cy <= 0 || cx <=0)
		return;
	m_dAspect = (double) cx / (double) cy;
	m_nOnSizeTimeout = SetTimer(m_nOnSizeTimeout, ONSIZETIMEOUT, NULL);
	UseSharpTexture(FALSE);
	SetupViewport(cx, cy);
}

BOOL C3DView::OnEraseBkgnd(CDC* pDC) 
{
	// tell Windows not to erase the background	
	return TRUE;
}

BOOL C3DView::SetupPixelFormat(HDC hDC, DWORD dwFlags)
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		0,			
		PFD_TYPE_RGBA,                  // RGBA type
		32,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		24,                             // 32-bit z-buffer
		1,                              // one stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};

	pfd.dwFlags = dwFlags;

	if( !(m_iPixelFormat = ::ChoosePixelFormat(hDC, &pfd)) )
	{
		MessageBox("ChoosePixelFormat failed");
		return FALSE;
	}

	if(!SetPixelFormat(hDC, m_iPixelFormat, &pfd))
	{
		MessageBox("SetPixelFormat failed");
		return FALSE;
	}

	// TRACE("Pixel Format %d was selected\n", m_iPixelFormat);

	return TRUE;
}

BOOL C3DView::SetupViewport(int cx, int cy)
{
	if(!::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return FALSE;
	glViewport(0, 0, cx, cy);
	::wglMakeCurrent(NULL, NULL);
	return TRUE;
}

// Apply the perspective view, according the object of Camera
BOOL C3DView::ApplyPerspective()
{
	GetCamera()->ApplyGLProjection(m_dAspect);
	return TRUE;
}

// Apply the direction where you look
BOOL C3DView::ApplyViewXForm()
{
	GetCamera()->ApplyGLView();
	return TRUE;
}

BOOL C3DView::PreRenderScene()
{
	ASSERT(FALSE);
	return TRUE;
}

BOOL C3DView::PostRenderScene()
{
	ASSERT(FALSE);
	return TRUE;
}







//ben 2005-9-8
 void C3DView::RenderContours(CTermPlanDoc * pDoc,double * dAlt,BOOL * bOn){
	
	// if(m_bHillDirtflag){
		//glDeleteLists(m_hillDList,1);	
	if( m_bCoutourEditMode  )
		 return;
	CContourTree *_tree=pDoc->GetTerminal().m_AirsideInput->pContourTree;
	CContourNode * node=_tree->GetRootNode();
	node=node->m_pFirstChild;
	//m_hillDList=glGenLists(1);
	//glNewList(m_hillDList,GL_COMPILE);
	GLfloat _alpha=1.0f;
	if(getColorMode()==_vivid){
		_alpha=1.0f;
	}else if(getColorMode()==_de_saturated){
		_alpha=0.5f;
	}
	glColor4f(1.0f, 1.0f, 1.0f, _alpha);	
	while(node!=NULL){
		HillProc *hill=new HillProc(node);
		hill->setDoc(pDoc);
		hill->setTextureid(GetParentFrame()->AllTexIDs()[Textures::TextureEnum::Hill_Grass]);				
		hill->DrawProc();
		delete hill;
		node=node->m_pNext;
	}

}


CCmdUI *g_ptCmdUI = NULL;

// RenderScene(BOOL bRenderFloors=TRUE)
BOOL C3DView::RenderScene(BOOL bRenderFloors)
{
	START_CODE_TIME_TEST
	CTermPlanDoc* pDoc = GetDocument();
	COLORREF col;

	int i;
	// modify by bird 2003/10/9, to can add any floor( not any 4 layer )
	//floor altitudes
	double dAlt[MAX_FLOOR+1];
	double dAltActive;
	//floor visible
	BOOL bOn[MAX_FLOOR+1];
	//
	const Path* path = NULL;
	//

	CHECK_GL_ERRORS("3DView::RenderScene(...), top");

	int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	int nCurFloorCount;

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);			// store the current viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);	// store the m-v matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); //store the projection matrix


	if(GetCamera()->GetProjectionType() == C3DCamera::perspective)
	{
		//draw floors & get altitudes & vis
		glPushMatrix();
		//if (pDoc->m_systemMode == EnvMode_AirSide)
		//{
		//	for(i=0; i<nFloorCount; i++)
		//	{
		//		// the Altitude of every floor
		//		dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->RealAltitude()*pDoc->m_iScale;
		//		//dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->Altitude();
		//		// if the floors are visible
		//		bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
		//	}
		//	dAlt[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->RealAltitude()*pDoc->m_iScale;
		//}
		//else
		{
			for(i=0; i<nFloorCount; i++)
			{
				// the Altitude of every floor
				dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->Altitude();
				// if the floors are visible
				bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
			}
		    dAlt[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->Altitude();
		}

		//
		bOn[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->IsVisible();
		nCurFloorCount = pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide ) + 1;

		dAlt[nCurFloorCount] = dAlt[nCurFloorCount-1] + 1000.0;	//set alt for dummy floor 
															//above top floor (used when
															//drawing multi-floor procs
		
		if(bRenderFloors) {
			glDisable(GL_LIGHTING);
			std::vector<CFloor2*> vSortedFloors;
			CFloorList teminalFloors =  GetFloorsByMode(EnvMode_Terminal);
			CAirsideGround* pAirsideFloor = GetAirside3D()->GetActiveLevel();

			vSortedFloors.insert(vSortedFloors.end(), teminalFloors.begin(), teminalFloors.end());
			vSortedFloors.push_back(pAirsideFloor);
			//CFloorList airsideFloors = GetFloorsByMode(EnvMode_AirSide);
			//CFloorList landsideFloors = GetFloorsByMode(EnvMode_LandSide);

			//vSortedFloors.insert(vSortedFloors.end(),airsideFloors.begin(),airsideFloors.end());
			//vSortedFloors.insert(vSortedFloors.end(),landsideFloors.begin(),landsideFloors.end());
			
			std::sort(vSortedFloors.begin(), vSortedFloors.end(), CBaseFloor::CompareFloors);

			CHECK_GL_ERRORS("3DView::RenderScene(...), pre draw floors");
			
			//draw floors
			/*if (pDoc->m_systemMode == EnvMode_AirSide)
			{	
				for(size_t i=0; i<vSortedFloors.size(); i++) 			
				{
					vSortedFloors[i]->DrawOGL(this,vSortedFloors[i]->RealAltitude()*pDoc->m_iScale);
				}
			}
			else*/
			{
				for(size_t i=0; i<vSortedFloors.size(); i++) 			
				{
					vSortedFloors[i]->DrawOGL(this,vSortedFloors[i]->Altitude());
				}
			}

			//
			
			CHECK_GL_ERRORS("3DView::RenderScene(...), post draw floors");
		}
		
		GLuint nConeDLID = GetParentFrame()->ConeDLID();
		
		for(i=0; i<nFloorCount; i++) 
		{			
			//draw marker
			if(bOn[i] && (pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i])->UseAlignLine())
			{
				CAlignLine alignline = (pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i])->GetAlignLine();
				ARCVector2 vMarkerLoc = alignline.getBeginPoint();
				glEnable(GL_LIGHTING);
				glColor3ubv(markercolor);
				DrawCone(nConeDLID, vMarkerLoc[VX], vMarkerLoc[VY], dAlt[i], 100.0, 200.0);
				DrawCone(nConeDLID, alignline.getEndPoint()[VX], alignline.getEndPoint()[VY], dAlt[i], 100.0,200.0);
			}
			else if(bOn[i] && (pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i])->UseMarker())
			{
				ARCVector2 vMarkerLoc;
				(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i])->GetMarkerLocation(vMarkerLoc);
				glEnable(GL_LIGHTING);
				glColor3ubv(markercolor);
				DrawCone(nConeDLID, vMarkerLoc[VX], vMarkerLoc[VY], dAlt[i], 100.0, 200.0);
			}
		}
		//
		CFloor2* pAirsideGround = GetParentFrame()->GetAirside3D()->GetActiveLevel();
		if( pAirsideGround && pAirsideGround->UseAlignLine())
		{
			CAlignLine alignline = pAirsideGround->GetAlignLine();
			ARCVector2 vMarkerLoc = alignline.getBeginPoint();
			glEnable(GL_LIGHTING);
			glColor3ubv(markercolor);
			DrawCone(nConeDLID, vMarkerLoc[VX], vMarkerLoc[VY], 0, 1000.0, 2000.0);
			DrawCone(nConeDLID, alignline.getEndPoint()[VX], alignline.getEndPoint()[VY], 0, 1000.0,2000.0);
		}
		else if( pAirsideGround && pAirsideGround->UseMarker() )
		{
			ARCVector2 vMarkerLoc;
			pAirsideGround->GetMarkerLocation(vMarkerLoc);
			glEnable(GL_LIGHTING);
			glColor3ubv(markercolor);
			DrawCone(nConeDLID, vMarkerLoc[VX], vMarkerLoc[VY], 0, 1000.0, 2000.0);
		}
		

		//draw marker
		glPopMatrix();
	}
	else
	{ //only draw active floor
		DistanceUnit distActiveFloor = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[pDoc->m_nActiveFloor]->Altitude();
		for(i=0; i<nFloorCount; i++) 
		{
			dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->Altitude() - distActiveFloor;
			bOn[i] = FALSE;
		}
		//modify ben 2005-9-8
		if( EnvMode_AirSide == GetDocument()->m_systemMode){
			 dAlt[nFloorCount]=0.0;
			 bOn[nFloorCount]=TRUE;
		}
			
		else{
			bOn[pDoc->m_nActiveFloor]=TRUE;
		}
			
		
		if(bRenderFloors)
			GetDocument()->GetActiveFloor()->DrawOGL(this,0.0);

		
	}

	if( EnvMode_AirSide == GetDocument()->m_systemMode)
		dAltActive = dAlt[nFloorCount];
	else
		dAltActive = dAlt[pDoc->m_nActiveFloor];

	CHECK_GL_ERRORS("3DView::RenderScene(...), after floors");

	/*if(GetDocument()->m_systemMode == EnvMode_LandSide){
		GetDocument()->GetLandsideView()->Render(this);
		
	}*/
	
  
	glPushMatrix();	

	
	GetParentFrame()->GetAirside3D()->DrawOGL(this,false);
	
	//draw Wall shape
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	RenderWallShapes(pDoc,dAlt,bOn);
	glDisable(GL_CULL_FACE);
	//draw CStructures 2005-9-1
	RenderStructures(pDoc,this,m_nSelectedStructure,m_bSelectStructureEdit,dAlt,bOn);

	//draw resource pool paths
	RenderResourcePaths(pDoc, dAlt);

	//draw railways
	RenderRailways(pDoc, m_nSelectedRailWay, m_bSelRailEdit, dAlt, bOn);

	//draw temp proc info
	RenderTempProcInfo(pDoc, dAltActive);	
	glPopMatrix();

	
	if(pDoc->m_eAnimState == CTermPlanDoc::anim_none) 
	{
		//draw procs in normal mode and tracers if on		
		//draw processors
		
		RenderProcessors(pDoc, this, -1, dAlt, bOn);

		
		CHECK_GL_ERRORS("3DView::RenderScene(...), post render procs");
	}
	else if(pDoc->m_bMobileElementDisplay)
	{
		/*if(m_bMovingCamera){
			glPolygonMode(GL_FRONT_AND_BACK ,GL_LINE);
		}	*/
		// draw pax, trains, processors in animation mode	
		AnimationData* pAnimData = &(pDoc->m_animData);
		long nTime = pAnimData->nLastAnimTime;

		// draw processors
		//DWORD t1 = GetTickCount();
		RenderProcessors(pDoc, this, nTime, dAlt, bOn);
		//DWORD t2 = GetTickCount();
		//m_strTime.Format("%s--%d", m_strTime, t2-t1);

		//modified by hans 2005-12-27 ,
		//according to user's option,draw Terminal animation or airside animation 
		if(pDoc->m_animData.m_AnimationModels.IsTerminalSel() || pDoc->m_animData.m_AnimationModels.IsOnBoardSel())
		{
			CHECK_GL_ERRORS("3DView::RenderScene(...), post render procs");

			// draw mobile elements in terminal
			//t1 = GetTickCount();
			DrawPax(pDoc, bOn, dAlt);
			//t2 = GetTickCount();
			//m_strTime.Format("%s--%d", m_strTime, t2-t1);
		}
		if (pDoc->m_animData.m_AnimationModels.IsAirsideSel() || pDoc->m_animData.m_AnimationModels.IsTerminalSel() )
		{
			CHECK_GL_ERRORS("3DView::RenderScene(...), post render pax");
			
			DistanceUnit dAirportAlt = GetAirside3D()->GetActiveAirport()->m_altAirport.getElevation();
			// draw flights in airside	
			glEnable(GL_POLYGON_OFFSET_FILL);
//			glPolygonOffset(-2.0,-2.0);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			DrawFlight(dAirportAlt,FALSE, !pDoc->m_bHideACTags, mvmatrix,projmatrix,viewport );
			DrawVehicle(dAirportAlt,FALSE, !pDoc->m_bHideACTags, mvmatrix,projmatrix,viewport);
			DrawStairs();
			glDisable(GL_CULL_FACE);
			glDisable(GL_POLYGON_OFFSET_FILL);
			glDisable(GL_LIGHTING);
			/*if(!pDoc->m_bHideACTags){
				glDisable(GL_DEPTH_TEST);
				DrawAirsideTags2(this, pDoc, &dAirportAlt, false, mvmatrix, projmatrix, viewport);
				glEnable(GL_DEPTH_TEST);
			}*/
		}
		
		if (pDoc->m_animData.m_AnimationModels.IsLandsideSel())
		{
			//haven't implement
			glEnable(GL_CULL_FACE);
			DrawCars();
			glDisable(GL_CULL_FACE);

		}		

		/*CHECK_GL_ERRORS("3DView::RenderScene(...), post draw flight");		

		CHECK_GL_ERRORS("3DView::RenderScene(...), post draw aircraft tags");

		
		glPolygonMode(GL_FRONT_AND_BACK ,GL_FILL);*/
	}		
	
	

	//draw areas and portals
	CAreaList* pAList = &(pDoc->GetTerminal().m_pAreas->m_vAreas);
	for(i=0; i<static_cast<int>(pAList->size()); i++) 
	{
		CNamedPointList* pArea = pAList->at(i);
		if(bOn[pArea->floor])
		{
			col = pArea->color;
			glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
			glBegin(GL_LINE_LOOP);
			for(int j=0; j<static_cast<int>(pArea->points.size()); j++)
			{
				glVertex3d(pArea->points[j][VX], pArea->points[j][VY], dAlt[pArea->floor]);
			}
			glEnd();
		}
	}
	CPortalList* vPortalList = &(pDoc->m_portals.m_vPortals);
	for(i=0; i<static_cast<int>(vPortalList->size()); i++) 
	{
		CNamedPointList* pPortal = vPortalList->at(i);
		if(bOn[pPortal->floor])
		{
			col = pPortal->color;
			glColor3ub(GetRValue(col), GetGValue(col), GetBValue(col));
			glBegin(GL_LINE_STRIP);
			for(int j=0; j<static_cast<int>(pPortal->points.size()); j++) 
			{
				glVertex3d(pPortal->points[j][VX], pPortal->points[j][VY], dAlt[pPortal->floor]);
			}
			glEnd();
		}
	}

	//draw line if in "getmanypoints mode" or "gettwopoints mode"

	if(m_eMouseState == NS3DViewCommon::_getonerect && m_vMousePosList.size() > 1) 
	{
		glColor3ubv(linecolor);
		ARCVector3 vDiff = m_vMousePosList[1]-m_vMousePosList[0];
		glBegin(GL_LINE_LOOP);
		glVertex3d(m_vMousePosList[0][VX]-vDiff[VX],m_vMousePosList[0][VY]-vDiff[VY],m_vMousePosList[0][VZ]);
		glVertex3d(m_vMousePosList[0][VX]+vDiff[VX],m_vMousePosList[0][VY]-vDiff[VY],m_vMousePosList[0][VZ]);
		glVertex3d(m_vMousePosList[0][VX]+vDiff[VX],m_vMousePosList[0][VY]+vDiff[VY],m_vMousePosList[0][VZ]);
		glVertex3d(m_vMousePosList[0][VX]-vDiff[VX],m_vMousePosList[0][VY]+vDiff[VY],m_vMousePosList[0][VZ]);
		glEnd();
	}
	else if(m_vMousePosList.size() > 0 &&
		(m_eMouseState == NS3DViewCommon::_getmanypoints
		|| m_eMouseState == NS3DViewCommon::_gettwopoints
		|| m_eMouseState == NS3DViewCommon::_distancemeasure
		|| m_eMouseState == NS3DViewCommon::_getmirrorline
		|| m_eMouseState == NS3DViewCommon::_getarrayline
		|| m_eMouseState == NS3DViewCommon::_getalignline
		|| m_eMouseState == NS3DViewCommon::_selectlandproc
		|| m_eMouseState == NS3DViewCommon::_pickstretchpos )
		) 
	{
		glDisable(GL_DEPTH_TEST);
		glColor3ubv(linecolor);
		glBegin(GL_LINE_STRIP);
		if(GetCamera()->GetProjectionType() == C3DCamera::perspective) 
		{
			for(i=0; i<static_cast<int>(m_vMousePosList.size()); i++) 
			{
				ARCVector3 v = m_vMousePosList[i];
				glVertex3d(v[VX], v[VY], v[VZ]);
			}
			ARCVector3 v = m_ptMousePos;
			glVertex3d(v[VX], v[VY], v[VZ]);
		}
		else 
		{
			for(i=0; i<static_cast<int>(m_vMousePosList.size()); i++) 
			{
				ARCVector3 v = m_vMousePosList[i];
				glVertex3d(v[VX], v[VY], 0.0);
			}
			ARCVector3 v = m_ptMousePos;
			glVertex3d(v[VX], v[VY], 0.0);
		}
		glEnd();
		glEnable(GL_DEPTH_TEST);
	}



	

	//draw pipes
	glDepthMask(GL_FALSE);
	RenderPipes(pDoc, GetParentFrame()->GetTextureResoure(), m_nSelectedPipe, m_bSelPipeEdit, dAlt, bOn);	
	//draw flows
	if(glIsList(GetParentFrame()->FlowDLID())) 
		glCallList(GetParentFrame()->FlowDLID());
	glDepthMask(GL_TRUE);

	CHECK_GL_ERRORS("3DView::RenderScene(...), end");

	
#if	_USECHLOE_ONBOARD
	//chloe
	if(GetDocument()->m_systemMode == EnvMode_OnBoard )
	{
		ARCVector3 eye_pos;
		ARCVector3 lookat;
		GetCamera()->GetLocation(eye_pos);
		GetCamera()->GetLookAtVector(lookat);
		double eye_pos_[3];
		double lookat_[3];
		for(int i=0; i<3; ++i)
		{
			eye_pos_[i] = eye_pos[i];
			lookat_[i] = lookat[i];
		}
		SAFE_CALL(m_OnBoradScene)->render(eye_pos_, lookat_);
	}
#endif

	//
	return TRUE;
}

void C3DView::RenderText()
{
	static char buf[256];

	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);			// store the current viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);	// store the m-v matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); //store the projection matrix
	
	//int i = 0;
	CTermPlanDoc* pDoc = GetDocument();
	double dWinPos[4];

	//floor altitudes
	double dAlt[MAX_FLOOR];
	//floor visible
	BOOL bOn[MAX_FLOOR];
	if(GetCamera()->GetProjectionType() == C3DCamera::perspective) 
	{
		//if (pDoc->m_systemMode == EnvMode_AirSide)
		//{
		//	for(i=0; i<static_cast<int>(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()); i++) 
		//	{
		//		dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->RealAltitude()*pDoc->m_iScale;
		//		bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
		//	}
		//	//
		//	dAlt[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->RealAltitude()*pDoc->m_iScale;
		//	bOn[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->IsVisible();
		//}
		//else
		{
			for(int i=0; i<static_cast<int>(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()); i++) 
			{
				dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->Altitude();
				bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
			}
			//
			dAlt[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->Altitude();
			bOn[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->IsVisible();
		}
	}
	else 
	{
		for(int i=0; i<static_cast<int>(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()); i++)
		{
			dAlt[i] = 0.0;
			bOn[i] = FALSE;
		}
		//
		dAlt[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = 0.0;
		bOn[pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )] = FALSE;

		bOn[pDoc->m_nActiveFloor] = TRUE;
	}


	if(!pDoc->m_displayoverrides.m_pbProcDisplayOverrides[PDP_DISP_PROCNAME]) {  //if NOT "hide all processor names"
		GetParentFrame()->GetAirside3D()->RenderALTObjectText(this);
		GetFloorProperty(pDoc->GetFloorByMode(EnvMode_Terminal),dAlt,bOn,pDoc);
		RenderProc2Text(pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vDefined, dAlt, bOn);
		RenderProc2Text(pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vUndefined,dAlt,bOn);
		RenderPipeText(dAlt,bOn);	
	}
	if(pDoc->m_bShowAirsideNodeNames)
	{
		GetParentFrame()->GetAirside3D()->GetActiveAirport()->RenderNodeNames(this);
	}
	if(m_eMouseState == NS3DViewCommon::_pickHoldLine || pDoc->m_bShowAirsideNodeNames)//render hold line name
	{
		GetParentFrame()->GetAirside3D()->GetActiveAirport()->RenderHoldLineNames(this);
	}

	std::vector<ARCVector3> vProcPosSEL; //selected defined and undefined procs
	BOOL bShowProcTags = FALSE;
	
	if(pDoc->m_eAnimState == CTermPlanDoc::anim_none) 
	{ //get proc selection box positions

		for(int i=0; i<static_cast<int>(pDoc->GetSelectProcessors().GetCount()); i++) 
		{
			//project	pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide )

			CObjectDisplay *pObjectDisplay = pDoc->GetSelectedObjectDisplay(i);

			if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
			{
			
			CProcessor2 *pProc2 =(CProcessor2 *)pObjectDisplay;

			ARCVector3 worldposXY;
			double worldposZ;
			worldposXY=pProc2->GetLocation();
			//worldposZ = dAlt[pDoc->GetSelectProcessors()[i]->GetFloor()];
			int nProcFloor;
			if( EnvMode_Terminal == GetDocument()->m_systemMode)
			{
				nProcFloor = pProc2->GetFloor();
			}
			else if( EnvMode_AirSide == GetDocument()->m_systemMode)
			{ // airside.
				nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_AirSide );
			}
			else
			{ // landside.
				nProcFloor = pDoc->GetDrawFloorsNumberByMode( EnvMode_LandSide );				
			}

			worldposZ = dAlt[nProcFloor];
			gluProject(worldposXY[VX], worldposXY[VY], worldposZ,
				mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));
			//store loc
			vProcPosSEL.push_back(ARCVector3(dWinPos[0], dWinPos[1], dWinPos[2]));
			}
			else
			{
				CObjectDisplay* pObjDisplay = pObjectDisplay;
				ARCVector3 worldposXY;
				double worldposZ;
				worldposXY=pObjDisplay->GetLocation();
				int nFloor = pObjDisplay->GetFloorIndex();
				worldposZ = dAlt[nFloor];
				gluProject(worldposXY[VX], worldposXY[VY], worldposZ,
					mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));
				//store loc
				vProcPosSEL.push_back(ARCVector3(dWinPos[0], dWinPos[1], dWinPos[2]));
			}
			
		}
		////for select other pipes
		//for(int i=0; i< (int)pDoc->m_vSelectPlacements.GetCount();++i)
		//{
		//	CObjectDisplay* pObjDisplay = pDoc->m_vSelectPlacements.GetItem(i);
		//	ARCVector3 worldposXY;
		//	double worldposZ;
		//	worldposXY=pObjDisplay->GetLocation();
		//	int nFloor = pObjDisplay->GetFloorIndex();
		//	worldposZ = dAlt[nFloor];
		//	gluProject(worldposXY[VX], worldposXY[VY], worldposZ,
		//		mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));
		//	//store loc
		//	vProcPosSEL.push_back(ARCVector3(dWinPos[0], dWinPos[1], dWinPos[2]));
		//}

		//get position of processor being scaled etc.. if any
		if(m_eMouseState == NS3DViewCommon::_scaleproc || m_eMouseState == NS3DViewCommon::_scaleprocX ||
		   m_eMouseState == NS3DViewCommon::_scaleprocY || m_eMouseState == NS3DViewCommon::_scaleprocZ||
		    m_eMouseState==NS3DViewCommon::_moveshapesZ) 
		{
			
			bShowProcTags = TRUE;
		}
	}
	else if(pDoc->m_bMobileElementDisplay) {
		DrawPaxTags(pDoc, this, bOn, dAlt);
		//RenderProc2Text(pDoc->GetCurrentPlacement(EnvMode_Terminal)->m_vDefined, dAlt, bOn);
		//DrawAirsideTags();
	}

	if(pDoc->m_eAnimState == CTermPlanDoc::anim_none) {

		//set proj and m-v matrices
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, viewport[2], 0.0, viewport[3],-1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		
		int size = vProcPosSEL.size();
		if(size > 0) {
			DWORD dwTick = GetTickCount();
			int i=0;
			if((dwTick % 1000)>500) {
				glColor3ubv(processorselectboxcolor1);
				glPushMatrix();
				glTranslatef(static_cast<GLfloat>(vProcPosSEL[size-1][VX]), static_cast<GLfloat>(vProcPosSEL[size-1][VY]),static_cast<GLfloat>(vProcPosSEL[size-1][VZ])/*0.0f*/);
				glCallList(GetParentFrame()->SelectionDLID());
				glPopMatrix();
				glColor3ubv(processorselectboxcolor2);
				for(i=0; i<size-1; i++) {
					glPushMatrix();
					glTranslatef(static_cast<GLfloat>(vProcPosSEL[i][VX]), static_cast<GLfloat>(vProcPosSEL[i][VY]),static_cast<GLfloat>(vProcPosSEL[i][VZ])/*0.0f*/);
					glCallList(GetParentFrame()->SelectionDLID());
					glPopMatrix();
				}
			}
			
			//now draw tags if bShowProcTags == TRUE
			if(bShowProcTags) {
				glDisable(GL_DEPTH_TEST);
				CString sTag;
				ARCVector3 vScale;
				ARCVector3 vSize;
				int nSign = 1;
				if(vProcPosSEL[i][VX] > (viewport[2]/2.0))
					nSign = -1;

				CUnitsManager::LENGTH_UNITS lu = UNITSMANAGER->GetLengthUnits();
				for(i=0; i<static_cast<int>(pDoc->GetSelectProcessors().GetCount()); i++) 
				{
					int x,y,w,h;
					x = static_cast<int>(vProcPosSEL[i][VX]+nSign*(25-(TEXTMANAGER3D->GetCharWidth())));
					y = static_cast<int>(vProcPosSEL[i][VY]+(25-(TEXTMANAGER3D->GetCharHeight()*0.7)));
					//
					int nStrMaxLength=0;
					int nStrMaxLine=4;
					switch(m_eMouseState)
					{
					case NS3DViewCommon::_scaleproc:
					case NS3DViewCommon::_scaleprocX:
					case NS3DViewCommon::_scaleprocY:
					case NS3DViewCommon::_scaleprocZ:
					case NS3DViewCommon::_moveshapesZ:
						{
							CObjectDisplay *pObjectDisplay = pDoc->GetSelectedObjectDisplay(i);
							if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
							{
								continue;
							}
							CProcessor2 * pProcessor2 =(CProcessor2 *)pObjectDisplay;

							pProcessor2->GetScale(vScale);
							vSize = pProcessor2->GetShape()->ExtentsMax() - pProcessor2->GetShape()->ExtentsMin();
							CString str1,str2,str3,str4;
							if(pDoc->m_systemMode == EnvMode_AirSide)
								str1.Format("\nSize X: %.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vScale[VX]*vSize[VX]),CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
							else
								str1.Format("\nSize X: %.2f %s",UNITSMANAGER->ConvertLength(vScale[VX]*vSize[VX]), UNITSMANAGER->GetLengthUnitString(lu));
							if(str1.GetLength()>nStrMaxLength)
								nStrMaxLength=str1.GetLength();

							if(pDoc->m_systemMode == EnvMode_AirSide)
								str2.Format("\nSize Y: %.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vScale[VY]*vSize[VY]), CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
							else
								str2.Format("\nSize Y: %.2f %s",UNITSMANAGER->ConvertLength(vScale[VY]*vSize[VY]), UNITSMANAGER->GetLengthUnitString(lu));
							if(str2.GetLength()>nStrMaxLength)
								nStrMaxLength=str2.GetLength();

							if(pDoc->m_systemMode == EnvMode_AirSide)
								str3.Format("\nSize Z: %.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vScale[VZ]*vSize[VZ]), CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
							else
								str3.Format("\nSize Z: %.2f %s",UNITSMANAGER->ConvertLength(vScale[VZ]*vSize[VZ]), UNITSMANAGER->GetLengthUnitString(lu));
							if(str3.GetLength()>nStrMaxLength)
								nStrMaxLength=str3.GetLength();
							
							ARCVector3 vLocation=pProcessor2->GetLocation();

							if(pDoc->m_systemMode == EnvMode_AirSide)
								str4.Format("\nHeight: %.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),vLocation[VZ]),CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
							else
								str4.Format("\nHeight: %.2f %s",UNITSMANAGER->ConvertLength(vLocation[VZ]), UNITSMANAGER->GetLengthUnitString(lu));
							if(str4.GetLength()>nStrMaxLength)
								nStrMaxLength=str4.GetLength();
							sTag=str1+str2+str3+str4;
						}
						break;
					case NS3DViewCommon::_rotateshapes:
					case NS3DViewCommon::_rotateproc:
					case NS3DViewCommon::_rotateairsideobject:
					case NS3DViewCommon::_moveprocs:
					case NS3DViewCommon::_moveshapes:
						break;
					}
					
					w = nSign*TEXTMANAGER3D->GetCharWidth()*(nStrMaxLength+2);
					h = TEXTMANAGER3D->GetCharHeight()*(nStrMaxLine+1);
					glColor3ubv(processortagbkgcolor);
					glBegin(GL_QUADS);
					glVertex3f(static_cast<GLfloat>(x),static_cast<GLfloat>(y),0.0);
					glVertex3f(static_cast<GLfloat>(x+w),static_cast<GLfloat>(y),0.0);
					glVertex3f(static_cast<GLfloat>(x+w),static_cast<GLfloat>(y+h),0.0);
					glVertex3f(static_cast<GLfloat>(x),static_cast<GLfloat>(y+h),0.0);
					glEnd();
					glColor3ubv(processortagcolor);
					glBegin(GL_LINE_LOOP);
					glVertex3f(static_cast<GLfloat>(x),static_cast<GLfloat>(y),0.0);
					glVertex3f(static_cast<GLfloat>(x+w),static_cast<GLfloat>(y),0.0);
					glVertex3f(static_cast<GLfloat>(x+w),static_cast<GLfloat>(y+h),0.0);
					glVertex3f(static_cast<GLfloat>(x),static_cast<GLfloat>(y+h),0.0);
					glEnd();
					
					ARCPoint2 vPos;
					if(nSign > 0)
						vPos[VX] = vProcPosSEL[i][VX]+25;
					else
						vPos[VX] = x+w+TEXTMANAGER3D->GetCharWidth();
					vPos[VY] = vProcPosSEL[i][VY]+25;
					glBegin(GL_LINES);
					glVertex3f(static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0.0);
					glVertex3f(static_cast<GLfloat>(vProcPosSEL[i][VX]), static_cast<GLfloat>(vProcPosSEL[i][VY]), 0.0);
					glEnd();
					TEXTMANAGER3D->DrawFormattedBitmapText2D(sTag,vPos);
				}
				glEnable(GL_DEPTH_TEST);
			}
		}

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		ApplyPerspective();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		ApplyViewXForm();
	}
}

void C3DView::RenderPipeText(double* pdAlt, BOOL* pbOn)
{
	for (int i = 0; i < GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeCount(); i++)
	{
		CPipe* pPipe = GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeAt(i);

		int nActiveFloor = GetDocument()->GetCurModeFloor().GetActiveFloorLevel();
		int nPipeFloor = pPipe->GetFloorIndex();
		if(nPipeFloor >= 0)
		{
			if( pbOn[nPipeFloor] &&
				(GetCamera()->GetProjectionType() == C3DCamera::perspective || nActiveFloor == nPipeFloor) )
			{
				ARCVector3 vWorldPos = pPipe->GetLocation();
				vWorldPos[VZ] += pdAlt[pPipe->GetFloorIndex()];
				TEXTMANAGER3D->DrawBitmapText(pPipe->GetPipeName(),vWorldPos);
			}
		}
	}
}
void C3DView::RenderProc2Text(CPROCESSOR2LIST& proc2List, double* pdAlt, BOOL* pbOn)
{
	static char buf[256];

	for (size_t i = 0; i < proc2List.size(); i++)//for each defined processor
	{ 
		CProcessor2* pProc2 = proc2List[i];

		if (pProc2->m_dispProperties.bDisplay[PDP_DISP_PROCNAME]) { //if display processor name flag on
			
			if (pProc2->GetProcessor() == NULL)
			{
				ARCVector3 vWorldPos;
				int nActiveFloor = GetDocument()->GetCurModeFloor().GetActiveFloorLevel();	

				if( pbOn[pProc2->GetFloor()] &&
					(GetCamera()->GetProjectionType() == C3DCamera::perspective || nActiveFloor == pProc2->GetFloor()) )
				{
					vWorldPos = pProc2->GetLocation();
					vWorldPos[VZ] += pdAlt[pProc2->GetFloor()];
					pProc2->ShapeName().printID(buf);
					glColor3ubv(pProc2->m_dispProperties.color[PDP_DISP_PROCNAME]);

					TEXTMANAGER3D->DrawBitmapText(buf,vWorldPos);
				}
			}
			else
			{
				const ProcessorID* id = pProc2->GetProcessor()->getID();
				id->printID(buf);
				glColor3ubv(pProc2->m_dispProperties.color[PDP_DISP_PROCNAME]);

				ARCVector3 vWorldPos;
				int nActiveFloor = GetDocument()->GetCurModeFloor().GetActiveFloorLevel();			

				if( pbOn[pProc2->GetFloor()] &&
					(GetCamera()->GetProjectionType() == C3DCamera::perspective || nActiveFloor == pProc2->GetFloor()) )
				{
					vWorldPos = pProc2->GetLocation();
					vWorldPos[VZ] += pdAlt[pProc2->GetFloor()];

					TEXTMANAGER3D->DrawBitmapText(buf,vWorldPos);
					Processor* proc=  pProc2->GetProcessor();

					if(proc->getProcessorType()==IntegratedStationProc) {
						IntegratedStation* pIS=(IntegratedStation*)proc;
						Point* ptPort=pIS->GetPort1Path()->getPointList();
						vWorldPos[VX] = ptPort[0].getX();
						vWorldPos[VY] = ptPort[0].getY();
						TEXTMANAGER3D->DrawBitmapText("Port1",vWorldPos);
						ptPort=pIS->GetPort2Path()->getPointList();
						vWorldPos[VX] = ptPort[0].getX();
						vWorldPos[VY] = ptPort[0].getY();
						TEXTMANAGER3D->DrawBitmapText("Port2",vWorldPos);
					}

				}
				else if(pProc2->GetProcessor()->getProcessorType() == StairProc) {
					//find point on this floor
					Stair* pStair = (Stair*)( pProc2->GetProcessor() );
					Path* stairPath = pStair->serviceLocationPath();
					int nPtCount = stairPath->getCount();
					ASSERT(nPtCount>1);
					Point* pts = stairPath->getPointList();

					for(int j=0; j<nPtCount; j++) {
						if(GetDocument()->m_nActiveFloor*SCALE_FACTOR == pts[j].getZ()) {
							vWorldPos[VX] = pts[j].getX(); vWorldPos[VY] = pts[j].getY();
							vWorldPos[VZ] = pdAlt[static_cast<int>(pts[j].getZ()/SCALE_FACTOR)];
							TEXTMANAGER3D->DrawBitmapText(buf,vWorldPos);
							break;
						}
					}
				}
				else if(pProc2->GetProcessor()->getProcessorType() == EscalatorProc) {
					//find point on this floor
					Escalator* pEsc = (Escalator*)( pProc2->GetProcessor() );
					Path* sEscPath = pEsc->serviceLocationPath();
					int nPtCount = sEscPath->getCount();
					ASSERT(nPtCount>1);
					Point* pts = sEscPath->getPointList();

					for(int j=0; j<nPtCount; j++) {
						if(GetDocument()->m_nActiveFloor*SCALE_FACTOR == pts[j].getZ()) {
							vWorldPos[VX] = pts[j].getX(); vWorldPos[VY] = pts[j].getY();
							vWorldPos[VZ] = pdAlt[static_cast<int>(pts[j].getZ()/SCALE_FACTOR)];
							TEXTMANAGER3D->DrawBitmapText(buf,vWorldPos);
							break;
						}
					}
				}
			}
		}
	}
}

BOOL C3DView::RenderCompass()
{
		
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);	// store the current viewport
	glViewport(0,0,60,60);					// make a small viewport on the side
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GetCamera()->ApplyCompassGLProjection();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GetCamera()->ApplyCompassGLView();

	if(glIsList(GetParentFrame()->CompassDLID())) {
		glCallList(GetParentFrame()->CompassDLID());
	}
	
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GetCamera()->ApplyGLProjection(m_dAspect);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GetCamera()->ApplyGLView();

	
	
	return TRUE;
}

int C3DView::SelectACTags(UINT nFlags, int x, int y, CSize sizeSelection)
{
	if(GetDocument()->m_bHideACTags)
		return 0 ;
	
	static GLuint selectBuf[SELECTION_BUFFER_SIZE];
	GLint hits;
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return -1;
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);			// store the current viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);	// store the m-v matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); //store the projection matrix
	glSelectBuffer(SELECTION_MAX_HITS, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), sizeSelection.cx,sizeSelection.cy, viewport);
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();

	CTermPlanDoc* pDoc=GetDocument();
	//get floor altitudes
	double dAlt[MAX_FLOOR];
	BOOL bOn[MAX_FLOOR];
	int i;
	if(GetCamera()->GetProjectionType() == C3DCamera::perspective) {
		for(i=0; i<static_cast<int>(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()); i++)
		{
			dAlt[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->Altitude();
			bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
		}
		//
		dAlt[i] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->Altitude();
		bOn[i] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->IsVisible();

	}
	else
	{
		int i;
		for( i=0; i<static_cast<int>(pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size()); i++) {
			dAlt[i] = 0.0;
			bOn[i] = FALSE;
		}
		//
		dAlt[i] = 0.0;
		bOn[i] = FALSE;

		bOn[pDoc->m_nActiveFloor]=TRUE;
	}

	
	//render flight tags
	//DrawAircraftTags(this, pDoc, bOn, dAlt, true, mvmatrix, projmatrix, viewport);

	//DrawAirsideTags2(this, pDoc, dAlt, true, mvmatrix, projmatrix, viewport);


	glFlush();
	hits = glRenderMode(GL_RENDER);

	//process hits
	pDoc->m_vSelectedACTags.clear();
	pDoc->m_vSelectedAirsideTags.clear();

	if(hits > 0) {
		if( selectBuf[3] < AIRSIDEACTAGSOFFSET )
			pDoc->m_vSelectedACTags.push_back(selectBuf[3]);
		else
			pDoc->m_vSelectedAirsideTags.push_back(selectBuf[3]-AIRSIDEACTAGSOFFSET);
	}

	::wglMakeCurrent(NULL, NULL);
	return hits;
}

int C3DView::SelectScene(UINT nFlags, int x, int y, GLuint* pSelProc,CSize sizeSelection,BOOL bDragSelect,int procType, BOOL bIncludUCProcs)
{
	static GLuint selectBuf[SELECTION_BUFFER_SIZE];
	GLint hits;
	GLint viewport[4];
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return -1;
	GLuint nSphereDLID = GetParentFrame()->SphereDLID();
	CHECK_GL_ERRORS("3DView::SelectScene(...), post wglMakeCurrent");
	m_nSelectedAirRout = -1;
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(SELECTION_BUFFER_SIZE, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), sizeSelection.cx,sizeSelection.cy, viewport);
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();

	CHECK_GL_ERRORS("3DView::SelectScene(...), post apply perspective, xform");

	int i;
	//get floor altitudes
	int nFloorCount;
	double dAlt[MAX_FLOOR];
	BOOL bOn[MAX_FLOOR];
	memset(dAlt,0,sizeof(dAlt));
	memset(bOn,0,sizeof(bOn));

	nFloorCount = static_cast<int>(GetDocument()->GetCurModeFloor().m_vFloors.size());
	if(GetCamera()->GetProjectionType() == C3DCamera::perspective)
	{
		
		for(i=0; i < nFloorCount; i++) 
		{
			//// Maybe this is the problem~~~~~~~~~~~~~~~~~~~~~~~~~!!!!!!!
			//if (GetDocument()->m_systemMode == EnvMode_AirSide)
			//{
			//	dAlt[i] = GetDocument()->GetCurModeFloor().m_vFloors[i]->RealAltitude()*GetDocument()->m_iScale;			
			//}
			//else
			{
				dAlt[i] = GetDocument()->GetCurModeFloor().m_vFloors[i]->Altitude();
			}

			bOn[i] = GetDocument()->GetCurModeFloor().m_vFloors[i]->IsVisible()
				&& GetDocument()->GetCurModeFloor().m_vFloors[i]->IsActive() ;
		}
		if(nFloorCount)
		{
			dAlt[nFloorCount] = 1000 + dAlt[nFloorCount-1];
			bOn[i] = FALSE;
		}
	}
	else 
	{
		int i;
		for( i=0; i<nFloorCount; i++) 
		{
			dAlt[i] = 0.0;
			bOn[i] = FALSE;
		}
		//
		dAlt[i] = 0.0;
		bOn[i] = FALSE;
		
		bOn[GetDocument()->m_nActiveFloor]=TRUE;
	}

	CTermPlanDoc* pDoc=GetDocument();
	if(pDoc->m_systemMode == EnvMode_LandSide )
	{
		CPROCESSOR2LIST& vProcList = (GetDocument()->GetCurrentPlacement()->m_vDefined);
		for(size_t i=0;i< vProcList.size();i++)
		{
			vProcList[i]->SetSelectName(i);
		}
	}
	

	if(pDoc->m_eAnimState == CTermPlanDoc::anim_none)
	{
		//render stuff
		if(procType==-1 && bIncludUCProcs )
		{
			CPROCESSOR2LIST& vProcList = (GetDocument()->GetCurrentPlacement()->m_vUndefined);
			
			CPROCESSOR2LIST::reverse_iterator itr; 
			for(itr = vProcList.rbegin(); itr!=vProcList.rend();itr++)
			{
				if(bOn[(*itr)->GetFloor()] || GetDocument()->m_systemMode == EnvMode_LandSide )
				{
					(*itr)->DrawSelectOGL(pDoc, dAlt);
				}
			}
			
		}

		CHECK_GL_ERRORS("3DView::SelectScene(...), post draw UC procs");

		CPROCESSOR2LIST& vProcList = (GetDocument()->GetCurrentPlacement()->m_vDefined);
		CPROCESSOR2LIST::reverse_iterator itr; 
		for(itr = vProcList.rbegin(); itr!=vProcList.rend();itr++)
		{
			if(bOn[(*itr)->GetFloor()] || GetDocument()->m_systemMode == EnvMode_LandSide)
			{
				(*itr)->DrawSelectOGL(pDoc, dAlt);
			}
		
			CHECK_GL_ERRORS("OPENGL ERRORS in 3DView::SelectScene(...), after drawing processor %d [%s]\n");
		}
					
		//render railway lines
		std::vector<RailwayInfo*> railWayVect;
		GetDocument()->m_trainTraffic.GetAllRailWayInfo(railWayVect);
		glLineWidth(3.0);
		if(procType==-1)
		{
			for(i=0; i<static_cast<int>(railWayVect.size()); i++)
			{
				if(railWayVect[i]->IsPathDefined())
				{
					const Path* path = &(railWayVect[i]->GetRailWayPath());
					Point* pts = path->getPointList();
					int c = path->getCount();
					glLoadName(GenerateSelectionID(SELTYPE_RAILWAY, SELSUBTYPE_MAIN, i));
					glBegin(GL_LINE_STRIP);
					for(int j=0; j<c; j++) 
					{
						double dRealAlt,dDiffAlt ;
						int LowerFloor=(int) (pts[j].getZ()/SCALE_FACTOR);
						double mult = (pts[j].getZ()/SCALE_FACTOR)-LowerFloor;
						if( LowerFloor <= nFloorCount )
							dDiffAlt = dAlt[LowerFloor+1]-dAlt[LowerFloor];
						else 
							dDiffAlt = 1000;
						dRealAlt = dAlt[LowerFloor]+ mult*dDiffAlt;
						
						glVertex3d(pts[j].getX(), pts[j].getY(), dRealAlt);
					}
					glEnd();
					if(i==m_nSelectedRailWay && m_bSelRailEdit) 
					{
						for(int j=0; j<c; j++) 
						{
							double dRealAlt,dDiffAlt ;
							int LowerFloor=(int) (pts[j].getZ()/SCALE_FACTOR);
							double mult = (pts[j].getZ()/SCALE_FACTOR)-LowerFloor;
							if( LowerFloor <= nFloorCount )dDiffAlt = dAlt[LowerFloor+1]-dAlt[LowerFloor];
							else dDiffAlt = 1000;
							dRealAlt = dAlt[LowerFloor]+ mult*dDiffAlt;
							glLoadName(GenerateSelectionID(SELTYPE_RAILWAY, SELSUBTYPE_POINT, j));
							DrawCube(pts[j].getX(), pts[j].getY(), dRealAlt, 100.0);
						}
					}
				}
			}
		}
		glLineWidth(1.0);

		CHECK_GL_ERRORS("3DView::SelectScene(...), post render railways");
		// render tracers
		if(procType==-1){
			if(pDoc->m_bShowTracers) {
				int nTrackCount = pDoc->m_tempTracerData.GetTrackCount();
				if(nTrackCount > 0) {
					glEnable(GL_LINE_STIPPLE);
					for(int nTrackIdx=0; nTrackIdx<nTrackCount; nTrackIdx++) {
						std::vector<CTrackerVector3>& track = pDoc->m_tempTracerData.GetTrack(nTrackIdx);
						CPaxDispPropItem* pPDPI = pDoc->m_paxDispProps.GetDispPropItem(pDoc->m_tempTracerData.GetDispPropIdx(nTrackIdx));
						ARCColor3 pdpcol(pPDPI->GetColor());
						int pdplt = (int) pPDPI->GetLineType();
						glColor3ubv(pdpcol);
						glLineWidth(static_cast<GLfloat>((pdplt % 3) +1));
						glLineStipple((pdplt % 3) +1, StipplePatterns[pdplt / 3]);
						glLoadName(GenerateSelectionID(SELTYPE_TERMINALTRACER, SELSUBTYPE_MAIN, pDoc->m_tempTracerData.GetPaxID(nTrackIdx)));
						glBegin(GL_LINE_STRIP);
						int nHitCount = track.size();
						bool bBeginVertex = false;
						for(int nHit=1; nHit<nHitCount; nHit++) {					
							 CTrackerVector3& hit = track[nHit];
							int nFloorIdx = 0;
							double dAltitude =0 ;
							if(hit.IsRealZ())
							{
								dAltitude = hit[VZ];
							}
							else
							{
								nFloorIdx = static_cast<int>(hit[VZ] / 100);
								double dOffset = (hit[VZ]/100.0 - (double)nFloorIdx);
								dAltitude = dAlt[nFloorIdx]*(1.0-dOffset)  + dAlt[nFloorIdx+1]*dOffset;
							}
							if(bOn[nFloorIdx])
{
								glVertex3f(static_cast<GLfloat>(hit[VX]), static_cast<GLfloat>(hit[VY]),(GLfloat)dAltitude);
								bBeginVertex = true; 
							}
							else if(bBeginVertex)
							{
								glEnd();
								glBegin(GL_LINE_STIPPLE);
								bBeginVertex =false;
							}
						}
						glEnd();
					}
					glDisable(GL_LINE_STIPPLE);
				}

				//airside
				nTrackCount = pDoc->m_tempAirsideTracerData.GetTrackCount();
				if(nTrackCount > 0) {
					glEnable(GL_LINE_STIPPLE);
					for(int nTrackIdx=0; nTrackIdx<nTrackCount; nTrackIdx++) {
						const std::vector<CTrackerVector3>& track = pDoc->m_tempAirsideTracerData.GetTrack(nTrackIdx);
						CAircraftDispPropItem* pADPI = pDoc->m_aircraftDispProps.GetDispPropItem(pDoc->m_tempAirsideTracerData.GetDispPropIdx(nTrackIdx));
						ARCColor3 adpcol(pADPI->GetColor());
						int adplt = (int) pADPI->GetLineType();
						glColor3ubv(adpcol);
						glLineWidth(static_cast<GLfloat>((adplt % 3) +1));
						glLineStipple((adplt % 3) +1, StipplePatterns[adplt / 3]);
						glLoadName(GenerateSelectionID(SELTYPE_AIRSIDETRACER, SELSUBTYPE_MAIN, pDoc->m_tempAirsideTracerData.GetPaxID(nTrackIdx)));
						glBegin(GL_LINE_STRIP);
						int nHitCount = track.size();
						for(int nHit=1; nHit<nHitCount; nHit++) {					
							const CTrackerVector3& hit = track[nHit];
							glVertex3f(static_cast<GLfloat>(hit[VX]), static_cast<GLfloat>(hit[VY]), static_cast<GLfloat>(hit[VZ]));
						}
						glEnd();
					}
					glDisable(GL_LINE_STIPPLE);
				}
			}
			glLineWidth(1.0);
		}

		CHECK_GL_ERRORS("3DView::SelectScene(...), post render tracers");

		//render pipes lines
		glDisable(GL_CULL_FACE);
		if(procType==-1 && GetDocument()->GetCurrentMode() == EnvMode_Terminal )
		{
			CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
			for(int i=0; i<pPipeDS->GetPipeCount(); i++) {
				CPipe* pPipe = pPipeDS->GetPipeAt(i);
				if(pPipe->GetDisplayFlag()) {
					//draw pipe points
					if(i==m_nSelectedPipe && m_bSelPipeEdit) 
					{
						int c = pPipe->GetPipePointCount();
						for(int j=0; j<c; j++) 
						{
							const PIPEPOINT& pt = pPipe->GetPipePointAt(j);
							glLoadName(GenerateSelectionID(SELTYPE_PIPE, SELSUBTYPE_POINT, j));
							DrawFlatSquare(pt.m_point.getX(), pt.m_point.getY(), dAlt[(int) pt.m_point.getZ()/100], 100.0);
							glLoadName(GenerateSelectionID(SELTYPE_PIPE, SELSUBTYPE_WIDTHPOINT, (2*j+0)));
							DrawFlatSquare(pt.m_bisectPoint1.getX(), pt.m_bisectPoint1.getY(), dAlt[(int) pt.m_bisectPoint1.getZ()/100], 75.0);
							glLoadName(GenerateSelectionID(SELTYPE_PIPE, SELSUBTYPE_WIDTHPOINT, (2*j+1)));
							DrawFlatSquare(pt.m_bisectPoint2.getX(), pt.m_bisectPoint2.getY(), dAlt[(int) pt.m_bisectPoint2.getZ()/100], 75.0);
						}
					}

					glLoadName(GenerateSelectionID(SELTYPE_PIPE, SELSUBTYPE_MAIN, i));
					//draw pipe region
					POLLYGONVECTOR polyVect;
					pPipe->GetPipeCoveredRegion(polyVect);
					glColor4ubv(PipeColors[i%16]);
					for(int j=0; j<static_cast<int>(polyVect.size()); j++) {
						Pollygon poly = polyVect[j];
						int c = poly.getCount();
						Point* pts = poly.getPointList();
						int nIndexInFloor=static_cast<int>(pts[0].getZ()/SCALE_FACTOR);
						if(!bOn[nIndexInFloor]) break;
						glBegin(GL_POLYGON);
						glNormal3f(0.0,0.0,1.0);
						for(int k=0; k<c; k++) {
							glVertex3d(pts[k].getX(), pts[k].getY(),dAlt[(int) pts[k].getZ()/100]);
						}
						glEnd();
					}
				}
			}
		}
		glEnable(GL_CULL_FACE);
		CHECK_GL_ERRORS("3DView::SelectScene(...), post render pipes");
		
		
		//render Wall Shape
		if(procType ==-1){
			WallShapeList & walllist=pDoc->GetCurWallShapeList();
			size_t nwallCount=walllist.getShapeNum();
			int ptCount=0;
			for(size_t i=0;i<nwallCount;++i){
				WallShape* wall=walllist.getShapeAt(i);
				if( wall->GetFloorIndex() == pDoc->m_nActiveFloor)
				{
					glPushMatrix();
					glTranslated(0.0,0.0,dAlt[wall->GetFloorIndex()]);
					if(wall->IsEditMode())
					{
						int ptnum=wall->GetPointCount();
						for(int k=0;k<ptnum;++k)
						{
							const Point & pt=wall->GetPointAt(k);
							glLoadName(GenerateSelectionID(SELTYPE_WALLSHAPE, SELSUBTYPE_POINT, k+ptCount));
							DrawFlatSquare(pt.getX(),pt.getY(),0.0,wall->GetWidth());
						}
						glLoadName(GenerateSelectionID(SELTYPE_WALLSHAPE, SELSUBTYPE_MAIN, i));
						wall->DrawSelectArea();
					}
					else
					{                       
						glLoadName(GenerateSelectionID(SELTYPE_WALLSHAPE, SELSUBTYPE_MAIN, i));
						wall->DrawSelectArea();
					}
					glPopMatrix();
				}
				ptCount+=wall->GetPointCount();
			}
		}
		CHECK_GL_ERRORS("3DView::SelectScene(...), post render WallShape");

		//render hold short line
		if(procType == -1)
		{
			std::vector<CTaxiInterruptLine3D* >pHoldShortLine3DList = GetAirside3D()->GetHoldShortLine3DList();
			for (size_t elementCount=0; elementCount<pHoldShortLine3DList.size();elementCount++)
			{
				glLoadName(GenerateSelectionID(SELTYPE_HOLDSHORTLINE,SELSUBTYPE_MAIN,elementCount));
				pHoldShortLine3DList[elementCount]->DrawSelect(this);
			}
		}
		
		//render Structure
		//render CStructures add by ben 2005-9-1
		if(procType==-1)
		{

			CStructureList & StructureList=pDoc->GetCurStructurelist();
			//draw active floor's Structure;
			size_t ncount=StructureList.getStructureNum();
			for(int i=ncount-1;i>=0;--i){
				CStructure* cs = StructureList.getStructureAt(i);
				if(cs->GetFloorIndex()==pDoc->m_nActiveFloor){					
					if(m_bSelectStructureEdit&&m_nSelectedStructure==i)
					{
						int num=cs->getPointNum();					

						for(int j=0;j<num;j++)
						{
							const Point& pt=cs->getPointAt(j);
							glLoadName(GenerateSelectionID(SELTYPE_STRUCTURE, SELSUBTYPE_POINT, j));

							DrawFlatSquare(pt.getX(),pt.getY(),dAlt[cs->GetFloorIndex()],50);
						}

					}	
					glLoadName(GenerateSelectionID(SELTYPE_STRUCTURE, SELSUBTYPE_MAIN, i));
					cs->DrawSelectArea(dAlt[cs->GetFloorIndex()]);			
				}
				else NULL;
			}			
		}

		if(procType==-1)
		{
			CAreaList* pAList = &(pDoc->GetTerminal().m_pAreas->m_vAreas);
			for(int nArea=0; nArea<static_cast<int>(pAList->size()); nArea++) 
			{
				CNamedPointList* pArea = pAList->at(nArea);
				if(bOn[pArea->floor])
				{
					glLoadName(GenerateSelectionID(SELTYPE_TERMINALAREA, SELSUBTYPE_MAIN, nArea));

					glBegin(GL_POLYGON);
					for(int j=0; j<static_cast<int>(pArea->points.size()); j++)
					{
						glVertex3d(pArea->points[j][VX], pArea->points[j][VY], dAlt[pArea->floor]);
					}
					glEnd();
				}
			}
			CPortalList* vPortalList = &(pDoc->m_portals.m_vPortals);
			for(int nPortal=0; nPortal<static_cast<int>(vPortalList->size()); nPortal++) 
			{
				CNamedPointList* pPortal = vPortalList->at(nPortal);
				if(bOn[pPortal->floor])
				{
					glLoadName(GenerateSelectionID(SELTYPE_TERMINALPORTAL, SELSUBTYPE_MAIN, nPortal));


					glBegin(GL_LINES);
					for(int j=0; j<static_cast<int>(pPortal->points.size()); j++) 
					{
						glVertex3d(pPortal->points[j][VX], pPortal->points[j][VY], dAlt[pPortal->floor]);
					}
					glEnd();
				}
			}



		}		
	}
	//Added by Tim In 2:40 4/14/2003---------------------
	else if(pDoc->m_bMobileElementDisplay)
	{
		if(m_bAnimPax)
		{
			DrawPax(pDoc,bOn,dAlt,TRUE);
			DistanceUnit dAirportAlt = GetAirside3D()->GetActiveAirport()->m_altAirport.getElevation();
			DrawFlight(dAirportAlt, TRUE);
			DrawVehicle(dAirportAlt, TRUE);
		}


	}
	//---------------------------------------------------
	glFlush();

	CHECK_GL_ERRORS("3DView::SelectScene(...), post glFlush()");

	hits = glRenderMode(GL_RENDER);

	int nRecordedHits = 0;
	if(0 > hits) {
		//selection buffer overflowed
		GLuint* pSelBufPos = selectBuf;
		while( pSelBufPos < (selectBuf + SELECTION_BUFFER_SIZE) ) {
			int nHits = *pSelBufPos;
			pSelBufPos += (3 + nHits);
			nRecordedHits += nHits;
		}
	}
	else
		nRecordedHits = hits;

	CHECK_GL_ERRORS("3DView::SelectScene(...), post hit return");

	//process hits
	if(bDragSelect)
	{
		if(nRecordedHits > 0)
		{
			for(int i=0; i<nRecordedHits; i++) 
			{
				pSelProc[i] = selectBuf[4*i+3];
			}
		}
		else
			pSelProc[0] = -1;
	}
	else
	{
		if(nRecordedHits > 0)
		{
			BYTE seltype = GetSelectType(selectBuf[3]);
			BYTE selsubtype = GetSelectSubType(selectBuf[3]);

			pSelProc[0] = selectBuf[3];

			if(nRecordedHits > 1) {
				if(SELTYPE_RAILWAY == seltype && SELSUBTYPE_MAIN == selsubtype) {
					for(int i=1; i<nRecordedHits; i++) {
						GLuint nextHit = selectBuf[4*i+3];
						if(SELTYPE_RAILWAY == GetSelectType(nextHit) && SELSUBTYPE_MAIN != GetSelectSubType(nextHit)) {
							pSelProc[i] = nextHit;
							break;
						}
					}
				}
				else if(SELTYPE_PIPE == seltype && SELSUBTYPE_MAIN == selsubtype) {
					for(int i=1; i<nRecordedHits; i++) {
						GLuint nextHit = selectBuf[4*i+3];
						if(SELTYPE_PIPE == GetSelectType(nextHit) && SELSUBTYPE_MAIN != GetSelectSubType(nextHit)) {
							pSelProc[i] = nextHit;
							break;
						}
					}
				}
				else if(SELTYPE_STRUCTURE == seltype && SELSUBTYPE_MAIN == selsubtype) {
					for(int i=1; i<nRecordedHits; i++) {
						pSelProc[i] = selectBuf[4*i+3];
					}
				}	
				else if(SELTYPE_TAXIWAY == seltype && SELSUBTYPE_MAIN == selsubtype) {
					for(int i=1; i<nRecordedHits; i++) {
						pSelProc[i] = selectBuf[4*i+3];
					}
				}
				else if(SELTYPE_TAXIWAY==seltype && SELSUBTYPE_POINT== selsubtype){
					for(int i=1; i<nRecordedHits; i++) {
						pSelProc[i] = selectBuf[4*i+3];
					}
				}
				else if(SELTYPE_STRUCTURE == seltype && SELSUBTYPE_POINT == selsubtype) {
					for(int i=1; i<nRecordedHits; i++) {
						pSelProc[i] = selectBuf[4*i+3];
					}
				}
				else if(SELTYPE_LANDSIDEPROCESSOR == seltype ){
					//resort the select hit by  min z val									
					std::list<int> sortedidx;std::list<int>::iterator itr;

					for(int i=0; i<nRecordedHits; i++) {
						unsigned int z = selectBuf[ 4*i + 1 ];
						for(itr = sortedidx.begin();itr!=sortedidx.end();itr++)
						{
							if( selectBuf[ 4*(*itr)+1 ] <= z )break;
						}
						sortedidx.insert( itr, i );
					}	
					
					itr = sortedidx.begin();
					for(int i=0;i<nRecordedHits;i++)
					{						
						pSelProc[i] = selectBuf[4*(*itr)+3];
						itr++;
					}
					
				}

				else{
					for(int i=1; i<nRecordedHits; i++) {
						pSelProc[i] = selectBuf[4*i+3];
					}
				}
			}
		}		
		else	
			pSelProc[0] = -1;
	}
	::wglMakeCurrent(NULL, NULL);
	return nRecordedHits;
}


int C3DView::SelectAirsideScene( int nFlag, int x, int y , CSize sel_size, SelectableSettings& selsetting , SelectableList& reslt )
{
	static GLuint selectBuf[SELECTION_BUFFER_SIZE];
	GLint hits;
	GLint viewport[4];
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return -1;
	glGetIntegerv(GL_VIEWPORT, viewport);
	glSelectBuffer(SELECTION_BUFFER_SIZE, selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((GLdouble) x, (GLdouble) (viewport[3] - y), sel_size.cx,sel_size.cy, viewport);
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();

	m_SelectionMap.Clear();
	
	GetParentFrame()->GetAirside3D()->DrawSelectOGL(this, selsetting);

	glFlush();

	hits = glRenderMode(GL_RENDER);

	if( sel_size == CSize(2,2) )// select object later
	{
		for(int i =0;i<hits;i++){
			int id = selectBuf[4*i+3];
			Selectable * pSel = m_SelectionMap.GetSelectable(id);
			if(pSel && pSel->GetSelectType() == Selectable::ALT_OBJECT)
				continue;	
			
			reslt.insert(pSel);			
			return 1;
		}
	}
	//select objects
	
	for(int i =0;i<hits;i++){
		int id = selectBuf[4*i+3];
		Selectable * pSel = m_SelectionMap.GetSelectable(id);
		if(pSel->GetSelectType() == Selectable::ALT_OBJECT ){			
			reslt.insert(pSel);
			if(sel_size == CSize(2,2) ) 
				return 1;
		}
	}


	return hits;
}
void C3DView::Draw()
{
	
	// Is this "busy" thing even useful?
	// If we assert below, replace with test & return
	//
	CChildFrame* pFrame = (CChildFrame*)GetParentFrame();
	// status bar
	pFrame->m_wndStatusBar.OnUpdateCmdUI(pFrame, FALSE);


	// m_bBusy = FALSE;
	if(m_bBusy)
		return;
	//ASSERT(!m_bBusy);

	// Set busy flag
	m_bBusy = TRUE;

	// Make the rendering context current
	if(! ::wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC)) {
		ASSERT(FALSE);
		return;
	}

	// We are calculating the true frame rate...
	// the time between Draw() calls.
	CalculateFrameRate();	
	
//	DrawStencil();

	DrawScene();

	DrawText();
	
	
	::SwapBuffers(m_pDC->GetSafeHdc());	
	
	m_bBusy = FALSE;	
	
}

void C3DView::DrawStencil()
{
	GetAirside3D()->GetActiveAirport()->RenderStenciles();
}
static void drawCallOutDlgLines(CTermPlanDoc* pDoc, C3DView* pview);
void C3DView::DrawText()
{
	// 1) Clear the depth buffer
	// 2) *3D only* Render floors in "no texture" mode to depth buffer only
	// 3) Render Text
	glClear(GL_DEPTH_BUFFER_BIT);

	const CFloorList& vFloorList = GetDocument()->GetFloorByMode( EnvMode_Terminal ).m_vFloors;

	if(C3DCamera::perspective == GetCamera()->GetProjectionType()) {
		glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
		int nFloorCount = vFloorList.size();

		for(int i=0; i<nFloorCount; i++) 
		{
			(vFloorList[i])->DrawOGLNoTexture(true,vFloorList[i]->Altitude());
		}
		//
        //GetDocument()->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->DrawOGLNoTexture();

		glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
	}
	RenderText();
	
	RenderBillBoard();
	//drawCallOutDlgLines(GetDocument(),this) ;
}

void C3DView::DrawScene()
{
	// clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if(!m_bUseSharpTex) {	// SHARP is OFF
		//
		// Render the scene normally.
		
		// Set the projection and model view matrices
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//Apply the perspective or ortho view
		ApplyPerspective();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLightfv(GL_LIGHT1, GL_POSITION, default_lightPosition);	// Position The Light
		// Apply the direction where you look
		ApplyViewXForm();
		// Extract the Frustum view
		GetCamera()->ExtractFrustum();

		// 3D case
		if(GetCamera()->GetProjectionType() == C3DCamera::perspective) {
			
			RenderScene();
			

		} //end 3D case

		else { // 2D case
			if(!m_bAutoSharp) { // normal 2d

				RenderScene();
			
			}
			else { // auto-sharp 2d
				RenderScene();
			}
		} //end 2D case
		
		
	} //end SHARP is OFF
	
	else { // SHARP is ON	
		//
		// Render the scene in sharp mode.

		// Get the current viewport
		
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0, viewport[2], 0.0, viewport[3],-1.0,1.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		
		glDepthMask(GL_FALSE); 

		// Enable texturing (rectangular texture)
		
		glEnable(GL_TEXTURE_RECTANGLE_NV);
		// Set texturing mode to decal
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
		// Bind sharp texture
		glBindTexture(GL_TEXTURE_RECTANGLE_NV, m_nSharpTexId);

		// Draw sharp texture in background

		
		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_QUADS);
		glNormal3d(0.0,0.0,1.0);
		glTexCoord2d(0.0,0.0);
		glVertex3d(0.0,0.0,0.0);
		glTexCoord2d(viewport[2], 0.0);
		glVertex3d(viewport[2], 0.0,0.0);
		glTexCoord2d(viewport[2], viewport[3]);
		glVertex3d(viewport[2],viewport[3],0.0);
		glTexCoord2d(0.0,viewport[3]);
		glVertex3d(0.0,viewport[3],0.0);
		glEnd();

		// Disable texturing (rectangular texture)
		glDisable(GL_TEXTURE_RECTANGLE_NV);
		// Re-enable depth buffer writing
		glDepthMask(GL_TRUE); 
		
		// STEP 2) Render floors to depth buffer only
		//         (i.e. with color buffer writing disabled 

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		ApplyPerspective();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glLightfv(GL_LIGHT1, GL_POSITION, default_lightPosition);	// Position The Light
		ApplyViewXForm();
		//
		GetCamera()->ExtractFrustum();

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f,1.0f);
		std::vector<CFloor2*> vSortedFloors(GetDocument()->GetCurModeFloor().m_vFloors);		
		std::sort(vSortedFloors.begin(), vSortedFloors.end(), CBaseFloor::CompareFloors);
		
		
		if(GetCamera()->GetProjectionType() == C3DCamera::perspective) 
		{
			//Current camera is a 3D camera
			//render the floors as untextured quads only to depth buffer (disable color buffer)
			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			int nFloorCount = vSortedFloors.size();
			for(int i=0; i<nFloorCount; i++) 
			{
				//(GetDocument()->m_floors.m_vFloors[i])->DrawOGLNoTexture();
				vSortedFloors[i]->DrawOGLNoTexture(true, vSortedFloors[i]->Altitude());
			}
			//
			//GetDocument()->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->DrawOGLNoTexture();

			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
		}
		
		glDisable(GL_POLYGON_OFFSET_FILL);
		// STEP 3) Render scene without floors		
		RenderScene(FALSE);	

	} //end SHARP is ON


	glDisable(GL_LIGHTING);
	glEnable(GL_BLEND);
	DrawGrids(GetDocument(),this);	
	glDisable(GL_BLEND);
	glEnable(GL_LIGHTING);	
	RenderCompass();
	glDisable(GL_LIGHTING);
	//render radar
	if(m_bShowRadar && GetCamera()->GetProjectionType() == C3DCamera::parallel) {
		RenderRadar();
	}

}

void C3DView::SetTitle()
{
	CTermPlanDoc* pDoc = GetDocument();
	CString cadName = "3D Terminal View of " + pDoc->GetTitle();

	CFrameWnd* pFrameWnd = GetParentFrame();
	CString str;
	pFrameWnd->GetWindowText(str);
	
	pFrameWnd->SetWindowText(cadName);
}

void C3DView::OnInitialUpdate() 
{
	m_nRedSquareBlinkTimer = SetTimer(m_nRedSquareBlinkTimer, 250, NULL);
	CView::OnInitialUpdate();
}

void C3DView::OnSetFocus(CWnd* pOldWnd) 
{
	CView::OnSetFocus(pOldWnd);
	//// TRACE("3DView of %s activated\n", GetDocument()->GetTitle());
	GetParentFrame()->RefreshSplitBars();
	Invalidate(FALSE);
	//ANIMTIMEMGR->EnrollDocument(GetDocument());
	//ANIMTIMEMGR->EnrollView(this);
	
}

void C3DView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	//// TRACE("3DView of %s de-activated\n", GetDocument()->GetTitle());
	GetParentFrame()->RefreshSplitBars();
	//ANIMTIMEMGR->EnrollDocument(NULL);
	//ANIMTIMEMGR->EnrollView(NULL);
	
}

DROPEFFECT C3DView::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_TerminalShape
		&& GetDocument()->GetCurrentMode() == EnvMode_Terminal
		)
	{
		CShape::SHAPEINFO* pShapeInfo = (CShape::SHAPEINFO*)dataItem.GetItem(0).GetData();
		m_pDragShape = pShapeInfo->pShape;
		return DROPEFFECT_COPY;
	}
	m_pDragShape = NULL;
	return DROPEFFECT_NONE;
}

DROPEFFECT C3DView::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point) 
{
	OnMouseMove(dwKeyState, point);

	if(m_pDragShape != NULL)
		return DROPEFFECT_COPY;
	else
		return DROPEFFECT_NONE;

}

void C3DView::OnDragLeave() 
{
	CView::OnDragLeave();
	if(m_pDragShape != NULL) {
		m_pDragShape = NULL;
	}
}

BOOL C3DView::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	CView::OnDrop(pDataObject, dropEffect, point);

	DragDropDataItem dataItem;
	if (OleDragDropManager::GetInstance().GetDragDropData(dataItem, pDataObject)
		&& dataItem.GetDataType() == DragDropDataItem::Type_TerminalShape
		&& GetDocument()->GetCurrentMode() == EnvMode_Terminal
		)
	{
		CShape::SHAPEINFO* pShapeInfo = (CShape::SHAPEINFO*)dataItem.GetItem(0).GetData();
		CShape* pDropShape = pShapeInfo->pShape;

		ARCVector3 mpos = Get3DMousePos(point);
		// TRACE("Dropped shape: %s at point:(%.2f,%.2f) %s\n", pDropShape->Name(), UNITSMANAGER->ConvertLength(mpos[VX]),UNITSMANAGER->ConvertLength(mpos[VY]), UNITSMANAGER->GetLengthUnitString(UNITSMANAGER->GetLengthUnits()));
		ARCVector3 mpos2D(mpos[VX], mpos[VY],0);

		CTermPlanDoc* pDoc = GetDocument();
		//create a CProcessor2 object
		CProcessor2* pNewProc2 = new CProcessor2();
		//set floor to active floor
		pNewProc2->SetFloor(pDoc->m_nActiveFloor);
		//set location, use default scale and rotation (set in constructor)
		pNewProc2->SetLocation(mpos2D);
		//set shape to pDropShape
//		pDropShape->ShapeName(pDropShape->Name());
		pNewProc2->SetShape(pDropShape);
		//set selection id
		pNewProc2->SetSelectName(pDoc->GetUniqueNumber());
		//set display properties to default
		GetDefaultProcDispProperties(&(pDoc->m_defDispProp), &(pNewProc2->m_dispProperties));
		//add new CProcessor2 to doc's m_Placements.m_vUndefined list
		CString strValue(pDropShape->Name());
		strValue.MakeUpper();
		CString strTemp = _T("");
	
		strTemp = pDoc->GetTerminalPlacement()->m_vUndefined.GetName(strValue);
	
		pNewProc2->ShapeName(strTemp);	
		CString sz = pNewProc2->ShapeName().GetIDString();
		pDoc->GetCurrentPlacement()->m_vUndefined.push_back(pNewProc2);
		pDoc->GetCurrentPlacement()->m_vUndefined.setNode(ALTObjectID(strTemp));
		pDoc->GetCurrentPlacement()->m_vUndefined.InsertUnDefinePro2(std::make_pair(strTemp,pNewProc2));
		//save dataset
		pDoc->GetCurrentPlacement()->saveDataSet(pDoc->m_ProjInfo.path, true);
		//add a node to the under construction node
		pDoc->AddToUnderConstructionNode(pNewProc2);
		Invalidate(FALSE);
		GetDocument()->UpdateAllViews(this);
		return TRUE;
	}

	return FALSE;
}

BOOL C3DView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void C3DView::OnUpdateGridSpacing(CCmdUI *pCmdUI)
{
	C3DView* p3DView = this;
	if(p3DView) { // if its valid
		CGrid* pGrid = p3DView->GetDocument()->GetActiveFloor()->GetGrid();
		ASSERT(pGrid != NULL);
		double ds = pGrid->dLinesEvery;//= p3DView->GetDocument()->GetActiveFloor()->GridSpacing();
		CString s(_T(""));
		CUnitsManager* pUM = CUnitsManager::GetInstance();
		CTermPlanDoc * pDoc = GetDocument();
		if(pDoc && pDoc->m_systemMode == EnvMode_AirSide)
			s.Format("\tGrid Spacing: %5.2f %s",CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),ds),CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
		else
			s.Format("\tGrid Spacing: %5.2f %s",pUM->ConvertLength(ds),pUM->GetLengthUnitString(pUM->GetLengthUnits()));
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
	}
	else
		pCmdUI->Enable(FALSE);
}

void C3DView::UpdateFloorOverlay(int nLevelID, CPictureInfo& picInfo)
{
	//if image file does not exist ,return
	CAirsideGround * pGround = GetAirside3D()->GetAirportLevel(nLevelID);
	if(pGround)
	{
		pGround->SetPictureFilePath(picInfo.sPathName);
		pGround->SetPictureFileName(picInfo.sFileName);
		pGround->m_picInfo.refLine = picInfo.refLine;
		pGround->m_picInfo.dRotation = picInfo.dRotation;
		pGround->m_picInfo.dScale = picInfo.dScale;
		pGround->m_picInfo.vOffset = picInfo.vOffset;
		pGround->m_picInfo.bShow = picInfo.bShow;
	}

	Invalidate(FALSE);

}



void C3DView::OnUpdateAnimTime(CCmdUI *pCmdUI)
{
	CTermPlanDoc* pDoc = GetDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none) {
		long nTime = pDoc->m_animData.nLastAnimTime;
		BOOL bMoveDay = pDoc->m_animData.bMoveDay;
		
		int nHour = nTime / 360000;
		int nMin = (nTime - nHour*360000)/6000;
		int nSec = (nTime - nHour*360000 - nMin*6000)/100;


		//	Modified by kevin
		CString s(_T(""));
		
		CStartDate startDate = pDoc->m_animData.m_startDate;
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		startDate.GetDateTime( ElapsedTime(nTime / TimePrecision), bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			s = date.Format(_T("%Y-%m-%d"));
		}	
		else
		{
			if(bMoveDay)
				nDtIdx -= 1;
			s.Format(_T("Day %d"), nDtIdx + 1);
		}
		s += _T("  ") + time.Format(_T("%H:%M:%S"));

//		FILE *fw = fopen("C:\\debug.txt","a+");
//		fprintf(fw, "%s\n", (char*)(const char*)s);
//		fclose(fw);

		if( NULL == pCmdUI )
		{
			if(pCmdUI)
			{
				pCmdUI->Enable(TRUE);
				pCmdUI->SetText( s );

				pCmdUI->m_pOther->UpdateWindow();
			}
		}
		else
		{
			pCmdUI->Enable(TRUE);
			pCmdUI->SetText( s );

			pCmdUI->m_pOther->UpdateWindow();
		}
	}
	else
	{
		if(pCmdUI){
			if( GetParentFrame()->IsLoadingModel() )
			{
				CString loadingText;
				loadingText.Format("Loading Shapes (%d %)...", (int)GetParentFrame()->GetLoadingPercent() * 100);
				pCmdUI->Enable(TRUE);
				pCmdUI->SetText(loadingText);
				pCmdUI->m_pOther->UpdateWindow();
			}
			else
			{
				pCmdUI->Enable(FALSE);
			}
		}
	}

}

void C3DView::OnUpdateAnimationSpeed(CCmdUI* pCmdUI)
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetParentFrame()->GetActiveDocument();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none)
	{
		CString s(_T(""));
		s.Format("\tSpeed: %5dx",pDoc->m_animData.nAnimSpeed/10);
		pCmdUI->Enable(TRUE);
		pCmdUI->SetText( s );
		pCmdUI->m_pOther->UpdateWindow();
	}
	else
	{
		if(pCmdUI){
			if(pDoc->m_eAnimState == CTermPlanDoc::anim_none)
			{
				pCmdUI->Enable(FALSE);
			}				
		}
	}
}

void C3DView::OnUpdateFPS(CCmdUI* pCmdUI)
{
	CString s(_T(""));
	s.Format("\tFPS: %5.2f",m_dFrameRate);
	pCmdUI->Enable(TRUE);
	pCmdUI->SetText( s );
}

void C3DView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar) {
	case '-':
		GetDocument()->OnScaleProc(-static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case '+':
		GetDocument()->OnScaleProc(static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'x':
		GetDocument()->OnScaleProcX(-static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'X':
		GetDocument()->OnScaleProcX(static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'y':
		GetDocument()->OnScaleProcY(-static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'Y':
		GetDocument()->OnScaleProcY(static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'z':
		GetDocument()->OnScaleProcZ(-static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	case 'Z':
		GetDocument()->OnScaleProcZ(static_cast<double>(nRepCnt),0.0);
		Invalidate(FALSE);
		break;
	}
	//// TRACE("Char: %c = %d\n", nChar, nChar);
	CView::OnChar(nChar, nRepCnt, nFlags);
}

void C3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
	case VK_DELETE:
		OnCtxDeleteProc();
		OnAltobjectDelete();
		break;
	case VK_INSERT:
		OnCtxCopyProcessor();
		OnAltobjectCopy();
		break;
	case VK_LEFT:
		GetDocument()->OnRotateShape(static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_RIGHT:
		GetDocument()->OnRotateShape(-static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_UP:
		GetDocument()->OnRotateProc(static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	case VK_DOWN:
		GetDocument()->OnRotateProc(-static_cast<double>(nRepCnt));
		Invalidate(FALSE);
		break;
	
	case VK_ESCAPE:
		break;
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void C3DView::ClearPointPickStruct()
{
	m_vMousePosList.clear();
}

void C3DView::OnCtxProcproperties() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	if (GetDocument()->GetSelectProcessorsCount() == 0)
		return;
	CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjDisplay == NULL || pObjDisplay->GetType() != ObjectDisplayType_Processor2)
		return;

	CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
	
	ARCVector3 _location;
	_location = pProc2->GetLocation();
	//_location[VZ] = pProc2->GetFloor()+0.5;
	_location[VZ] = GetDocument()->m_nActiveFloor * SCALE_FACTOR ;

	
	ProcessorID id;
	id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
	if(pProc2->GetProcessor() != NULL)
	{
		id = *(pProc2->GetProcessor()->getID());
	}
	CProcPropDlg dlg( pProc2->GetProcessor(), _location,this );
	
	if(dlg.DoModal() == IDOK) {
		//	Change Shape
		//int nShapeIndex = dlg.GetShapeIndex();
		//OnChangeShapesMenuList(CHANGE_SHAPES_MENU_LIST_ID_BEGIN + nShapeIndex);

		Processor* pProc = dlg.GetProcessor();
		if(pProc2->GetProcessor() == NULL) {  //we have a new Processor
			pProc2->SetProcessor(pProc);
			if(pProc->getProcessorType() == IntegratedStationProc) {
				GetDefaultStationDisplayProperties(&(pProc2->m_dispProperties));
				ARCVector3 oldloc;
				oldloc = pProc2->GetLocation();
				pProc2->SetLocation(ARCVector3(oldloc[VX],oldloc[VY],0));
			}
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
			//remove from undefined processor list and put in defined processor list
			CPROCESSOR2LIST* pUndefList = &(GetDocument()->GetCurrentPlacement()->m_vUndefined);
			CPROCESSOR2LIST* pDefList = &(GetDocument()->GetCurrentPlacement()->m_vDefined);
			int size = pUndefList->size();
			for(int i=0; i<static_cast<int>(pUndefList->size()); i++) {
				if(pUndefList->at(i) == pProc2) {
					pUndefList->removePro2(pProc2);
					pUndefList->erase(pUndefList->begin()+i);
					break;
				}
			}
			//ASSERT(i < size);
			pDefList->push_back(pProc2);
			
			if (pProc->getProcessorType()==StairProc)
			{
				pProc2->m_dispProperties.color[PDP_DISP_SHAPE]=ARCColor3(255,255,0);//yellow
			}
			else if(pProc->getProcessorType()==EscalatorProc)
			{
				pProc2->m_dispProperties.color[PDP_DISP_SHAPE]=ARCColor3(210,180,140);//tan/light brown
			}
			
			//remove from undefined nodes and put in defined nodes
			GetDocument()->MoveFromUCToDefined(pProc2);
		}
		else { //existing processor
			pProc2->SetProcessor(pProc);
			GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
			if (!id.idFits(*pProc->getID()))
			{
				std::stack<CString>vList;
				while(id.idLength())
				{
					vList.push(id.GetLeafName());
					id.clearLevel(id.idLength() - 1);
				}
				
				CTVNode* pPlacement = GetDocument()->ProcessorsNode();
				CString sName(_T(""));
				CTVNode* pNode = pPlacement;
				while(vList.size())
				{
					sName = vList.top();
					vList.pop();
				
					if (pNode)
					{
						pNode = (CTVNode*)pNode->GetChildByName(sName);
					}
				}
			
				GetDocument()->GetNodeView()->RefreshProcessorItem(pNode,pPlacement,*pProc->getID());
			}
		}

	}
}
void C3DView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	if(this == pSender)
		return;

	if(lHint == VM_CHANGENAME_ALTOBJECT)
	{
		ALTObject * pObj = (ALTObject*) pHint;	
		ALTObject3D* pObj3D = GetAirside3D()->GetObject3D(pObj->getID());
		if(pObj3D && pObj3D->GetObject() )
		{
			pObj3D->GetObject()->setObjName(pObj->GetObjectName());
		}
	}
	if(lHint == VM_MODIFY_ALTOBJECT)
	{
		ALTObject * pObj = (ALTObject*) pHint;		
		GetAirside3D()->UpdateChangeOfObject(pObj);	
	}
	if(lHint == VM_DELECT_ALTOBJECT)
	{
		GetAirside3D()->UpdateAddorRemoveObjects();
	}
	if(lHint == VM_NEW_SINGLE_ALTOBJECT)
	{		
		GetAirside3D()->UpdateAddorRemoveObjects();		
	}

	if(lHint == VM_SELECT_ALTOBJECT )
	{
		GetParentFrame()->UnSelectAll();
		ALTObject3D * pObj = GetAirside3D()->GetObject3D((int)pHint);
		if(pObj)
			GetParentFrame()->Select(pObj);
	}

	if(lHint == VM_MODIFY_AIRSIDEGROUND)
	{
		for(int i =0;i<(int)GetParentFrame()->GetAirside3D()->GetAirportList().size();++i){
			GetParentFrame()->GetAirside3D()->GetAirportList().at(i)->ReadGroundInfo();
		}
	}

	if(lHint == VM_MODIFY_AIRPORT)
	{		
		CAirport3D * pAirport = GetParentFrame()->GetAirside3D()->GetAirport((int)pHint);
		if(pAirport)pAirport->Update();
		GetDocument()->SetCameraWBS();
	}

	if(lHint == VM_ADD_OR_DELETE_AIRPORT)
	{
		GetParentFrame()->GetAirside3D()->UpdateAddorRemoveAirports();
	}
	if(lHint == VM_UPDATA_ALTOBJECTS)
	{
		std::vector<UINT> *altobjectidlist = (std::vector<UINT> *)pHint;
		for(std::vector<UINT>::iterator it=altobjectidlist->begin(); it!=altobjectidlist->end(); ++it)
		{				
			GetAirside3D()->UpdateObject(*it);

		}
	}
	if (lHint == VM_MODIFY_HOLDSHORTLINE)
	{
		GetAirside3D()->UpdateHoldShortLineInfo();
	}	
	if( lHint == VM_MODIFY_AIRROUTE )
	{
		GetAirside3D()->GetAirspace()->UpdateAirRoute((int)pHint);
	}
	if( lHint == VM_SET_AIRSIDEPLACEMARKER )
	{
		CAirsideGround* pAG = GetAirside3D()->GetActiveLevel();
		if(pAG){
			pAG->UseMarker(TRUE);
			pAG->SetMarkerLocation(*(ARCVector2*)pHint);
		}
	}
	if( lHint == VM_SET_AIRSIDEPLACEMARKERLINE)
	{
		CAirsideGround* pAG = GetAirside3D()->GetActiveLevel();
		if(pAG){
			pAG->UseAlignLine(TRUE);
			pAG->SetAlignLine(*(CAlignLine*)pHint);
		}
	}
	if( lHint == VM_REMOVE_AIRSIDEPLACEMARKER )
	{
		CAirsideGround* pAG = GetAirside3D()->GetActiveLevel();
		if(pAG)
			pAG->UseMarker(FALSE);
	}
	if(lHint == VM_REMOVE_AIRSIDEPLACEMARKERLINE )
	{
		CAirsideGround* pAG = GetAirside3D()->GetActiveLevel();
		if(pAG)
			pAG->UseAlignLine(FALSE);
	}
	if(lHint == VM_ALIGN_AIRSIDE_MARKS)
	{
		CAirsideGround* pAG = GetAirside3D()->GetActiveLevel();
		if(pAG)
		{			
			FuntionAlignFloors(pAG, GetDocument()->GetFloorByMode(EnvMode_Terminal).m_vFloors, pAG);
			GetDocument()->GetFloorByMode(EnvMode_Terminal).saveDataSet( GetDocument()->m_ProjInfo.path, false);
		}	
	}
	
	Invalidate(FALSE);
	CView::OnUpdate(pSender, lHint, pHint);
}

void C3DView::OnCtxCopyProcessor() 
{
	if(GetDocument()->GetSelectProcessors().GetCount() > 0) 
	{
		//CopyProcessorsCommand* c = new CopyProcessorsCommand(GetDocument()->GetSelectProcessors(), GetDocument());
		//c->Do();
		//GetDocument()->m_CommandHistory.Add(c);
		GetDocument()->CopyProcessors(GetDocument()->GetSelectProcessors());
	}
}

void C3DView::OnCtxRotateProcessors() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0 );
	if (GetDocument()->GetSelectProcessorsCount() == 0)
		return;

	m_eMouseState = NS3DViewCommon::_rotateproc;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();
	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;

		//pProc2->GetScale(GetDocument()->m_ptProcCmdPosStart);
		GetDocument()->m_ptProcCmdPosStart = ARCVector3(pProc2->GetRotation(), 0.0, 0.0);

	}
}

void C3DView::OnCtxScaleProcessors() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	if (GetDocument()->GetSelectProcessorsCount() == 0)
		return;

	m_eMouseState = NS3DViewCommon::_scaleproc;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();
	

	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;

		pProc2->GetScale(GetDocument()->m_ptProcCmdPosStart);
	}

}

void C3DView::OnCtxScaleProcessorsX() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);

	if (GetDocument()->GetSelectProcessorsCount() == 0)
		return;

	m_eMouseState = NS3DViewCommon::_scaleprocX;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();
	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;
		pProc2->GetScale(GetDocument()->m_ptProcCmdPosStart);

	}


}

void C3DView::OnCtxScaleProcessorsY() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	m_eMouseState = NS3DViewCommon::_scaleprocY;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();


	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;

		pProc2->GetScale(GetDocument()->m_ptProcCmdPosStart);
	}

}

void C3DView::OnCtxScaleProcessorsZ() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	m_eMouseState = NS3DViewCommon::_scaleprocZ;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();

	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;
		pProc2->GetScale(GetDocument()->m_ptProcCmdPosStart);
	}

}

void C3DView::OnCtxMoveProcessors() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0 );
	m_eMouseState = NS3DViewCommon::_moveprocs;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
}

void C3DView::OnCtxDeleteProc() 
{
	int nSize = GetDocument()->GetSelectProcessors().GetCount();
	if(nSize <= 0)
		return;
	if(nSize==1) 
	{
		CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
		if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
		{
			return;
		}
		CProcessor2* pSelecProc2 = (CProcessor2 *)pObjectDisplay;
		Processor *pSelecProc= pSelecProc2->GetProcessor();
		//if(AfxMessageBox("Are you sure you want to delete the selected processor?", MB_YESNO) == IDYES)
		if(pSelecProc!=NULL)
		{
			CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),(ProcessorID *)(pSelecProc->getID()));
			CUsedProcInfoBox box;
			box.AddInfo(usedProcInfo);
			CPaxFlowConvertor pPaxFlowConvertor;
			CAllPaxTypeFlow pAllPaxFlow;
			pPaxFlowConvertor.SetInputTerm(&(GetDocument()->GetTerminal()));
			pAllPaxFlow.FromOldToDigraphStructure( pPaxFlowConvertor );
			int nAllPaxFlowCount = pAllPaxFlow.GetSinglePaxTypeFlowCount();
			int n = 0;
			for (n;n < nAllPaxFlowCount; n++)
			{
				CSinglePaxTypeFlow* pCurAllPaxTypeFlow = NULL;
				pCurAllPaxTypeFlow = pAllPaxFlow.GetSinglePaxTypeFlowAt(n);
				int nFlowList = pCurAllPaxTypeFlow->GetFlowPairCount();
				int m = 0;
				for (m;m < nFlowList;m++)
				{
					CProcessorDestinationList* pCurListFlow = NULL;
					int t = 0;
					pCurListFlow = pCurAllPaxTypeFlow->GetFlowPairAt(m);
					int nListCount = pCurListFlow->GetDestCount();
					for (t;t < nListCount ; t++)
					{
						CFlowDestination* pCurDestFlow = pCurListFlow->GetDestProcArcInfo(t);
						if (pCurDestFlow->GetProcID().idFits(*(pSelecProc->getID())))
						{
							if (pCurDestFlow->GetOneToOneFlag())
							{								
								CString strMsg;
								strMsg.Format(_T(" The processor %s is using in Flow and has 1:1 constraint, it can't be deleted. \r\n Please retry after remove the reference."),pSelecProc->getID()->GetIDString());
								MessageBox(strMsg,_T("ARCport warning"), MB_OK);
								return;
							}
							break;
						}
					}
				}
			}
			int nIndex = 0;
			int nSubFlowCount = GetDocument()->GetTerminal().m_pSubFlowList->GetProcessUnitCount();
			for(nIndex;nIndex < nSubFlowCount;nIndex++)
			{
				CSinglePaxTypeFlow* pCurSingPaxTypeFlow = NULL;
				pCurSingPaxTypeFlow = GetDocument()->GetTerminal().m_pSubFlowList->GetProcessUnitAt(nIndex)->GetInternalFlow();
				int nFlowListCount = pCurSingPaxTypeFlow->GetFlowPairCount();
				int k = 0;
				for (k; k < nFlowListCount;k++)
				{
					CProcessorDestinationList* pCurDestListFlow = NULL;
					pCurDestListFlow = pCurSingPaxTypeFlow->GetFlowPairAt(k);
					int n = 0;
					int nPaxFlowCount = pCurDestListFlow->GetDestCount();
					for (n; n < nPaxFlowCount;n++)
					{
						CFlowDestination* pCurFlowDest = NULL;
						pCurFlowDest = pCurDestListFlow->GetDestProcArcInfo(n);
						if (pCurFlowDest->GetProcID().idFits(*(pSelecProc->getID())))
						{
							if (pCurFlowDest->GetOneToOneFlag())
							{
								CString strMsg;
								strMsg.Format(_T(" The processor %s is using in Process and has 1:1 constraint, it can't be deleted.\r\n Please retry after remove the reference."),pSelecProc->getID()->GetIDString());
								MessageBox(strMsg,_T("ARCport warning"), MB_OK);
								return;
							}
							break;
						}
					}
				}
			}
			if(pSelecProc->getProcessorType() == DepSinkProc || pSelecProc->getProcessorType() == DepSourceProc)
			{
				ProcessorClassList emProType;
				MiscProcessorIDWithOne2OneFlag* pProcIDWithOne2One = NULL;
				switch(pSelecProc->getProcessorType())
				{
				case DepSinkProc:
					emProType = DepSourceProc;
					break;
				case DepSourceProc:
					emProType = DepSinkProc;
					break;
				default:
					break;
				}
				MiscProcessorData* pMiscDB = GetDocument()->GetTerminal().miscData->getDatabase( (int)emProType);
				int DBlist =pMiscDB->getCount();
				int i= 0;
				for (i;i<DBlist;i++)
				{
		//			if (pMiscDB->getItem(i)->getID()->idFits(*(pSelecProc->getID())))
					{
						MiscData* pMiscData = ((MiscDataElement*)pMiscDB->getItem(i))->getData();
						MiscDepSourceData *data = (MiscDepSourceData *)pMiscData;
						ProcessorIDList *idList = data->getDependentList();
						int listCount = MIN ((idList->getCount()),128);
						int k = 0;
						for (k; k < listCount; k++)
						{
							if ((MiscProcessorIDWithOne2OneFlag*)(idList->getID(k))->idFits(*((pSelecProc->getID()))))
							{
								pProcIDWithOne2One=(MiscProcessorIDWithOne2OneFlag*)idList->getID (k);
								if (pProcIDWithOne2One->getOne2OneFlag())
								{
									CString strMsg;
									strMsg.Format(_T(" The processor %s is using in Behavior and has 1:1 constraint, it can't be deleted.\r\n Please retry after remove the reference."),pSelecProc->getID()->GetIDString());
									MessageBox(strMsg,_T("ARCport warning"), MB_OK);
									return;
								}
								break;
							}
						}
					}
				}			
			}
			if(!box.IsNoInfo())
			{
				if(MessageBox("Delete processor,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
				{
					CObjectDisplayList vProcessors = GetDocument()->GetSelectProcessors();
					GetDocument()->DeleteProcessors(vProcessors);
				}
				
			}
			else
			{	
				if(box.DoModal()==IDOK)
				{
// 					CObjectDisplayList vProcessors = GetDocument()->GetSelectProcessors();
// 					GetDocument()->DeleteProcessors(vProcessors);
				}
			}
		}
		else
		{
			GetDocument()->DeleteProcessors(GetDocument()->GetSelectProcessors());
		}
	}
	else 
	{
		CUsedProcInfoBox *box = new CUsedProcInfoBox();
		for(int j=0;j<nSize;j++)
		{
			CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(j);
			if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
			{
				continue;
			}

			Processor * pProc =((CProcessor2*)pObjectDisplay)->GetProcessor();
			if(pProc!=NULL)
			{
				CUsedProcInfo usedProcInfo(&GetDocument()->GetTerminal(),(ProcessorID*)pProc->getID());
				box->AddInfo(usedProcInfo);
 //             GetDocument()->GetTerminal().GetProcessorList()->removeProcessor(((CProcessor2*)pObjectDisplay)->GetProcessor(),false);
			}
		}
		//if(AfxMessageBox("Are you sure you want to delete all selected processors?", MB_YESNO) == IDYES)
		if(box->m_vInfoList.size()>0)
		{
			if(!box->IsNoInfo())
			{
				if(MessageBox("Delete processors,Are you sure?",NULL,MB_OKCANCEL)==IDOK)
				{
// 					for(int j=0;j<nSize;j++)
// 					{
// 						CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(j);
// 						if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
// 						{
// 							continue;
// 						}
// 
// 						Processor *pProc = ((CProcessor2*)pObjectDisplay)->GetProcessor();
//  						if (pProc)
//  							GetDocument()->GetTerminal().GetProcessorList()->removeProcessor(((CProcessor2*)pObjectDisplay)->GetProcessor(),false);
// 					}
					CObjectDisplayList vProcessors = GetDocument()->GetSelectProcessors();
					GetDocument()->DeleteProcessors(vProcessors);	
				}			
			}
			else
			{
				
				if(box->DoModal()==IDOK)	
				{
// 					for(int j=0;j<nSize;j++)
// 					{
// 						CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(j);
// 						if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
// 						{
// 							continue;
// 						}
// 
// 						Processor *pProc = ((CProcessor2*)pObjectDisplay)->GetProcessor();
// 						if (pProc)
// 							GetDocument()->GetTerminal().GetProcessorList()->addProcessor(((CProcessor2*)pObjectDisplay)->GetProcessor(),false);
// 					}
// 
// 					//	Fixed by Kevin
// 					CObjectDisplayList vProcessor = GetDocument()->GetSelectProcessors();
// 					GetDocument()->DeleteProcessors(vProcessor);	
				}
				else
				{
					for(int jj=0;jj<nSize;jj++)
					{
						CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(jj);
						if (pObjectDisplay == NULL || pObjectDisplay->GetType() != ObjectDisplayType_Processor2)
						{
							continue;
						}


						Processor *pProc = ((CProcessor2*)pObjectDisplay)->GetProcessor();
						GetDocument()->GetTerminal().GetProcessorList()->addProcessor(((CProcessor2*)pObjectDisplay)->GetProcessor(),false);
					}
				}
			}
		}
		else
		{
			CObjectDisplayList vProcessors = GetDocument()->GetSelectProcessors();
			GetDocument()->DeleteProcessors(vProcessors);	
		}
		
		delete box;
	}
}

void C3DView::OnCtxRotateShapes() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);

	m_eMouseState = NS3DViewCommon::_rotateshapes;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();

	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjectDisplay && pObjectDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2 *pProc2 = (CProcessor2 *)pObjectDisplay;

		GetDocument()->m_ptProcCmdPosStart = ARCVector3(pProc2->GetRotation(), 0.0, 0.0);
	}


	
	//GetDocument()->m_ptProcCmdPosStart = ARCVector3(GetDocument()->GetSelectProcessors()[0]->GetRotation(), 0.0, 0.0);
}

void C3DView::OnCtxDeletePipe()
{
	if( m_nSelectedPipe >= 0 )
	{
		CString strFlowName = _T("");
		CString strValue = _T("");
		CMobileElemConstraint* database = NULL;
		if( GetDocument()->GetTerminal().m_pPassengerFlowDB->IfUseThisPipe(m_nSelectedPipe,database))
		{
			database->screenPrint(strFlowName,0,255);
			strValue.Format(_T("Passenger flow: %s use this pipe, cannot delete it!"),strFlowName);
			AfxMessageBox(strValue,MB_OK);
			return;
		}

		if(GetDocument()->GetTerminal().m_pSubFlowList->IfUseThisPipe(m_nSelectedPipe,strFlowName))
		{
			strValue.Format(_T("Process flow: %s is using this pipe,cannot delete it"),strFlowName);
			AfxMessageBox(strValue,MB_OK);
			return;
		}
		//modify the pipe index after m_iCurListSel
		GetDocument()->GetTerminal().m_pPassengerFlowDB->deletePipe( m_nSelectedPipe ) ;
		GetDocument()->DeletePipe(m_nSelectedPipe);
		GetDocument()->GetTerminal().m_pPipeDataSet->DeletePipeAt( m_nSelectedPipe );

		//delete pipe from the congestionparam file
		CCongestionAreaManager& dataset = GetDocument()->getARCport()->getCongestionAreaManager();
		INTVECTOR* pPipes = dataset.GetPipesVect();
		int j = 0;
		for(j = 0; j < static_cast<int>(pPipes->size()); j++){
			if(m_nSelectedPipe == (*pPipes)[j]){
				pPipes->erase(pPipes->begin()+j);
				break;
			}
		}
		for(j = 0; j < static_cast<int>(pPipes->size()); j++){
			if(m_nSelectedPipe < (*pPipes)[j]){
				(*pPipes)[j]--;
			}
		}
		//end delete from file

		//save 
		for(int i=0; i<GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeCount(); i++)
		{
			if( GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeAt( i )->GetPipePointCount()<2 )
			{
				CString strMsg;
				strMsg = "The "+GetDocument()->GetTerminal().m_pPipeDataSet->GetPipeAt(i)->GetPipeName() +" pipe's point number less than 2!\r\n";
				MessageBox( strMsg );
				return;
			}
		}

		CString path	= GetDocument()->m_ProjInfo.path;
		GetDocument()->GetTerminal().m_pPipeDataSet->saveDataSet( path, FALSE );
	
		GetDocument()->GetTerminal().m_pPassengerFlowDB->saveDataSet( path, FALSE );
		
		CString strPath = GetDocument()->m_ProjInfo.path + "\\INPUT";
		GetDocument()->getARCport()->getCongestionAreaManager().saveDataSet(strPath, FALSE);
		Invalidate(FALSE);
	}
}

void C3DView::OnCtxMoveShapes() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	m_eMouseState = NS3DViewCommon::_moveshapes;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
}

void C3DView::OnViewPrint()
{
	CDC dc;
	CPrintDialog printDlg(FALSE);
	if(printDlg.DoModal() == IDCANCEL )
		return;
	dc.Attach( printDlg.GetPrinterDC() );
	dc.m_bPrinting = TRUE;
	CString sTitle;
	sTitle = "View";
	DOCINFO di;
	::ZeroMemory( &di, sizeof(DOCINFO) );
	di.cbSize = sizeof( DOCINFO );
	di.lpszDocName = sTitle;
	
	BOOL bPrintingOK = dc.StartDoc( &di );

	CPrintInfo info;
	info.m_rectDraw.SetRect( 0,0,dc.GetDeviceCaps( HORZRES ),dc.GetDeviceCaps( VERTRES ));
	
	OnBeginPrinting( &dc, &info );

	CRect rcClient;
	this->GetClientRect(&rcClient);
	int nWidth = rcClient.Width();
	int nHeight = rcClient.Height();
	nWidth -= (nWidth % 4);
	nHeight -= (nHeight % 4);
	BYTE* pBits = new BYTE[nWidth*nHeight*3];
	PBufferPointer Hpbuff = NULL;
	RenderOffScreen(Hpbuff, nWidth, nHeight, pBits);
	if(Hpbuff){ delete Hpbuff; Hpbuff = NULL;}
	else{
		AfxMessageBox("Failed Print View!");
	}

	//OverWrite Model, So donot need delete  it
	C24bitBMP::SaveBMP((LPCTSTR)(PROJMANAGER->GetAppPath() + _T("\\{253B8E24-B748-4bef-AB76-697C343D98D8}.bmp")), nWidth, nHeight, pBits);

	OnPrint( &dc, &info );
	bPrintingOK = TRUE;
	OnEndPrinting( &dc, &info );

	if( bPrintingOK )
	{
		dc.EndDoc();
	}
	else
		dc.AbortDoc();
	dc.Detach();

	delete [] pBits;
	//DeleteFile(PROJMANAGER->GetAppPath() + _T("\\{253B8E24-B748-4bef-AB76-697C343D98D8}.bmp"));
}

void C3DView::OnPrint(CDC *pDC, CPrintInfo *pInfo)
{
	HBITMAP hBitmap = (HBITMAP)::LoadImage(NULL, (LPCTSTR)(PROJMANAGER->GetAppPath() + _T("\\{253B8E24-B748-4bef-AB76-697C343D98D8}.bmp")),
		                                  IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	CBitmap bmp;
	bmp.Attach(hBitmap);

	CPalette palette;
	HANDLE hDIB = CAFTEMSBitmap::DDBToDIB(bmp, BI_RGB, &palette);

	LPBITMAPINFOHEADER lpbi;
	lpbi = (LPBITMAPINFOHEADER)hDIB;
	int nColors = lpbi->biClrUsed ? lpbi->biClrUsed : 1 << lpbi->biBitCount;
	BITMAPINFO &bmInfo = *(LPBITMAPINFO)hDIB;
	LPVOID lpDIBBits;
	if( bmInfo.bmiHeader.biBitCount > 8 )
		lpDIBBits = (LPVOID)((LPDWORD)(bmInfo.bmiColors + 
		bmInfo.bmiHeader.biClrUsed) + 
		((bmInfo.bmiHeader.biCompression == BI_BITFIELDS) ? 3 : 0));
	else
		lpDIBBits = (LPVOID)(bmInfo.bmiColors + nColors);

	BITMAP bm;
	bmp.GetBitmap( &bm );

	CRect r = pInfo->m_rectDraw;

	double dBMAspect = ((double)bm.bmWidth)/bm.bmHeight;
	double dPAspect = ((double)r.Width())/r.Height();

	HDC hDC = pDC->GetSafeHdc();
	if(dBMAspect > dPAspect) {
		//bm relatively wider
		StretchDIBits(hDC,0, 0, r.Width(), static_cast<int>(r.Width()/dBMAspect), 0, 0, 
			bm.bmWidth, bm.bmHeight, lpDIBBits,	&bmInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	else {
		StretchDIBits(hDC, 0, 0, static_cast<int>(r.Height()*dBMAspect), r.Height(), 0, 0, 
			bm.bmWidth, bm.bmHeight, lpDIBBits,	&bmInfo, DIB_RGB_COLORS, SRCCOPY);
	}
	
	bmp.DeleteObject();
}

void C3DView::OnViewExport()
{
	CRender3DView* pRender3DView = GetDocument()->GetRender3DView();
	if (pRender3DView) // do nothing when use new render engine
	{
		pRender3DView->OnViewExport();
		return;
	}

	//get file name
	CFileDialog fileDlg(FALSE, _T("bmp"), _T("*.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), NULL, 0, FALSE );
	if(fileDlg.DoModal() == IDOK) 
	{
		/*
		Invalidate(FALSE);
		UpdateWindow();
		//get bitmap of view & save bitmap to disk
		CAFTEMSBitmap::WriteWindowToDIB((LPCTSTR)fileDlg.GetPathName(), this);
		*/
		CRect rcClient;
		this->GetClientRect(&rcClient);
		int nWidth = rcClient.Width();
		int nHeight = rcClient.Height();
		nWidth -= (nWidth % 4);
		nHeight -= (nHeight % 4);
		BYTE* pBits = new BYTE[nWidth*nHeight*3];

		PBufferPointer Hpbuff = NULL;
		RenderOffScreen(Hpbuff, nWidth, nHeight, pBits);
		if(Hpbuff){ 
			delete Hpbuff; 
			Hpbuff = NULL;	
			C24bitBMP::SaveBMP((LPCTSTR)fileDlg.GetPathName(),nWidth,nHeight,pBits);
		}
		else {
			AfxMessageBox(_T("Failed Export View!"));
		}

		delete [] pBits;
	}
}


void C3DView::ShowDistanceDialog()
{
	CString msg;
	double dist = 0.0;
	int nPtCount=m_vMousePosList.size();
	for(int i=0; i<nPtCount-1; i++) {
		dist+=(m_vMousePosList[i+1]-m_vMousePosList[i]).Magnitude();
	}
	CTermPlanDoc * pDoc = GetDocument();
	if(pDoc && pDoc->m_systemMode == EnvMode_AirSide)
	{
		dist = CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit(),dist);
		msg.Format("Distance: %.2f %s", dist, CARCLengthUnit::GetLengthUnitString(pDoc->m_umGlobalUnitManager.GetCurSelLengthUnit()));
	}
	else
	{
		dist = UNITSMANAGER->ConvertLength(dist);
		msg.Format("Distance: %.2f %s", dist, UNITSMANAGER->GetLengthUnitString());
	}
	AfxMessageBox(msg, MB_OK|MB_ICONINFORMATION);
}

void C3DView::OnToggleRailPtEdit()
{
	m_bSelRailEdit = !m_bSelRailEdit;
	Invalidate(FALSE);
}

void C3DView::OnUpdateToggleRailPtEdit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
	if(m_bSelRailEdit) {
		pCmdUI->SetText("Disable Point Editing");
	}
	else {
		pCmdUI->SetText("Enable Point Editing");
	}
}

void C3DView::OnInsertRailEditPt()
{
	//find the point on the selected railway that is closest to the mouse position
	std::vector<RailwayInfo*> railWayVect;
	GetDocument()->m_trainTraffic.GetAllRailWayInfo(railWayVect);
	//railWayVect[m_nSelectedRailWay]->
	const Path* path = &(railWayVect[m_nSelectedRailWay]->GetRailWayPath());
	int numPts = path->getCount();
	ASSERT(numPts > 1);
	Point pt(m_ptMousePos[VX], m_ptMousePos[VY], path->getPoint(0).getZ());
	Point closestPt = path->getClosestPoint(pt);
	//3 cases
	//1) closestPt is 1st point, insert point after 1st
	//2) closestPt is last pt, insert point before last
	//3) closestPt is neither 1st nor last
	if(closestPt.preciseCompare(path->getPoint(0)))
		railWayVect[m_nSelectedRailWay]->InsertPoint(0,pt);
	else if(closestPt.preciseCompare(path->getPoint(numPts-1)))
		railWayVect[m_nSelectedRailWay]->InsertPoint(numPts-2,pt);
	else 
	{
		int cpIdx = -1;
		int i=0;
		for(; i<numPts; i++) 
		{
			if(closestPt.preciseCompare(path->getPoint(i)))
				break;
		}
		//ASSERT(i!=-1);
		//make 3 vectors
		ARCVector2 next(path->getPoint(i+1).getX()-closestPt.getX(),path->getPoint(i+1).getY()-closestPt.getY());
		ARCVector2 prev(path->getPoint(i-1).getX()-closestPt.getX(),path->getPoint(i-1).getY()-closestPt.getY());
		ARCVector2 test(pt.getX()-closestPt.getX(),pt.getY()-closestPt.getY());
		//if angle between test and next < test and prev then we insert after i, otherwise insert after i-1)
		if((test * next) > (test * prev)) {
			railWayVect[m_nSelectedRailWay]->InsertPoint(i,pt);
		}
		else {
			railWayVect[m_nSelectedRailWay]->InsertPoint(i-1,pt);
		}
	}

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("Insert Point;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	GetDocument()->GetTerminal().m_pAllCarSchedule->AdjustRailWay( &(GetDocument()->m_trainTraffic));
	GetDocument()->GetTerminal().pRailWay->saveDataSet( GetDocument()->m_ProjInfo.path, false );
	GetDocument()->GetTerminal().m_pAllCarSchedule->saveDataSet( GetDocument()->m_ProjInfo.path,false );

	Invalidate(FALSE);
}

void C3DView::OnDeleteRailEditPt()
{
	//delete point
	std::vector<RailwayInfo*> railWayVect;
	GetDocument()->m_trainTraffic.GetAllRailWayInfo(railWayVect);
	ASSERT(m_nSelectedRailWay >= 0 && m_nSelectedRailWay < static_cast<int>(railWayVect.size()));
	const Path* path = &(railWayVect[m_nSelectedRailWay]->GetRailWayPath());
	int numPts = path->getCount();
	ASSERT(numPts > 0 && m_nSelectedRailPt >= 0 && m_nSelectedRailPt < numPts);
	if(m_nSelectedRailPt > 0 && m_nSelectedRailPt < numPts-1) {
		if(IDYES == AfxMessageBox("Are you sure you want to delete the selected point?", MB_YESNO)) {
			railWayVect[m_nSelectedRailWay]->DeletePoint(m_nSelectedRailPt);
		}
	}
	else {
		AfxMessageBox("Cannot delete first or last railway point");
	}

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("Delete Point;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	GetDocument()->GetTerminal().m_pAllCarSchedule->AdjustRailWay( &(GetDocument()->m_trainTraffic));
	GetDocument()->GetTerminal().pRailWay->saveDataSet( GetDocument()->m_ProjInfo.path, false );
	GetDocument()->GetTerminal().m_pAllCarSchedule->saveDataSet( GetDocument()->m_ProjInfo.path,false );

	Invalidate(FALSE);
}


void C3DView::OnCtxGroup() 
{
	//CTermPlanDoc::PROC_GROUP *pNewPG=new CTermPlanDoc::PROC_GROUP;
	CObjectDisplayGroup * pObjPG = new CObjectDisplayGroup;

	int nCount = GetDocument()->GetSelectProcessorsCount();

	for ( int i = 0; i < nCount; ++i)
	{
		CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(i);
		pObjDisplay->SetGroup(pObjPG);
		pObjPG->AddItem(pObjDisplay);
	}

	//std::vector<CProcessor2*>::iterator	iter;

	//for(iter =  GetDocument()->GetSelectProcessors().begin(); iter !=  GetDocument()->GetSelectProcessors().end(); iter++)
	//{		
	//	(*iter)->SetGroup(pObjPG);
	//	pObjPG->AddItem(*iter);
	//		
	//}
	GetDocument()->GetTerminalPlacement()->AddGroup(pObjPG);
}



void C3DView::OnUpdateCtxGroup(CCmdUI* pCmdUI) 
{
	std::vector<CProcessor2*>::iterator	iter;
	 int nCount=GetDocument()->GetSelectProcessors().GetCount();
	 if(nCount<2)
	 {
		 pCmdUI->Enable(FALSE);
		 return;
	 }
	
	
	 for (int i =0; i < nCount; ++i)
	 {
		 if((GetDocument()->GetSelectedObjectDisplay(i))->GetGroup())
		 {
			 pCmdUI->Enable(FALSE);
			 return;
		 }
	 }


	//for(iter =  GetDocument()->GetSelectProcessors().begin(); iter !=  GetDocument()->GetSelectProcessors().end(); iter++)
	//{
	//	if((*iter)->GetGroup())
	//	{
	//		pCmdUI->Enable(FALSE);
	//		return;
	//	}
	//}


	pCmdUI->Enable(TRUE);
}

void C3DView::OnCtxUngroup() 
{
	int nCount=GetDocument()->GetSelectProcessors().GetCount();

	for (int i = 0; i < nCount; ++i)
	{
		CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(i);
		if(pObjDisplay->GetGroup())
		{

			CObjectDisplayGroup * pGroup = pObjDisplay->GetGroup();

			for(int i=0;i<pGroup->GetCount();++i)
			{
				CObjectDisplay * pObjDisplay = pGroup->GetItem(i);
				pObjDisplay->SetGroup(NULL);
			}

			GetDocument()->GetTerminalPlacement()->RemoveGroup(pGroup);		
			break;
		}
	}

	//std::vector<CProcessor2*>::iterator	iter;
	//for(iter =  GetDocument()->GetSelectProcessors().begin(); iter !=  GetDocument()->GetSelectProcessors().end(); iter++)
	//{
	//	if((*iter)->GetGroup())
	//	{

	//		CObjectDisplayGroup * pGroup = (*iter)->GetGroup();

	//		for(int i=0;i<pGroup->GetCount();++i)
	//		{
	//			CObjectDisplay * pObjDisplay = pGroup->GetItem(i);
	//			pObjDisplay->SetGroup(NULL);
	//		}

	//		GetDocument()->GetTerminalPlacement()->RemoveGroup(pGroup);		
	//		break;
	//	}

	//}
	
}

void C3DView::OnUpdateCtxUngroup(CCmdUI* pCmdUI) 
{


	int nCount=GetDocument()->GetSelectProcessors().GetCount();

	for (int i = 0; i < nCount; ++i)
	{
		if(GetDocument()->GetSelectedObjectDisplay(i)->GetGroup())
		{
			pCmdUI->Enable(TRUE);
			return;
		}

	}
	//std::vector<CProcessor2*>::iterator	iter;
	//for(iter =  GetDocument()->GetSelectProcessors().begin(); iter !=  GetDocument()->GetSelectProcessors().end(); iter++)
	//{
	//	if((*iter)->GetGroup())
	//	{
	//		pCmdUI->Enable(TRUE);
	//		return;
	//	}
	//	
	//		
	//}
	pCmdUI->Enable(FALSE);
	
}



void C3DView::OnCtxMoveshapeZ() 
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	m_eMouseState = NS3DViewCommon::_moveshapesZ;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	m_ptMousePos = Get3DMousePos(m_ptFrom);
	
}



void C3DView::OnCtxMoveshapeZ0() 
{
	GetDocument()->OnMoveShapeZ(0,0,TRUE);
	Invalidate(FALSE);
}

void C3DView::OnChangeShapesMenuList(UINT nID)
{
	int nIndex=nID-CHANGE_SHAPES_MENU_LIST_ID_BEGIN;
	CShape::CShapeList* pSL = SHAPESMANAGER->GetShapeList();
	ASSERT(pSL != NULL);

	CShape* pSelShape= pSL->at(nIndex);

	int nCount = GetDocument()->GetSelectProcessorsCount();
	for (int i =0; i < nCount; ++i)
	{
		CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(i);
		if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
		{
			CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;
			pProc2->SetShape(pSelShape);
		}
	}
	//std::vector<CProcessor2*>::iterator	iter;
	//for(iter =  GetDocument()->GetSelectProcessors().begin(); iter !=  GetDocument()->GetSelectProcessors().end(); iter++)
	//{
	//	(*iter)->SetShape(pSelShape);
	//}
	GetDocument()->UpdateAllViews(NULL);
	
}

void C3DView::InsertMenuToChangeShapes(CNewMenu* pCtxMenu)
{

}


void C3DView::OnSharpenView()
{
	if(!m_bAutoSharp) {
		GenerateSharpTexture();
		m_bAutoSharp = TRUE;
	}
	else {
		UseSharpTexture(FALSE);
		m_bAutoSharp = FALSE;
	}
	Invalidate(FALSE);
}

void C3DView::OnViewRadar()
{
	m_bShowRadar = !m_bShowRadar;
	Invalidate(FALSE);
}

void C3DView::OnTimer(UINT nIDEvent)
{
	OnUpdateAnimTime(NULL);

    if (!m_bDragSelect)
	{
		if(nIDEvent == m_nPanZoomTimeout) { //pan or zoom completed 1 second ago
			KillTimer(m_nPanZoomTimeout);
			m_bMovingCamera = FALSE;
			if(m_bAutoSharp) {
				GenerateSharpTexture();				
				Invalidate(FALSE);
			}

            return;
		}
		else if(nIDEvent == m_nOnSizeTimeout) {
			KillTimer(m_nOnSizeTimeout);
			m_bMovingCamera = FALSE;
			if(m_bAutoSharp) {
				GenerateSharpTexture();
				Invalidate(FALSE);
			}
			return;
		}
		else if(nIDEvent == m_nMouseMoveTimeout) {
			//call OnMouseHover
			DWORD dwPos = ::GetMessagePos();
			CPoint p((int) LOWORD(dwPos), (int) HIWORD(dwPos));
			this->ScreenToClient(&p);
			// TRACE("Mouse Hover: (%d, %d)\n", p.x, p.y);
			OnMouseHover(p);
			KillTimer(m_nMouseMoveTimeout);
			m_bMovingCamera = FALSE;
			//// TRACE("KillTimer\n");
		}
		else if(nIDEvent == m_nRedSquareBlinkTimer)
		{
			if (GetDocument()->m_eAnimState == CTermPlanDoc::anim_none
				&& ( GetDocument()->GetSelectProcessors().GetCount()>0 || GetParentFrame()->m_vSelected.size() > 0 ) )
				Draw();
			//KillTimer(m_nRedSquareBlinkTimer);
		}
	}
	CView::OnTimer(nIDEvent);
}

void C3DView::OnCtxFlipProcessors()
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	ClearPointPickStruct();
	m_eMouseState = NS3DViewCommon::_getmirrorline;
}

void C3DView::OnCtxReflectProcessors()
{
	//copy selected processors and flip
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	GetDocument()->CopySelectedProcessors(); //copies selected processors and selects the copies
	OnCtxFlipProcessors();
}

void C3DView::OnCtxAlignProcessors()
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	m_vMousePosList.clear();
	m_eMouseState = NS3DViewCommon::_getalignline;
}

void C3DView::OnUpdateAlign(CCmdUI* pCmdUI)
{
	std::vector<CProcessor2*>::iterator	iter;
	int nCount=GetDocument()->GetSelectProcessors().GetCount();
	if(nCount<2)
	{
		pCmdUI->Enable(FALSE);
		return;
	}


	for (int i =0; i < nCount; ++i)
	{
		if((GetDocument()->GetSelectedObjectDisplay(i))->GetGroup())
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}

	pCmdUI->Enable(TRUE);
}

void C3DView::OnCtxArrayProcessors()
{
	ASSERT(GetDocument()->GetSelectProcessors().GetCount() > 0);
	ClearPointPickStruct();
	m_eMouseState = NS3DViewCommon::_getarrayline;
}

void C3DView::GetMobileElmentOnDifferentFloorZ(const MobEventStruct& pes, double* dAlt,double& z)
{
	
}

void C3DView::DrawPax(CTermPlanDoc* pDoc,BOOL* bOn,double* dAlt,BOOL bSelectMode)
{	
	AnimationData* pAnimData = &(pDoc->m_animData);
	long nTime = pAnimData->nLastAnimTime;



	try
	{
		if(pDoc->m_eAnimState==CTermPlanDoc::anim_pause) {
			if(m_bAnimDirectionChanged) {
				pDoc->GetTerminal().m_pLogBufManager->InitBuffer();
				m_bAnimDirectionChanged = FALSE;
			}
			pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, (CEventLogBufManager::ANIMATION_MODE)m_nAnimDirection,pDoc );
		}
		else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playF) {
			pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_FORWARD,pDoc );
		}
		else if(pDoc->m_eAnimState==CTermPlanDoc::anim_playB) {
			pDoc->GetTerminal().m_pLogBufManager->loadDataIfNecessary( nTime, CEventLogBufManager::ANIMATION_BACKWARD,pDoc );
		}
	}
	catch(...){
		//AfxMessageBox("NULL");
		return;
	}
	// load data if necessary
	

	CPaxDispProps& paxdispprops = pDoc->m_paxDispProps;
	std::vector<int>& paxdisppropidx = pDoc->m_vPaxDispPropIdx;
	MobLogEntry element;
	PaxLog* pPaxLog = pDoc->GetTerminal().paxLog;
	const PaxEventBuffer& logData = pDoc->GetTerminal().m_pLogBufManager->getLogDataBuffer();
	
	
	AnimationPaxGeometryData geoData;
	PaxAnimationData animaPaxData;
	for( PaxEventBuffer::const_iterator it = logData.begin(); it!= logData.end(); ++it )
	{		
	
		CPaxDispPropItem* pPDPI = paxdispprops.GetDispPropItem(paxdisppropidx[it->first]);
		if(!pPDPI || !pPDPI->IsVisible())
			continue;

		pPaxLog->getItem(element, it->first);	
		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();
		bool bInTimeRange = (nStartTime <= nTime && nEndTime >= nTime);
		if(!bInTimeRange)
			continue;

		const part_event_list& part_list = it->second;				

		int pesAIdx = 0;
		//draw pax
		if(!CAnimationManager::GetPaxShow(pDoc,part_list,nTime,bOn,dAlt,geoData,animaPaxData,pesAIdx))
			continue;

		const ARCVector3& pos = geoData.pos;
		bool bInCamera = GetCamera()->PointInCamera(pos[VX],pos[VY],pos[VZ]);
		if(!bInCamera)
			continue;

		double distToCam =  GetCamera()->m_vLocation.DistanceTo( ARCVector3(pos[VX],pos[VY],pos[VZ])) ;
		if(distToCam>2000000)//can not see it so far
			continue;
			

		bool bReveseShapeColor = false; // 
		if( !bSelectMode&&m_nPaxSeledID==it->first)
		{
			if(m_nTickTotal/8%2==0)
			{
				bReveseShapeColor = true;				
			}			
			m_nTickTotal++;
			if(m_nTickTotal>=2E15)
				m_nTickTotal=0;
		}		

			
		if(bSelectMode){
			glLoadName(it->first);
		}
		if(!bReveseShapeColor)
		{
			glColor4ubv( ARCColor4(pPDPI->GetColor()) );
		}
		else{
			COLORREF clr=RGB(255-GetRValue(pPDPI->GetColor()),255-GetGValue(pPDPI->GetColor()),255-GetBValue(pPDPI->GetColor()));
			glColor4ubv( ARCColor4(clr) );
		}


		if(distToCam>40000){
			glBegin(GL_POINTS);
			glVertex3d(pos[VX],pos[VY],pos[VZ]+10); //draw head as a point
			glEnd();			
		}
		else{
			//draw full detail
			GLuint nDLID;// = GetParentFrame()->PaxShapeDLIDs()[0] ;	
			int dlIndex = paxdisppropidx[it->first] ;
			if(dlIndex== -1)
				nDLID = GetParentFrame()->DefaultPaxShapeDLID();
			else if(dlIndex == -2)
				nDLID = GetParentFrame()->OverlapPaxShapeDLID();
			else
			{
				if(dlIndex>=0 && dlIndex < static_cast<int>(GetParentFrame()->PaxShapeDLIDs().size()) )
					nDLID = GetParentFrame()->PaxShapeDLIDs()[dlIndex];
				else
				{
					ASSERT(FALSE);
					continue;
				}
			}
			
			//update select pax
			if(bReveseShapeColor){
				COLORREF clr=RGB(255-GetRValue(pPDPI->GetColor()),255-GetGValue(pPDPI->GetColor()),255-GetBValue(pPDPI->GetColor()));
				GLuint unTemp=SHAPESMANAGER->GenerateShapeDisplayList(pPDPI->GetShape().first,pPDPI->GetShape().second,clr);
				DrawPAXShape(pos[VX],pos[VY],pos[VZ], geoData.dAngleFromX, unTemp );				
				if(glIsList(unTemp))
					glDeleteLists(unTemp,1);
			}	
			else
			{				
					
				if(geoData.bNeedRotate)	{
					ARCVector4 quat;
					quat.x = element.getAngle().x;
					quat.y = element.getAngle().y;
					quat.z = element.getAngle().z;
					quat.w = element.getAngle().w;
					DrawPAXShape(pos[VX],pos[VY],pos[VZ],geoData.dAngleFromX, nDLID,quat);	
				}
				else{
					DrawPAXShape(pos[VX],pos[VY],pos[VZ],geoData.dAngleFromX, nDLID);				
				}
			}			
		}

		

	}

	CHECK_GL_ERRORS("C3DView::DrawPax(...). end");
}


void C3DView::DrawFlight(const DistanceUnit& airportAlt,BOOL bSelectMode,BOOL bShowTag , const double* mvmatrix , const double* projmatrix, const int* viewport)
{
	glFrontFace(GL_CCW);
	// (CTermPlanDoc*)Document* m_pDocument;
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
//#if 0	
	CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	CAircraftDispProps& aircraftdispprops = pDoc->m_aircraftDispProps;
	CAircraftTags& aircrafttags = pDoc->m_aircraftTags;

	std::vector< std::pair<int,int> >& aircraftdisppropidx = pDoc->m_vACDispPropIdx;
	std::vector<int>& aircraftshapeidx = pDoc->m_vACShapeIdx;
	std::vector< std::pair<int,int> >& aircrafttagpropidx = pDoc->m_vACTagPropIdx;



	AirsideFlightLogEntry element;
	element.SetEventLog(&(airsideSimLogs.m_airsideFlightEventLog));

	AirsideFlightLog& airsideFlightLog = airsideSimLogs.m_airsideFlightLog;
//	int nCount = airsideFlightLog.getCount();

	const CAirsideEventLogBuffer<AirsideFlightEventStruct> &airsideFlightEventLog = GetDocument()->GetOutputAirside()->GetLogBuffer()->m_flightLog;
	size_t nCount = airsideFlightLog.getCount();//airsideFlightEventLog.m_lstElementEvent.size();

	for (size_t i = 0; i < nCount; i++)
	{
		const CAirsideEventLogBuffer<AirsideFlightEventStruct>::ELEMENTEVENT& ItemEventList = airsideFlightEventLog.m_lstElementEvent[i];
		airsideFlightLog.getItem(element, ItemEventList.first);

		// the entry time and exit time of every flight
		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();

		// test if if's the time for stop in airport
		if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
		{
			ARCVector3 pos;
			ARCVector3 dangles;
			int pesAIdx =-1;
			double dSpeed =0;
			if( CAnimationManager::GetFlightShow(ItemEventList,nCurTime,airportAlt
													,pos,dangles,dSpeed,pesAIdx)
				)
			{
				const AirsideFlightEventStruct& pesA = ItemEventList.second[pesAIdx];
				CAircraftDispPropItem* pADPI = NULL; 
				CAircraftTagItem* pATI =NULL;
				if(pesA.state == 'A')
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].first);		
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].first);
				}
				else
				{
					pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i].second);
					pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i].second);
				}

				if( !(pADPI && pADPI->IsVisible()) )
					continue;
				
				// Get the color and scale size of one flight
				ARCColor4 col(pADPI->GetColor());
				double scale = pADPI->GetScaleSize();
				if(pos[VZ] < 10000 ) scale = fabs(1 +  (scale-1) * (pos[VZ]) / 10000); 
				if (scale < 1)
				{
					scale = 1.0;
				}
				
				ARCColor4 final_color = col;

				// set the color 
				glColor4ubv(final_color);
		
				// set the Frutum View, and test if it is in the Viewport
				if( GetCamera()->SphereInCamera( Point(pos[VX],pos[VY],pos[VZ]),10000*scale) ){	
					if(bSelectMode)
					{
						glLoadName(i+SEL_FLIGHT_OFFSET);
					}
	
					if (!bSelectMode&& m_nPaxSeledID==i+SEL_FLIGHT_OFFSET)
					{
						if(m_nTickTotal/8%2==0)
						{
						//	glColor4ub(255-col[RED],255-col[GREEN],255-col[BLUE],col[ALPHA]);
							final_color = ARCColor4(255-col[RED],255-col[GREEN],255-col[BLUE],col[ALPHA]);
						}

						m_nTickTotal++;
						if(m_nTickTotal>=2E15)
							m_nTickTotal=0;
					
					}					
				
					//draw flight
					glEnable(GL_LIGHTING);
					glPushMatrix();
					glTranslated(pos[VX],pos[VY],pos[VZ]);	
					glRotated(dangles[VZ],0,0,1);
					glRotated(dangles[VY],0,1,0);
					glRotated(dangles[VX],1,0,0);	
					glScaled(scale,scale,scale);	
					CglShapeResource * pShapeRes = GetParentFrame()->GetAirsideShapeResource();
					FlightShape * pflightshape = (FlightShape*)pShapeRes->GetShape( CString(element.GetAirsideDesc().acType) );
					if (!pflightshape)
					{
						CString strshapeName = ACALIASMANAGER->getOglShapeName( CString(element.GetAirsideDesc().acType));
						pflightshape = (FlightShape*)pShapeRes->GetShape( strshapeName );
					}
					if(pflightshape)
					{
						if(pADPI->IsLogoOn())
						{							
							CString airlinecode = element.GetAirsideDesc()._airline.GetValue();
							CTexture * pairlineTex  =  GetParentFrame()->GetAirlineTextures()->getTexture(airlinecode);
							if(!pairlineTex)
								pairlineTex =  GetParentFrame()->GetAirlineTextures()->getTexture("01");
							pflightshape->setAirlineTexture(pairlineTex);
						}
						else
						{
							pflightshape->setAirlineTexture(NULL);
						}
						// Draw the flight
						//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	
						DistanceUnit& dlen = element.GetAirsideDesc().dLength;
						DistanceUnit& dWid = element.GetAirsideDesc().dWingSpan;
						glScaled( dlen,dWid,(dlen+dWid)*0.5 );
						pflightshape->display();
					}
					else
					{
						//draw paper flight							
						GLuint nDLID = GetParentFrame()->DefaultACShapeDLID();
						DistanceUnit& dlen = element.GetAirsideDesc().dLength;
						DrawPAXShape(0, 0, 400,1.0f, dlen/4000.0, nDLID);
					}	
					glPopMatrix();
					//end draw flight				

					//CAircraftDispPropItem* pADPI = aircraftdispprops.GetDispPropItem(aircraftdisppropidx[i]);
					//CAircraftTagItem* pATI = aircrafttags.GetTagItem(aircrafttagpropidx[i]);
					bool bDrawTag = bShowTag && mvmatrix && projmatrix && viewport && pADPI && pADPI->IsVisible() && pATI && pATI->IsShowTags();	
					
					if( bDrawTag )
					{						
						glDisable(GL_LIGHTING);
						glDisable(GL_DEPTH_TEST);
						const AirsideFlightDescStruct& fds  = element.GetAirsideDesc();
						DrawFlightTag(fds,pesA,dSpeed,pos[VZ]+airportAlt, *pATI, pos + ARCVector3(0,0,fds.dHeight*0.5),mvmatrix,projmatrix,viewport);
						glEnable(GL_DEPTH_TEST);
					}		

				}				

//---------------------------------------------------------------------------------				
				CHECK_GL_ERRORS("flight display");
					

				//DrawPAX(0, 0, 0,1 ,0, nDLID);
				//glPopMatrix();
				/*glDisable(GL_POLYGON_OFFSET_FILL);*/

				//// TRACE("FlightEvent --- x: %f - y: %f\n", xpos, ypos);
			}

		}
	}
	



}

void C3DView::OnAnimPax()
{
	m_bAnimPax=!m_bAnimPax;
	ClearSeledPaxList();		
}

void C3DView::OnUpdateAnimPax(CCmdUI* pCmdUI)
{
	CTermPlanDoc* pDoc=GetDocument();
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	if(pDoc->m_eAnimState != CTermPlanDoc::anim_none
		&&pDoc->m_bMobileElementDisplay) 
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetCheck(m_bAnimPax);
		pMF->GetAnimPaxCB()->EnableWindow(m_bAnimPax);
		
		
	}
	else
	{
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(FALSE);
		pMF->GetAnimPaxCB()->EnableWindow(FALSE);
	}
}


void C3DView::ClearSeledPaxList()
{
	if(m_bAnimPax==FALSE)
	{
		m_nPaxSeledID=-1;
		m_nPaxSeledIDList.clear();
		m_nPaxSeledTagList.clear();
		//Clear Selected Pax List
		CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
		pMF->GetAnimPaxCB()->ResetContent();
	}
	
}

void C3DView::InitSeledPaxList()
{
	CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
	pMF->GetAnimPaxCB()->ResetContent();
	
	std::vector<UINT>::iterator iter;
	std::vector<CString>::iterator iterTag;
	CString strPaxSeled;
	
	for(iter=m_nPaxSeledIDList.begin();iter!=m_nPaxSeledIDList.end();iter++)
	{
		if(iter==m_nPaxSeledIDList.begin())
			m_nPaxSeledID=*iter;
		iterTag=m_nPaxSeledTagList.begin()+(iter-m_nPaxSeledIDList.begin());
		strPaxSeled=*iterTag;
							
		int nIndex=pMF->GetAnimPaxCB()->InsertString(0,strPaxSeled);
		pMF->GetAnimPaxCB()->SetItemData(nIndex,*iter);
		
	}
	if(pMF->GetAnimPaxCB()->GetCount()>0)
		pMF->GetAnimPaxCB()->SetCurSel(0);
}

void C3DView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if(bActivate)
	{
		if(m_nViewIDCur!=m_nViewID)
			InitSeledPaxList();
		m_nViewIDCur=m_nViewID;
		
		GetParentFrame()->OnUpdateFrameTitle(TRUE);
	}
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}



void C3DView::SelectProc(int& nSelectCount,UINT nFlags,CPoint point,GLuint* idx,CTermPlanDoc* pDoc,int procType,CSize sizeSelection,BOOL bDragSelect)
{
	nSelectCount=SelectScene(nFlags, point.x, point.y, idx, sizeSelection, bDragSelect, procType);

//unselect structures
	
	for(int i=0;i< nSelectCount;i++) 
	{
		//Added by Tim In 4/15/2003--------------------------------------------------------------------------------------------------------------------------------
		if(pDoc->m_eAnimState != CTermPlanDoc::anim_none
			&&pDoc->m_bMobileElementDisplay) 
		{

			// Select Flight blinking
			//if(m_nFlightSelect!=-1&&m_nFlightSelect==idx[0])
			//{			
			//		m_nFlightSelect=-1;
			//}
			//else
			//{
			//	if(m_nFlightSelect!=idx[0]&&idx[0]>=SEL_FLIGHT_OFFSET&&idx[0]<SEL_VEHICLE_OFFSET)
			//		m_nFlightSelect=idx[0];
			//}

			//// Select Vehicle blinking
			//if(m_nVehicleSelect!=-1&&m_nVehicleSelect==idx[0])
			//{	
			//		m_nVehicleSelect=-1;
			//}
			//else
			//{
			//	if(m_nVehicleSelect!=idx[0]&&idx[0]>=SEL_VEHICLE_OFFSET)
			//		m_nVehicleSelect=idx[0];
			//}

//			if(m_nPaxSeledID!=idx[0])
			{
				m_nPaxSeledID=idx[0];
				std::vector<UINT>::iterator itFind=std::find(m_nPaxSeledIDList.begin(),m_nPaxSeledIDList.end(),m_nPaxSeledID);
				CString strPaxSeled;
				CMainFrame* pMF=(CMainFrame*)AfxGetMainWnd();
				if(itFind==m_nPaxSeledIDList.end())
				{
					strPaxSeled=pDoc->GetPaxTagFromID(m_nPaxSeledID);
					int nIndexCB=pMF->GetAnimPaxCB()->InsertString(0,strPaxSeled);	
					m_nPaxSeledIDList.insert(m_nPaxSeledIDList.begin(),m_nPaxSeledID);
					m_nPaxSeledTagList.insert(m_nPaxSeledTagList.begin(),strPaxSeled);
					pMF->GetAnimPaxCB()->SetItemData(nIndexCB,m_nPaxSeledID);
					pMF->GetAnimPaxCB()->SetCurSel(0);
				}
				else
				{
					int nOffsetIndex=itFind-m_nPaxSeledIDList.begin();
					m_nPaxSeledIDList.erase(itFind);
					std::vector<CString>::iterator itFindTag=m_nPaxSeledTagList.begin()+nOffsetIndex;
					m_nPaxSeledTagList.erase(itFindTag);

					pMF->GetAnimPaxCB()->GetLBText(nOffsetIndex,strPaxSeled);
					DWORD nItemDate=pMF->GetAnimPaxCB()->GetItemData(nOffsetIndex);
					pMF->GetAnimPaxCB()->DeleteString(nOffsetIndex);

					int nIndexCB=pMF->GetAnimPaxCB()->InsertString(0,strPaxSeled);	
					m_nPaxSeledIDList.insert(m_nPaxSeledIDList.begin(),m_nPaxSeledID);
					m_nPaxSeledTagList.insert(m_nPaxSeledTagList.begin(),strPaxSeled);
					pMF->GetAnimPaxCB()->SetItemData(nIndexCB,m_nPaxSeledID);
					pMF->GetAnimPaxCB()->SetCurSel(0);	
				}

				//if (m_nPaxSeledID>=SEL_VEHICLE_OFFSET)
				//{
				//	nTempSeledID=m_nPaxSeledID-SEL_VEHICLE_OFFSET;
				//}
				//else if(m_nPaxSeledID>=SEL_FLIGHT_OFFSET&&m_nPaxSeledID<SEL_VEHICLE_OFFSET)
				//{
				//	nTempSeledID=m_nPaxSeledID-SEL_FLIGHT_OFFSET;
				//}
				//else
				//{
				//	nTempSeledID = m_nPaxSeledID;
				//}

				GetDocument()->OnSelectPAX(m_nPaxSeledID, strPaxSeled);

			}
			break;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		m_nLastSelIdx = idx[i];

		BYTE seltype = GetSelectType(idx[i]);
		BYTE selsubtype = GetSelectSubType(idx[i]);

		if(SELTYPE_PROCESSOR == seltype ) 
		{
			//if ctrl is down:
			//click on unselected proc adds to selection
			//clck on selected proc does nothing (until mouse up)
			//if ctrl is not down:
			//click on unselected or selected proc starts new selection
			
			// TRACE("left-click on processor idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);

			m_bMovingProc = TRUE;
			m_bProcHasMoved = FALSE;
			if(!GetDocument()->IsJustAftCopyProc())
			{
				if(bDragSelect&&nSelectCount>1)
					GetDocument()->DoLButtonDownOnProc(GetSelectIdx(idx[i]), TRUE);
				else
					GetDocument()->DoLButtonDownOnProc(GetSelectIdx(idx[i]), nFlags&MK_CONTROL);
			}
			
			//break;
		}
		else if(SELTYPE_RAILWAY == seltype)
		{
			if(SELSUBTYPE_POINT == selsubtype) {
				// TRACE("left-click on railway point idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);
				//railway point...  mark point as selected and enable point moving
				//we assume that the pt belongs only to the currently rail
				m_nSelectedRailPt = GetSelectIdx(idx[i]);
				//get the real altitude of selectRailwayPoint;				
				m_bMovingRailPoint = TRUE;
				break;
			}
		}
		else if(SELTYPE_PIPE == seltype)
		{
			
			if(SELSUBTYPE_POINT == selsubtype) 
			{
				// TRACE("left-click on pipe point idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);
				//pipe point...  mark point as selected and enable point moving
				//we assume that the pt belongs only to the currently pipe
				m_nSelectedPipePt = GetSelectIdx(idx[i]);

				m_bMovingPipePoint = TRUE;
				m_bMovingProc = FALSE;
				break;
			}
			else if(SELSUBTYPE_WIDTHPOINT == selsubtype) {
				// TRACE("left-click on pipe width point idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);
				//pipe width point...  mark point as selected and enable point width moving
				//we assume that the pt belongs only to the currently pipe
				m_nSelectedPipeWidthPt = GetSelectIdx(idx[i]);

				m_bMovingPipeWidthPoint = TRUE;
				m_bMovingProc = FALSE;
				break;
			}
			else if(SELSUBTYPE_MAIN == selsubtype)
			{  
				m_bMovingProc = TRUE;
				m_bProcHasMoved = FALSE;
				//add to the selection list
				int nSelectPipe = GetSelectIdx(idx[i]);
				CPipeDisplay* pPipeDisplay = GetDocument()->GetTerminalPlacement()->GetPipeDislplay( GetDocument()->GetTerminal().m_pPipeDataSet, nSelectPipe);
				if(pPipeDisplay)
				{
					if(bDragSelect&&nSelectCount>1)
					{
						GetDocument()->SelectObject(pPipeDisplay,TRUE);					
					}
					else
					{

						GetDocument()->SelectObject(pPipeDisplay,nFlags&MK_CONTROL);
					}
	

					//GetDocument()->GetSelectProcessors().AddItem(pPipeDisplay);
				}

			}//2005-9-1
		}
		else if(SELTYPE_TERMINALAREA == seltype)
		{
			if(SELSUBTYPE_MAIN == selsubtype)
			{
				m_bMovingProc = TRUE;
				m_bProcHasMoved = FALSE;
				int nSelectArea = GetSelectIdx(idx[i]);
				CObjectDisplay* pObjectDisplay = 
					GetDocument()->GetTerminalPlacement()->GetAreaDisplay( GetDocument()->GetTerminal().m_pAreas, nSelectArea);

				if(pObjectDisplay)
				{
					if(bDragSelect&&nSelectCount>1)
					{
						GetDocument()->SelectObject(pObjectDisplay,TRUE);
					}
					else
					{
						GetDocument()->SelectObject(pObjectDisplay,nFlags&MK_CONTROL);

					}

					//GetDocument()->GetSelectProcessors().AddItem(pPipeDisplay);
				}

			}
		}
		else if(SELTYPE_TERMINALPORTAL == seltype)
		{
			if(SELSUBTYPE_MAIN == selsubtype)
			{
				m_bMovingProc = TRUE;
				m_bProcHasMoved = FALSE;
				int nSelectPortal = GetSelectIdx(idx[i]);
				CObjectDisplay* pObjectDisplay = 
					GetDocument()->GetTerminalPlacement()->GetPortalDisplay( &(GetDocument()->m_portals) , nSelectPortal);

				if(pObjectDisplay)
				{
					if(bDragSelect&&nSelectCount>1)
					{
						GetDocument()->SelectObject(pObjectDisplay,TRUE);
					}
					else
					{
					GetDocument()->SelectObject(pObjectDisplay,nFlags&MK_CONTROL);
					}


					//GetDocument()->GetSelectProcessors().AddItem(pPipeDisplay);
				}
			}

		}
		else if(SELTYPE_TERMINALTRACER == seltype || SELTYPE_AIRSIDETRACER == seltype || SELTYPE_LANDSIDETRACER == seltype)
		{
			if(m_pTracerTagWnd) {
				m_pTracerTagWnd->DestroyWindow();
				delete m_pTracerTagWnd;
			}
			
			CTracerTagWnd::TracerType eTracerType = CTracerTagWnd::Terminal;
			if(SELTYPE_AIRSIDETRACER == seltype)
				eTracerType = CTracerTagWnd::Airside;
			else if(SELTYPE_LANDSIDETRACER == seltype)
				eTracerType = CTracerTagWnd::Landside;

			m_pTracerTagWnd = new CTracerTagWnd(eTracerType, GetSelectIdx(idx[i]), GetDocument());
			m_pTracerTagWnd->CreateEx(WS_EX_TOOLWINDOW, NULL, NULL, WS_CHILD, CRect(0,0,250,200), this, IDD_DIALOG_PROCESSORTAGS );
			CRect rc, rcClient, rcMain;
			this->GetWindowRect(&rcClient);
			AfxGetMainWnd()->GetWindowRect(&rcMain);
			m_pTracerTagWnd->GetWindowRect(&rc);

			CPoint ptClient(m_ptFrom);

			if( ptClient.y > rc.Height() ) {
				ptClient.y -= rc.Height();
			}
			if( ptClient.x > (rcClient.Width()-rc.Width()) ) {
				ptClient.x -= rc.Width();
			}

			// TRACE("left-click on track idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), ptClient.x, ptClient.y);

			m_pTracerTagWnd->MoveWindow(ptClient.x, ptClient.y, rc.Width(), rc.Height());
			m_pTracerTagWnd->ShowWindow(SW_SHOW);
			break;
		}
		else if(SELTYPE_STRUCTURE == seltype)
		{
			if(SELSUBTYPE_MAIN == selsubtype)
			{
				// TRACE("left-click on structure idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);
				m_bMovingStructure=TRUE;
				m_nSelectedStructure=GetSelectIdx(idx[i]);

				CObjectDisplay* pObjectDisplay = 
					GetDocument()->GetTerminalPlacement()->GetStructureDisplay( GetDocument()->GetTerminal().m_pStructureList, m_nSelectedStructure);
				
				if(pObjectDisplay)
				{
					if(bDragSelect&&nSelectCount>1)
					{
						GetDocument()->SelectObject(pObjectDisplay,TRUE);					
					}
					else
					{
						GetDocument()->SelectObject(pObjectDisplay,nFlags&MK_CONTROL);
					}

					//GetDocument()->GetSelectProcessors().AddItem(pPipeDisplay);
				}
				
			}
			else if(SELSUBTYPE_POINT == selsubtype) 
			{
				// TRACE("left-click on structure point idx: %d, pt=(%d,%d)\n", GetSelectIdx(idx[i]), point.x, point.y);
				m_nSelectedStructurePoint = GetSelectIdx(idx[i]);
				m_bMovingStructurePoint=TRUE;
				//break;
			}
		}
		
		else if(SELTYPE_WALLSHAPE==seltype)
		{
			if( SELSUBTYPE_MAIN == selsubtype)
			{
				m_nSelectWallShape=GetSelectIdx(idx[i]) ;
				m_bMovingSelectWallShape=TRUE;

				CObjectDisplay* pObjectDisplay = 
					GetDocument()->GetTerminalPlacement()->GetWallShapeDisplay( GetDocument()->GetTerminal().m_pWallShapeList, m_nSelectWallShape);

				if(pObjectDisplay)
				{
					if(bDragSelect&&nSelectCount>1)
					{
						GetDocument()->SelectObject(pObjectDisplay,TRUE);
					}
					else
					{
						GetDocument()->SelectObject(pObjectDisplay,nFlags&MK_CONTROL);
					}

					//GetDocument()->GetSelectProcessors().AddItem(pPipeDisplay);
				}
			}
			else if(SELSUBTYPE_POINT ==selsubtype)
			{
				//get the select point belong to 
				WallShapeList & walllist=pDoc->GetCurWallShapeList();
				size_t nwallCount=walllist.getShapeNum();
				int ptCount=0;
				for(size_t j=0;j<nwallCount;++j){
					WallShape* wall=walllist.getShapeAt(j);
					int wallPtCount=wall->GetPointCount();
					if(  GetSelectIdx(idx[i])<ptCount+wallPtCount && GetSelectIdx(idx[i])>=ptCount ){
						m_nSelectWallShape=j ;
						m_nSelectWallShapePoint=GetSelectIdx(idx[i])-ptCount;
						m_bMovingSelectWallShapePoint=TRUE;
						break;
					}						
					ptCount+=wallPtCount;
				}				
			}
		}
		else if (SELTYPE_HOLDSHORTLINE==seltype)
		{
			if (SELSUBTYPE_MAIN==selsubtype)
			{
				m_nSelectHoldshortLine = GetSelectIdx(idx[i]);
				m_bMovingSelectHoldshortLine = TRUE;
			}
		}
		/*else if(SELTYPE_LANDSIDEPROCESSOR ==seltype ) 
		{
			LandsideDocument * pdoc = GetDocument()->GetLandsideDoc();
			pdoc->SelectProcessor(pdoc->GetPlacementPtr()->m_vDefined[ NewGetSelectIdx(idx[i]) ] ) ;
			m_bMovingProc = TRUE;
			if(selsubtype == SELSUBTYPE_POINT )
			{
				GetDocument()->GetLandsideView()->m_nCtrlPointsId = NewGetSubSelectIdx(idx[i]);
				GetDocument()->GetLandsideView()->m_bMovingCtrlPoint = true;
				m_bMovingProc = FALSE;
			}
		}*/
		else if(SELTYPE_AIRROUTE == seltype)
		{
			m_nSelectedAirRout = GetSelectIdx(idx[i]);
			Invalidate(FALSE);
		}
	}


	GetDocument()->SetJustAftCopyProc(false);


}

void C3DView::BeginDragSelect(UINT nFlags,CTermPlanDoc* pDoc,BOOL& bDragSelect,CPoint& point,CSize& sizeSelection)
{
	if(nFlags&MK_SHIFT&&pDoc->m_eAnimState == CTermPlanDoc::anim_none)
	{
		ReleaseCapture();
		CRectTracker rt;
		rt.m_sizeMin=CSize(2,2);
		if(bDragSelect=rt.TrackRubberBand(this,point))
		{
			CRect trueRect;
			rt.GetTrueRect(trueRect);
			trueRect.NormalizeRect();
			point=trueRect.CenterPoint();
			if(trueRect.Size().cx>rt.m_sizeMin.cx&&trueRect.Size().cy>rt.m_sizeMin.cy)
			{
				trueRect.NormalizeRect();
				point=trueRect.CenterPoint();
				GetDocument()->UnSelectAllProcs();
				//sizeSelection=CSize(trueRect.Size().cx/2,trueRect.Size().cy/2);
				sizeSelection=trueRect.Size();
			}
		}
	}
}

void C3DView::OnCopyToClipboard() 
{
	COleDataSource* pSource=SaveForCopy();
	if(pSource)
	{
		pSource->SetClipboard();
		m_eMouseState=NS3DViewCommon::_pasteproc;
		((CMainFrame*)AfxGetMainWnd())->m_bCanPasteProcess = TRUE;
	}
}

void C3DView::OnCutToClipboard() 
{
	
}

COleDataSource* C3DView::SaveForCopy()
{
	COleDataSource* pSource=new COleDataSource;
	
	//CProc2DataForClipBoard PDFCB(GetDocument()->GetSelectProcessors());
	//CString strCopyed=PDFCB.ToString(m_ptPickForCopy);
	//
	//char* pStrPlacement=strCopyed.GetBuffer(strCopyed.GetLength());
	//
	//int nDataSize=sizeof(char)*(strlen(pStrPlacement)+1);
	//HGLOBAL hData=::GlobalAlloc(GMEM_SHARE,nDataSize);
	//LPVOID pData=::GlobalLock(hData);
	//ASSERT(pData!=NULL);
	//memcpy(pData,pStrPlacement,nDataSize);
	//::GlobalUnlock(hData);
	//pSource->CacheGlobalData(CF_TEXT,hData);
	//strCopyed.ReleaseBuffer();

	return pSource;
}

void C3DView::OnPasteToFloor(UINT nID)
{
	m_nPasteToFloorIndex=nID-ID_PASTE_TO_FLOOR1;
	m_eMouseState=NS3DViewCommon::_pasteproc;
}

void C3DView::LoadFromPaste(COleDataObject *pDataObject,ARCVector3& arcVector)
{
	if(!pDataObject->IsDataAvailable(CF_TEXT))
	{
		return;
	}
	HGLOBAL hStrPlacement=pDataObject->GetGlobalData(CF_TEXT);
	ASSERT(hStrPlacement!=NULL);
	LPVOID lpStrPlacement=::GlobalLock(hStrPlacement);
	ASSERT(lpStrPlacement!=NULL);
	char* pStr=(char*)lpStrPlacement;
	CString str=pStr;
	CPDFCFormat PDFCFormat(str);
	if(!PDFCFormat.Parse(GetDocument()->m_nActiveFloor*SCALE_FACTOR,arcVector))
	{
		m_eMouseState = NS3DViewCommon::_default;
		((CMainFrame*)AfxGetMainWnd())->m_bCanPasteProcess = FALSE;
		AfxMessageBox("Data Format is bad.");
	}
	else
	{
		int nCount=PDFCFormat.m_vpPDFC.size();	
		for(int i=0;i<nCount;i++)
		{
			GetDocument()->PasteProc(GetDocument()->m_nActiveFloor, PDFCFormat.m_vpPDFC[i]);
		}
	}
}

void C3DView::OnUpdatePasteToFloor(CCmdUI* pCmdUI)
{
	CTermPlanDoc* pDoc=GetDocument();
	int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
	BOOL bOn[MAX_FLOOR];
	
	int i=0; 
	for(;i<nFloorCount; i++)
	{
		bOn[i] = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->IsVisible();
	}
	//
	bOn[i] = pDoc->GetFloorByMode(EnvMode_AirSide).m_vFloors[0]->IsVisible();

	switch(pCmdUI->m_nID)
	{
	case ID_PASTE_TO_FLOOR1:
		pCmdUI->Enable(bOn[0]);
		break;
	case ID_PASTE_TO_FLOOR2:
		pCmdUI->Enable(bOn[1]);
		break;
	case ID_PASTE_TO_FLOOR3:
		pCmdUI->Enable(bOn[2]);
		break;
	case ID_PASTE_TO_FLOOR4:
		pCmdUI->Enable(bOn[3]);
		break;
	}
}

//void C3DView::ShowMovieDialog()
//{
//
//	if(m_pDlgMovie) {
//		m_pDlgMovie->DestroyWindow();
//		delete m_pDlgMovie;
//		m_pDlgMovie = NULL;
//	}
//	m_pDlgMovie = new CDlgMovie( GetDocument(), this, this );
//	m_pDlgMovie->Create( CDlgMovie::IDD, this );
//	
//	m_pDlgMovie->ShowWindow( SW_SHOW );
//}

LRESULT C3DView::OnMsgDestroyDlgWalkthrough(WPARAM wParam, LPARAM lParam)
{
	GetDocument()->m_pModelessDlgMan->DestroyDialogWalkThrough();//->OnMsgDestroyDlgWalkthrough(wParam, lParam);
	return 1;
}

void C3DView::OnPickconveyorAdddestination() 
{
	Pickconveyor(0);
}

void C3DView::OnPickconveyorInsertafter() 
{
	Pickconveyor(1);
		
}

void C3DView::OnPickconveyorInsertbefore() 
{
	Pickconveyor(2);
}

void C3DView::Pickconveyor(int nInsertType)
{
	if(!m_pPickConveyorTree)
		m_pPickConveyorTree=new CPickConveyorTree;
	if(m_pPickConveyorTree->m_hWnd==NULL)
		m_pPickConveyorTree->Create(NULL,this);
	CPoint point=GetMessagePos();

	int nCount=GetDocument()->GetSelectProcessors().GetCount();
	
	if(nCount <= 0)
		return;

	CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if(pObjDisplay == NULL || pObjDisplay->GetType() != ObjectDisplayType_Processor2)
		return;


	CProcessor2* pProc2=(CProcessor2 *)pObjDisplay;
	m_pPickConveyorTree->Init(pProc2,nInsertType);
	m_pPickConveyorTree->SetWindowPos(NULL,point.x,point.y,180,240,SWP_NOZORDER|SWP_SHOWWINDOW);
	
}

void C3DView::OnEditUndo()
{
	if(!GetDocument()->m_CommandHistory.Undo())
		TRACE0("nothing to undo\n");
	Invalidate(FALSE);
}

void C3DView::OnEditRedo()
{
	if(!GetDocument()->m_CommandHistory.Redo())
		TRACE0("nothing to redo\n");
	Invalidate(FALSE);
}

void C3DView::OnAltObjectEditFinished()
{
	if(m_eMouseState == NS3DViewCommon::_rotateairsideobject)
	{
		ALTObject3DList selectObject;
		GetParentFrame()->m_vSelected.GetSelectObjects(selectObject);
		for(int i=0;i<(int)selectObject.size();i++)
		{
			ALTObject3D * pObject = selectObject.at(i).get();
			ALTObject3DList vObject3Ds; 
			vObject3Ds.push_back(pObject);
			GetAirside3D()->ReflectChangeOf(vObject3Ds);
			pObject->FlushChange();
		}
		ALTObject3DList altobject3dlist;
		(GetParentFrame()->m_vSelected).GetSelectObjects(altobject3dlist);
		GetDocument()->m_CommandHistory.Add(new RotateALTObjectCommand(altobject3dlist, GetDocument(), m_dRotate));
	}
}

void C3DView::OnProcEditFinished()
{
	if(GetDocument()->GetSelectProcessors().GetCount() <= 0)
		return;
	CObjectDisplay *pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjDisplay == NULL || pObjDisplay->GetType() != ObjectDisplayType_Processor2)
	{
		GetDocument()->OnProcEditFinished();
		return;
	}
	CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;

	if(m_eMouseState == NS3DViewCommon::_scaleproc || 
		m_eMouseState == NS3DViewCommon::_scaleprocX || 
		m_eMouseState == NS3DViewCommon::_scaleprocY || 
		m_eMouseState == NS3DViewCommon::_scaleprocZ) 
	{
		const ARCVector3 vDelta = pProc2->GetScale() - GetDocument()->m_ptProcCmdPosStart;
		GetDocument()->m_CommandHistory.Add(new ScaleProcessorsCommand(GetDocument()->GetSelectProcessors(), GetDocument(), vDelta[VX], vDelta[VY], vDelta[VZ]));

		const ARCVector3& vTo = pProc2->GetScale();
		const ARCVector3& vFrom = GetDocument()->m_ptProcCmdPosStart;
		// TRACE("Scale Proc from (%.2f, %.2f) to (%.2f, %.2f)\n", vFrom[VX], vFrom[VY], vTo[VX], vTo[VY]);
	}
	else if(m_eMouseState == NS3DViewCommon::_rotateproc ) 
	{
		double delta = pProc2->GetRotation() - GetDocument()->m_ptProcCmdPosStart[VX];
		GetDocument()->m_CommandHistory.Add(new RotateProcessorsCommand(GetDocument()->GetSelectProcessors(), GetDocument(), delta));
	}
	else if(m_eMouseState == NS3DViewCommon::_rotateshapes )
	{
		double delta = pProc2->GetRotation() - GetDocument()->m_ptProcCmdPosStart[VX];
		GetDocument()->m_CommandHistory.Add(new RotateShapesCommand(GetDocument()->GetSelectProcessors(), GetDocument(), delta));
	}
	//else if(m_eMouseState == _rotateairsideobject)
	//{
	//	//	
	//	ALTObject3DList selectObject;
	//	GetParentFrame()->m_vSelected.GetSelectObjects(selectObject);
	//	for(int i=0;i<(int)selectObject.size();i++)
	//	{
	//		ALTObject3D * pObject = selectObject.at(i).get();
	//		ALTObject3DList vObject3Ds; 
	//		vObject3Ds.push_back(pObject);
	//		GetAirside3D()->ReflectChangeOf(vObject3Ds);
	//		pObject->FlushChange();
	//	}
	//	//SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin();
	//	//const ARCPoint3& vTo = (*itr)->GetLocation();
	//	//double delta = vTo[VX] - m_ptAltObjectCmdPosStart[VX];
	//	ALTObject3DList altobject3dlist;
	//	(GetParentFrame()->m_vSelected).GetSelectObjects(altobject3dlist);
	//	GetDocument()->m_CommandHistory.Add(new RotateALTObjectCommand(altobject3dlist, GetDocument(), m_dRotate));
	//}
		
	GetDocument()->OnProcEditFinished();
}

void C3DView::OnClearClipboard()
{
		m_eMouseState=NS3DViewCommon::_default;
		((CMainFrame*)AfxGetMainWnd())->m_bCanPasteProcess = FALSE;//invalidate the paste menu
}

void C3DView::OnProcessPaste() 
{
	// TODO: Add your command handler code here
	CPoint point = m_ptRMouseDown;
	COleDataObject dataObject;
	VERIFY(dataObject.AttachClipboard());
	//paste the process to the new position where the right mouse click
//	ARCVector3 arcVector=Get3DMousePos(point,FALSE,GetDocument()->m_nActiveFloor);
	//paste the process to the origin-position
	ARCVector3 arcVector = ARCVector3(0,0,0);
	//LoadFromPaste(&dataObject, arcVector);  
//	m_eMouseState=NS3DViewCommon::_default;
//	((CMainFrame*)AfxGetMainWnd())->m_bCanPasteProcess = FALSE;//invalidate the paste menu
	Invalidate(FALSE);

	m_bTrackLButton = FALSE;
}


void C3DView::OnCopyProcessors(UINT nID)
{
	UINT nMenuID = nID - MENU_FLOOR_ID;
	CTermPlanDoc* pDoc = NULL;
	int nFloor = -1;
	if (m_cpmi.GetRegularInfoByMenuID(nMenuID, &pDoc, nFloor))
		GetDocument()->CopyProcessors(pDoc, nFloor);
	else
		AfxMessageBox(_T("Could not find item"));
}

void C3DView::OnChooseMultipleSelectItem(UINT nID)
{
	int i = nID - MENU_MULTIPLESELECT_ID;

	TRACE("Selected item %d [%d]\n", i, m_vMultiSelectIDs[i]);

	BYTE seltype = GetSelectType(m_vMultiSelectIDs[i]);
	BYTE selsubtype = GetSelectSubType(m_vMultiSelectIDs[i]);
	unsigned short idx = GetSelectIdx(m_vMultiSelectIDs[i]);

	if(SELTYPE_PROCESSOR == seltype ) 
	{
		//if ctrl is down:
		//click on unselected proc adds to selection
		//clck on selected proc does nothing (until mouse up)
		//if ctrl is not down:
		//click on unselected or selected proc starts new selection
		GetDocument()->DoLButtonDownOnProc(idx, ::GetAsyncKeyState(VK_CONTROL)<0);
		m_bMovingProc = TRUE;
		m_bProcHasMoved = FALSE;
	}
	else if(SELTYPE_RAILWAY == seltype)
	{
		if(SELSUBTYPE_POINT == selsubtype) {
			//railway point...  mark point as selected and enable point moving
			//we assume that the pt belongs only to the currently rail
			m_nSelectedRailPt = idx;
			m_bMovingRailPoint = TRUE;
		}
	}
	else if(SELTYPE_PIPE == seltype)
	{
		if(SELSUBTYPE_POINT == selsubtype) {
			//pipe point...  mark point as selected and enable point moving
			//we assume that the pt belongs only to the currently pipe
			m_nSelectedPipePt = idx;
			m_bMovingPipePoint = TRUE;
		}
		else if(SELSUBTYPE_WIDTHPOINT == selsubtype) {
			//pipe width point...  mark point as selected and enable point width moving
			//we assume that the pt belongs only to the currently pipe
			m_nSelectedPipeWidthPt = idx;
			m_bMovingPipeWidthPoint = TRUE;
		}//2005-9-1
	}
	else if(SELTYPE_STRUCTURE == seltype)
	{
		if(SELSUBTYPE_MAIN == selsubtype)
		{
			//structure do something 
			m_bMovingStructure=TRUE;
			m_nSelectedStructure=idx;
		}
		else if(SELSUBTYPE_POINT == selsubtype)
		{
			m_nSelectedStructurePoint = idx;
			m_bMovingStructurePoint=TRUE;	
		}
	}
	else if(SELTYPE_TERMINALTRACER == seltype || SELTYPE_AIRSIDETRACER == seltype || SELTYPE_LANDSIDETRACER == seltype)
	{
		if(m_pTracerTagWnd) {
			m_pTracerTagWnd->DestroyWindow();
			delete m_pTracerTagWnd;
		}

		CTracerTagWnd::TracerType eTracerType = CTracerTagWnd::Terminal;
		if(SELTYPE_AIRSIDETRACER == seltype)
			eTracerType = CTracerTagWnd::Airside;
		else if(SELTYPE_LANDSIDETRACER == seltype)
			eTracerType = CTracerTagWnd::Landside;

		m_pTracerTagWnd = new CTracerTagWnd(eTracerType, idx, GetDocument());
		m_pTracerTagWnd->CreateEx(WS_EX_TOOLWINDOW, NULL, NULL, WS_CHILD, CRect(0,0,250,200), this, IDD_DIALOG_PROCESSORTAGS );
		CRect rc, rcClient, rcMain;
		this->GetWindowRect(&rcClient);
		AfxGetMainWnd()->GetWindowRect(&rcMain);
		m_pTracerTagWnd->GetWindowRect(&rc);

		CPoint ptClient(m_ptFrom);

		if( ptClient.y > rc.Height() ) {
			ptClient.y -= rc.Height();
		}
		if( ptClient.x > (rcClient.Width()-rc.Width()) ) {
			ptClient.x -= rc.Width();
		}

		// TRACE("left-click on track idx: %d, pt=(%d,%d)\n", idx, ptClient.x, ptClient.y);

		m_pTracerTagWnd->MoveWindow(ptClient.x, ptClient.y, rc.Width(), rc.Height());
		m_pTracerTagWnd->ShowWindow(SW_SHOW);
	}

	else if(SELTYPE_WALLSHAPE ==seltype){
		//if()
	}
	else if ( SELTYPE_LANDSIDEPROCESSOR==seltype )
	{
		m_bMovingProc = TRUE;
	}
}

void C3DView::OnCtxTogglestructureptedit()
{
	// TODO: Add your command handler code here
	m_bSelectStructureEdit=!m_bSelectStructureEdit;
	Invalidate(FALSE);
}

void C3DView::OnUpdateCtxTogglestructureptedit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	if(m_bSelectStructureEdit) {
		pCmdUI->SetText("Disable Point Editing");
	}
	else {
		pCmdUI->SetText("Enable Point Editing");
	}
}

void C3DView::OnCtxTogglepipeptedit()
{
	// TODO: Add your command handler code here
	m_bSelPipeEdit = !m_bSelPipeEdit;
	Invalidate(FALSE);
}

void C3DView::OnUpdateCtxTogglepipeptedit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(TRUE);
	if(m_bSelPipeEdit) {
		pCmdUI->SetText("Disable Point Editing");
	}
	else {
		pCmdUI->SetText("Enable Point Editing");
	}
}

void C3DView::OnSubmenuRemovesurface()
{
	// TODO: Add your command handler code here
	CStructureList *pCSlist=NULL;
	pCSlist=& GetDocument()->GetCurStructurelist();
	if(!( m_nSelectedStructure<(int)pCSlist->getStructureNum()) )return;

	CStructure* pStucture = pCSlist->getStructureAt(m_nSelectedStructure);
	CTVNode* pSurNode = GetDocument()->m_msManager.GetSurfaceAreasNode();
	CStructureID id = pStucture->getID();
	std::stack<CString>vList;
	while(id.idLength())
	{
		vList.push(id.GetLeafName());
		id.clearLevel(id.idLength() - 1);
	}

	CString sName(_T(""));
	CTVNode* pNode = pSurNode;
	while(vList.size())
	{
		sName = vList.top();
		vList.pop();

		if (pNode)
		{
			pNode = (CTVNode*)pNode->GetChildByName(sName);
		}
	}

	CTVNode* pTmpNode = NULL;
	while (pSurNode != pNode)
	{
		if (pNode->GetChildCount() == 0)
		{
			pTmpNode = pNode;
			pNode = (CTVNode*)pNode->Parent();
			pNode->RemoveChild(pTmpNode);
		}
		else
		{
			break;
		}
	}
	
	GetDocument()->DeleteStructueObject(m_nSelectedStructure);
	pCSlist->removeStructure(m_nSelectedStructure);
	GetDocument()->GetCurStructurelist().saveDataSet(GetDocument()->m_ProjInfo.path, false);

	GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pSurNode);

	//update NodeView will be add later 
	
}
void C3DView::OnSubmenuProprerties()
{
	// TODO: Add your command handler code here
	//display SurfaceArea properties and save the change;
	
	CStructureList *pCSlist=NULL;
	pCSlist=& GetDocument()->GetCurStructurelist();
	if(!( m_nSelectedStructure<(int)pCSlist->getStructureNum()) )return;
	CStructure * cs=pCSlist->getStructureAt(m_nSelectedStructure);
	ProcessorID id;
	id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
	id.setID(cs->getID().GetIDString());
	CDlgStructureProp dlg(cs,this);
	
	if (IDOK == dlg.DoFakeModal()){
		cs->setTexture(dlg.getText());
		cs->setAreaPath(dlg.getServiceLocation());		

		CTVNode* pSurfaceNode = GetDocument()->m_msManager.GetSurfaceAreasNode();

		std::stack<CString>vList;
		while(id.idLength())
		{
			vList.push(id.GetLeafName());
			id.clearLevel(id.idLength() - 1);
		}

		CString sName(_T(""));
		CTVNode* pNode = pSurfaceNode;
		while(vList.size())
		{
			sName = vList.top();
			vList.pop();

			if (pNode)
			{
				pNode = (CTVNode*)pNode->GetChildByName(sName);
			}
		}

		ProcessorID idNew;
		idNew.SetStrDict(GetDocument()->GetTerminal().inStrDict);
		idNew.setID(cs->getID().GetIDString());
		GetDocument()->GetNodeView()->RefreshProcessorItem(pNode,pSurfaceNode,idNew);
		GetDocument()->UpdateStructureChange(false);
	}
}

void C3DView::OnColormodeVivid()
{
	// TODO: Add your command handler code here
	setColorMode(_vivid);
}

void C3DView::OnColormodeDesaturated()
{
	// TODO: Add your command handler code here
	setColorMode(_de_saturated);
}

void C3DView::OnClose()
{

	CView::OnClose();
}



void C3DView::OnTaxiwayPointEnableEdit()
{
	// TODO: Add your command handler code here

	int nCount=GetDocument()->GetSelectProcessors().GetCount();
	if (nCount <= 0)
		return;	

	CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
		TaxiwayProc * taxiway=(TaxiwayProc *)pProc2->GetProcessor();
		taxiway->setEditMode(!taxiway->EditMode());
		Invalidate(FALSE);
	}

}

void C3DView::OnUpdateTaxiwayEnable(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here

	int nCount=GetDocument()->GetSelectProcessors().GetCount();
	if (nCount <= 0)
		return;	

	CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
		if(pProc2==NULL)
			return;
		TaxiwayProc * taxiway=(TaxiwayProc *)pProc2->GetProcessor();
		pCmdUI->Enable(TRUE);
		if(taxiway->EditMode()) 
		{
			pCmdUI->SetText("Disable Point Editing");
		}
		else 
		{
			pCmdUI->SetText("Enable Point Editing");
		}
	}
}

void C3DView::OnTaxiwayAddpointhere()
{
	// TODO: Add your command handler code here
	//get the close line to the select point


	int nCount=GetDocument()->GetSelectProcessors().GetCount();
	if (nCount <= 0)
		return;	

	CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);

	if (pObjDisplay && pObjDisplay->GetType() == ObjectDisplayType_Processor2)
	{
		CProcessor2* pProc2 = (CProcessor2 *)pObjDisplay;
		if(pProc2==NULL)
			return;
		TaxiwayProc * taxiway=(TaxiwayProc *)pProc2->GetProcessor();
		if(!taxiway->EditMode())return;
		Point mousePoint;
		mousePoint.setPoint(m_ptMousePos[VX], m_ptMousePos[VY],0.0);

		//add point to the path;
		taxiway->AddNewPoint(mousePoint);
		//save data
		CString sPath = GetDocument()->m_ProjInfo.path;

		GetDocument()->GetTerminal().GetAirsideProcessorList()->saveDataSet(sPath, true);
		Invalidate(FALSE);
	}


}

void C3DView::OnTaxiwayDeletepoint()
{
	// TODO: Add your command handler code here
	TaxiwayProc * m_pSelectTaxiway=NULL;
	CPROCESSOR2LIST* vProcList = &(GetDocument()->GetCurrentPlacement(EnvMode_AirSide)->m_vDefined);
	int nTaxiwayCount=0;
	for(int j=0; j<static_cast<int>(vProcList->size()); j++) 
	{
		CProcessor2* pProc2 = vProcList->at(j);
		if(pProc2->GetProcessor()->getProcessorType()==TaxiwayProcessor)
		{
			if(nTaxiwayCount==m_nSelectedTaxiway)
			{
				m_pSelectTaxiway=(TaxiwayProc *)pProc2->GetProcessor();
				break;
			}
			nTaxiwayCount++;
		}
	}					
	if(m_pSelectTaxiway ==NULL)return;
	//dont remove point if count of points below 2
	if(m_pSelectTaxiway->serviceLocationPath()->getCount()<3)return;
	m_pSelectTaxiway->removePoint(m_nSelectedTaxiwayPoint);
}


void C3DView::OnWallshapePointedit()
{
	// TODO: Add your command handler code here
	if(m_nSelectWallShape>=0)
	{
		WallShape* wall=GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
		wall->SetEditMode(! wall->IsEditMode());
	}
	Invalidate(FALSE);

}

void C3DView::OnUpdateWallshapePointedit(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if(m_nSelectWallShape>=0)
	{	
		WallShape * wall=GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
		pCmdUI->Enable(TRUE);
		if(wall->IsEditMode())
		{
			pCmdUI->SetText("Disable Point Editing");
		}
		else
		{
			pCmdUI->SetText("Enable Point Editing");
		}
	}
}

void C3DView::OnWallshapemenuAddpointhere()
{
	// TODO: Add your command handler code here
	if(m_nSelectWallShape>=0)
		GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape)->AddPointByPosition(m_ptMousePos[VX], m_ptMousePos[VY],0.0);
	Invalidate(FALSE);
}

void C3DView::OnWallshapepointmenuDelete()
{
	// TODO: Add your command handler code here
	GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape)->DelPoint(m_nSelectWallShapePoint);
	Invalidate(FALSE);
}

void C3DView::OnWallshapeDelete()
{
	// TODO: Add your command handler code here
	if(m_nSelectWallShape>=0)
	{
		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
		{
			CTVNode* pWallNode = GetDocument()->m_msManager.FindNodeByName(strProcTypeName);
			if (pWallNode == NULL)
				return;
			
			WallShape* pWallShape = GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
			CStructureID id = pWallShape->getID();
			CTVNode* pTempNode = NULL;
			std::stack<CString>vList;
			while(id.idLength())
			{
				vList.push(id.GetLeafName());
				id.clearLevel(id.idLength() - 1);
			}

			CString sName(_T(""));
			CTVNode* pNode = pWallNode;
			while(vList.size())
			{
				sName = vList.top();
				vList.pop();

				if (pNode)
				{
					pNode = (CTVNode*)pNode->GetChildByName(sName);
				}
			}

			while (pWallNode != pNode)
			{
				if (pNode->GetChildCount() == 0)
				{
					pTempNode = pNode;
					pNode = (CTVNode*)pNode->Parent();
					pNode->RemoveChild(pTempNode);
				}
				else
				{
					break;
				}
			}
			GetDocument()->DeleteWallShapeObject(m_nSelectWallShape);
			GetDocument()->GetCurWallShapeList().removeShape(m_nSelectWallShape);
			GetDocument()->GetCurWallShapeList().saveDataSet(GetDocument()->m_ProjInfo.path, false);

			GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pWallNode);
			Invalidate(FALSE);
		}
	}
}

void C3DView::OnWallshapemenuWallshapeproperty()
{
	// TODO: Add your command handler code here
	if(m_nSelectWallShape>=0)
	{	
		WallShape* wallsh=GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
		ProcessorID id;
		id.SetStrDict(GetDocument()->GetTerminal().inStrDict);
		id.setID(wallsh->getID().GetIDString());

		DlgWallShapeProp dlg(wallsh,this);

		if (IDOK == dlg.DoFakeModal()){
			wallsh->SetPath(dlg.m_Wallpath);
		
			CString strProcTypeName;
			if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
			{
				CTVNode* pWallNode = GetDocument()->m_msManager.FindNodeByName(strProcTypeName);
				if (pWallNode == NULL)
					return;

				std::stack<CString>vList;
				while(id.idLength())
				{
					vList.push(id.GetLeafName());
					id.clearLevel(id.idLength() - 1);
				}

				CString sName(_T(""));
				CTVNode* pNode = pWallNode;
				while(vList.size())
				{
					sName = vList.top();
					vList.pop();

					if (pNode)
					{
						pNode = (CTVNode*)pNode->GetChildByName(sName);
					}
				}

				ProcessorID idNew;
				idNew.SetStrDict(GetDocument()->GetTerminal().inStrDict);
				idNew.setID(wallsh->getID().GetIDString());
				GetDocument()->GetNodeView()->RefreshProcessorItem(pNode,pWallNode,idNew);
			}
		}
		Invalidate(FALSE);
	}
	
}

void C3DView::OnCtxWallshapeDispProperties()
{
	if(m_nSelectWallShape>=0){	
		WallShape* pWallShape = GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
		CDlgWallDisplayProperties dlg(pWallShape, this);

		if(IDOK == dlg.DoModal())
			GetDocument()->GetCurWallShapeList().saveDataSet(GetDocument()->m_ProjInfo.path, FALSE);
	
		Invalidate(FALSE);
	}
}


//moving airside drawing here in progress...

void C3DView::OnViewClose()
{
	CChildFrame *pChildFrm = GetDocument()->Get3DViewParent();
	if(pChildFrm->GetActiveView() == this)
		pChildFrm->DestroyWindow();
}
void C3DView::setSelectWallShape(WallShape * pShape){
	WallShapeList * pWalllist =&( GetDocument()->GetCurWallShapeList());
	size_t i;
	for( i=0; i<pWalllist->getShapeNum(); i++){
		if(pShape == pWalllist->getShapeAt(i) )break;
	}
	if( i<pWalllist->getShapeNum() )
		m_nSelectWallShape = i;
	else m_nSelectWallShape =-1;
}
void C3DView::setSelectStructure(CStructure * pStruct){
	CStructureList * pStructlist =& (GetDocument()->GetCurStructurelist());
	size_t i;
	for(  i=0;i<pStructlist->getStructureNum();i++)
	{
		if (pStruct == pStructlist->getStructureAt(i) )break;
	}
	if( i<pStructlist->getStructureNum() )
		m_nSelectedStructure = i;
	else m_nSelectedStructure =-1;
}

void C3DView::OnAircraftdisplayToggleontag()
{
	// TODO: Add your command handler code here
	if(m_nSelectedAircraft>=0 && m_nSelectedAircraft<(int) GetDocument()->m_vACDispSwitchs.size() ){
		AircraftDisplaySwitch & dispswitch = GetDocument()->m_vACDispSwitchs[m_nSelectedAircraft];
		//dispswitch.m_bUseGrpSet = false;
		dispswitch.m_bTagOn = !dispswitch.m_bTagOn;
	}
	Invalidate(FALSE);
}

void C3DView::OnUpdateAircraftdisplayToggleontag(CCmdUI *pCmdUI)
{
	if(m_nSelectedAircraft>=0 && m_nSelectedAircraft<(int) GetDocument()->m_vACDispSwitchs.size() ){
		AircraftDisplaySwitch & dispswitch = GetDocument()->m_vACDispSwitchs[m_nSelectedAircraft];
		pCmdUI->Enable(TRUE);
		if(dispswitch.m_bTagOn) {
			pCmdUI->SetText("Toggle Off Tag");
		}
		else {
			pCmdUI->SetText("Toggle On Tag");
		}
	}
}

//void C3DView::OnAirrouteDisplayproperties()
//{
//	// TODO: Add your command handler code here
//	ns_AirsideInput::AirRoute * pAirRoute = & GetDocument()->GetAirsieInput().GetAirRoutes().GetAirRoutes()[m_nSelectedAirRout];
//	
//	CDlgAirRouteDisplayProperties dlg(&pAirRoute->getDispProperties(), this);
//
//	if(IDOK == dlg.DoModal()){}
//		
//	Invalidate(FALSE);
//}


const static int iVehicleTypeCnt = 3;
const static CString strVehicleType[] = {
	"truck","car","car1"
} ;
const static ARCColor3 colorTable[iVehicleTypeCnt] = { ARCColor3(0,0,255), ARCColor3(0,255,0), ARCColor3(255,0,0), };


void C3DView::DrawCars(BOOL bSelectMode){
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	//#if 0	
	//Landside_Output::LandsideSimLogs& landsideSimLogs = pDoc->GetLandsideSimLogs();
	//long nCurTime = pDoc->m_animData.nLastAnimTime;
	//
	////glColor3ub(0,0,255);
	//glPolygonMode(GL_FRONT,GL_FILL);
	//const Landside_Output::VehicleList& vehiclelist = landsideSimLogs.GetVehicleList();	
	//int nCount = vehiclelist.size();

	//for (int i = 0; i < nCount; i++)
	//{
	//	Landside_Output::Vehicle * pVehicle = vehiclelist.at(i);
	//	int nVehicleType = abs(pVehicle->GetType()) % iVehicleTypeCnt;
	//	glColor3ubv(colorTable[nVehicleType]);

	//	const Landside_Output::VehicleEventList & eventlist = pVehicle->GetEventList(); 
	//	long nStartTime = (long)eventlist.front().time;
	//	long nEndTime = (long)eventlist.back().time;

	//	if (nStartTime <=  nCurTime && nEndTime >= nCurTime) 
	//	{
	//		long nEventCount =(long) eventlist.size();
	//			
	//		long nextIdx = -1;
	//		for (long j = 0; j < nEventCount; j++)
	//		{
	//			if (eventlist[j].time >= nCurTime )
	//			{
	//				nextIdx = j;
	//				break;
	//			}
	//		}
	//		if (nextIdx > 0)
	//		{
	//			const LandsideVehicleEventStruct& pesB = eventlist[nextIdx];
	//			const LandsideVehicleEventStruct& pesA = eventlist[nextIdx-1];
	//		//	//

	//		//	// calculate the current location
	//			int w = pesB.time - pesA.time;
	//			int d = pesB.time - nCurTime;
	//			float r = ((float) d) / w;
	//			float xpos = (1.0f - r) * pesB.x + r * pesA.x;
	//			float ypos = (1.0f - r) * pesB.y + r * pesA.y;

	//		
	//			float zpos = ((1.0f - r)*pesB.z + r*pesA.z);

	//			
	//			
	//			if( GetCamera()->PointInCamera(xpos, ypos, zpos) ){	
	//				ARCVector3 dir =ARCVector3(pesB.x,pesB.y,pesB.z) - ARCVector3(pesA.x ,pesA.y,pesA.z) ;				
	//				dir.Normalize();
	//				ARCVector2 dir2d(dir[VX],dir[VY]);dir2d.Normalize();
	//				double dangleY = ARCMath::RadiansToDegrees(acos(dir2d[VY]));
	//				if(dir2d[VX]>0)dangleY = -dangleY;
	//				double dangleZ = ARCMath::RadiansToDegrees(asin(dir[VZ]));
	//				if(pesA.bpush)dangleY += 180;
	//				glPushMatrix();
	//				glTranslatef(xpos,ypos,zpos);
	//				//glutSolidSphere(500,12,12);
	//				
	//				glRotated(dangleY,0,0,1);glRotated(dangleZ,1,0,0);
	//				glScaled(0.7,0.7,0.7);
	//				//GetShapeResource()->
	//				CglShape* pShape =  GetShapeResource()->GetShape(strVehicleType[nVehicleType]);
	//				if(pShape) pShape->display();
	//				glPopMatrix();
	//			}
	//			

	//	}
	//}

	//}
}

CglShapeResource* C3DView::GetShapeResource( void )
{
	return GetParentFrame()->GetShapeResource();
}
CTextureResource * C3DView::getTextureResource(){
	return GetParentFrame()->GetTextureResoure();
}


void C3DView::RenderRadar()
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);	//get the current viewport
	//render radar
	glDisable(GL_DEPTH_TEST);	//disable depth test
	glViewport(viewport[2]-110,viewport[3]-110,100,100);// make a small viewport on upper right

	//get the extents of the active floor's map
	ARCVector2 vExtMin, vExtMax; 
	GetDocument()->GetActiveFloor()->GetExtents(&vExtMin, &vExtMax);		
	double dSize = 0.0;
	if(fabs(vExtMax[VX]-vExtMin[VX]) > fabs(vExtMax[VY]-vExtMin[VY]))
		//x > y
		dSize = 0.5*(vExtMax[VX] - vExtMin[VX])*GetDocument()->GetActiveFloor()->MapScale();
	else
		//y > x
		dSize = 0.5*(vExtMax[VY] - vExtMin[VY])*GetDocument()->GetActiveFloor()->MapScale();

	//if(dSize<1.0) {
	dSize = max(
		GetDocument()->GetActiveFloor()->GetGrid()->dSizeX,
		GetDocument()->GetActiveFloor()->GetGrid()->dSizeY);
	//}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-dSize,dSize,-dSize,dSize,-1,1); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4ubv(ARCColor4::BACKGROUND);
	glBegin(GL_QUADS);
	glVertex2d(-dSize,-dSize);
	glVertex2d(dSize,-dSize);
	glVertex2d(dSize,dSize);
	glVertex2d(-dSize,dSize);
	glEnd();

	GetDocument()->GetActiveFloor()->DrawOGL(this,0.0);
	GetDocument()->GetActiveFloor()->DrawGrid(0.0);
	double dFDist = GetCamera()->GetFocusDistance();
	ARCVector3 vLoc, vUp;
	GetCamera()->GetLocation(vLoc);
	GetCamera()->GetUpVector(vUp);
	ARCVector3 yAxis(0.0,1.0,0.0);
	//find angle between vUp and Y axis
	GLfloat angle =(GLfloat) ARCMath::RadiansToDegrees( acos(vUp.dot(yAxis)) );
	if(vUp[VX] < 0)
		angle = -angle;
	// TRACE("angle = %.2f\n", angle);
	double l = -0.8*dFDist*m_dAspect;// + vLoc[VX];
	double r = 0.8*dFDist*m_dAspect;// + vLoc[VX];
	double b = -0.8*dFDist;// + vLoc[VY];
	double t = 0.8*dFDist;// + vLoc[VY];
	glPushMatrix();
	glTranslated(vLoc[VX],vLoc[VY],0.0);
	glRotatef(angle,0.0f,0.0f,1.0);
	glColor3f(0.0f,0.0f,1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2d(l,b);
	glVertex2d(r,b);
	glVertex2d(r,t);
	glVertex2d(l,t);
	glEnd();
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,100.0,0.0,100.0,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glColor3ubv(ARCColor3::BLACK);
	glBegin(GL_LINE_LOOP);
	glVertex2i(0,0);
	glVertex2i(99,0);
	glVertex2i(99,99);
	glVertex2i(0,99);
	glEnd();

	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();

	glEnable(GL_DEPTH_TEST);
}

void C3DView::RenderGrids()
{
	if(GetCamera()-> GetProjectionType() == C3DCamera::perspective)
	{                      //3D
		const CFloorList& vTermFloorList = GetDocument()->GetFloorByMode( GetDocument()->m_systemMode ).m_vFloors;
		int nFloorCount = vTermFloorList.size();
		for(int i=0;i<nFloorCount;i++)
		{
			/*if (GetDocument()->m_systemMode == EnvMode_AirSide)
			{
				if( vTermFloorList[i]->IsVisible() )
					vTermFloorList[i]->DrawGrid(vTermFloorList[i]->RealAltitude()*GetDocument()->m_iScale);
			}
			else*/
			{
				if( vTermFloorList[i]->IsVisible() )
					vTermFloorList[i]->DrawGrid(vTermFloorList[i]->Altitude());
			}

		}

	}
	else    //2D
	{
		CFloor2 * pFloor = GetDocument()->GetCurModeFloor().GetActiveFloor();
		if( pFloor->IsVisible() ) pFloor->DrawGrid(0.0);
	}
}

static ARCVector2 GetShortestLinePoint(const std::vector<ARCVector2>& _From ,const ARCVector2& _to)
{
	if((int)_From.size() < 0)
		return ARCVector2()  ;
	DistanceUnit _Shortdistance =  _From[0].DistanceTo(_to);
	ARCVector2 _shortPoint = _From[0] ;
	for (int i = 1 ; i < (int) _From.size() ;i++)
	{
		DistanceUnit distance = 0 ;
		distance = _From[i].DistanceTo(_to) ;
		if(distance < _Shortdistance )
		{
			_shortPoint = _From[i] ;
			_Shortdistance = distance ;
		}
	}
	return _shortPoint ;
}

void C3DView::RenderBillBoard()
{
		
	GLint viewport[4];
	GLdouble mvmatrix[16], projmatrix[16];
	glGetIntegerv(GL_VIEWPORT, viewport);			// store the current viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);	// store the m-v matrix
	glGetDoublev(GL_PROJECTION_MATRIX, projmatrix); //store the projection matrix


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, viewport[2], 0.0, viewport[3],-1.0,1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//render selection box
	DWORD dwTick = GetTickCount();

	if(GetDocument()->m_eAnimState == CTermPlanDoc::anim_none  
		&& GetParentFrame()->m_vSelected.size() > 0  
		&& (dwTick % 1000)>500 )
	{
		ARCVector3 dWinPos;
		ARCVector3 worldpos;

		//draw first selection box
		SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin();
		worldpos = (*itr)->GetLocation();
		//get window pos
		gluProject(worldpos.x, worldpos.y, worldpos.z,
			mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));		
		glColor3ubv(processorselectboxcolor1);
		glPushMatrix();
		glTranslated(dWinPos.x, dWinPos.y, dWinPos.z);
		glCallList(GetParentFrame()->SelectionDLID());
		glPopMatrix();
		
		//draw other selection box
		glColor3ubv(processorselectboxcolor1);
		for(itr++;itr!=GetParentFrame()->m_vSelected.end();itr++)
		{
			worldpos = (*itr)->GetLocation();
			glColor3ubv(processorselectboxcolor2);
			gluProject(worldpos.x, worldpos.y, worldpos.z,
				mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));	
			
			glPushMatrix();
			glTranslated(dWinPos.x, dWinPos.y, dWinPos.z);
			glCallList(GetParentFrame()->SelectionDLID());
			glPopMatrix();
		}	
	}
	//render object names
	//render call out dlg lines
	CCalloutDlgLineData* pData = GetDocument()->GetCallOutLineData();
	ASSERT(pData);
	for(int i=0;i<pData->GetCount();i++)
	{
		if(!pData->GetItem(i)->NeedDrawLine())
			continue ;
		pData->GetItem(i)->UpdateTheCurrentDlgPosition();
		CCallOutLines* pLines = pData->GetItem(i)->GetDrawLinesData();
		const std::vector<Point>& vPts = pLines->m_processorPoint;

		const CRect& rect = pLines->m_rect;
		CPoint pt1 = rect.BottomRight();
		CPoint pt2 = rect.TopLeft();
		CPoint pt3 (rect.right,rect.top);
		CPoint pt4 (rect.left,rect.bottom);
		ScreenToClient(&pt1);
		ScreenToClient(&pt2);
		ScreenToClient(&pt3);
		ScreenToClient(&pt4);
		pt1.y =  viewport[3] - pt1.y;
		pt2.y =  viewport[3] - pt2.y;
		pt3.y =  viewport[3] - pt3.y;
		pt4.y =  viewport[3] - pt4.y;

		for(int j=0;j<(int)vPts.size();j++)
		{
			Point pt = vPts.at(j);
			double dz = GetDocument()->GetFloorByMode(EnvMode_Terminal).getVisibleAltitude(pt.getZ());

			ARCVector3 dWinPos;
			gluProject(pt.getX(), pt.getY(), dz,
				mvmatrix, projmatrix, viewport, &(dWinPos[0]), &(dWinPos[1]), &(dWinPos[2]));

			//get the closest pt
			std::vector<ARCVector2> _FromPoint ;

			ARCVector2 _point ;
			ARCVector2 _toPoint ;
			_point.x  = pt1.x ;
			_point.y = pt1.y ;
			_FromPoint.push_back(_point) ;


			_point.x  = pt2.x ;
			_point.y = pt2.y ;
			_FromPoint.push_back(_point) ;


			_point.x  = pt3.x ;
			_point.y = pt3.y ;
			_FromPoint.push_back(_point) ;

			_point.x  = pt4.x ;
			_point.y = pt4.y ;
			_FromPoint.push_back(_point) ;
			//
			_toPoint.x = dWinPos.x ;
			_toPoint.y = dWinPos.y ;
			_point = GetShortestLinePoint(_FromPoint,_toPoint) ;
			glBegin(GL_LINES);
			glVertex2d(dWinPos[0],dWinPos[1]);
			glVertex2d(_point.x,_point.y);
			glEnd();
		}


	}

	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ApplyPerspective();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	ApplyViewXForm();

}

CString StrEDEditControlPoint[2] = 
{
	"Enable Edit Control Point",
	"Disable Edit Control Point",
};

static CString GlobleEDctrlPoint = StrEDEditControlPoint[0];

void C3DView::SelectableMenuPop(Selectable * pSel,const CPoint& point)
{
	if(pSel->GetSelectType() == Selectable::ALT_OBJECT){
		
		ALTObject3D * pObj3D = (ALTObject3D * )pSel;
		ALTObject * pObj = pObj3D->GetObject();		
		CNewMenu menu, *pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_3DVIEW_PICK);
  //      if(pObj->GetType()!=ALT_HELIPORT)
		//{
		//	pCtxMenu =  DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(pObj->GetType()));
		//}
		//else
		//{
		//	pCtxMenu =  DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(21));
		//}
		if (pObj->GetType() == ALT_HELIPORT)
		{
			pCtxMenu =  DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(21));
		}
		else if (pObj->GetType() == ALT_CIRCLESTRETCH)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(22));
		}
		else if (pObj->GetType() == ALT_REPORTINGAREA)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		}
		else if (pObj->GetType() == ALT_STARTPOSITION)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		}
		else if (pObj->GetType() == ALT_MEETINGPOINT)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		}
		else if(pObj->GetType()== ALT_APAXBUSSPOT)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		}
		else if(pObj->GetType()==ALT_ABAGCARTSPOT)
		{
			pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(0));
		}
		else
		{
			pCtxMenu =  DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(pObj->GetType()));
		}

		if(pCtxMenu != NULL)
		{
			//Edit Point Item			
			/*if(pObj3D->IsInEdit())
				ASSERT(pCtxMenu->SetMenuText(ID_ALTOBJECT_EDITCONTROLPOINT,"Disable Edit Point ",MF_BYCOMMAND));
			else 
				ASSERT(pCtxMenu->SetMenuText(ID_ALTOBJECT_EDITCONTROLPOINT,"Enable Edit Point ",MF_BYCOMMAND));*/
			
			
			pCtxMenu->LoadToolBar(IDR_MAINFRAME); 
			pCtxMenu->SetMenuTitle(_T(pObj->GetTypeName()),MFT_GRADIENT|MFT_TOP_TITLE);				

			//Edit Point Item			
			if(pObj3D->IsInEdit())
			{
				pCtxMenu->SetMenuText(ID_ALTOBJECT_EDITCONTROLPOINT, "Disable Edit Control Point",MF_BYCOMMAND);
				//pCtxMenu->EnableMenuItem(ID_ALTOBJECT_OBJECTPROPERTY,MF_GRAYED);
			}
			else
			{
				pCtxMenu->SetMenuText(ID_ALTOBJECT_EDITCONTROLPOINT, "Enable Edit Control Point",MF_BYCOMMAND);
				//pCtxMenu->EnableMenuItem(ID_ALTOBJECT_OBJECTPROPERTY,MF_ENABLED);
			}

			CString strLockText = pObj->GetLocked() ? _T("Unlock ") : _T("Lock ");
			strLockText += pObj->GetObjNameString();
			pCtxMenu->SetMenuText(ID_ALTOBJECT_LOCK, strLockText, MF_BYCOMMAND);
			UINT nEnable = pObj->GetLocked() ? MF_BYCOMMAND | MF_DISABLED | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED;
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_DISPLAYPROPERTIES, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_EDITCONTROLPOINT, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_ADDEDITPOINT, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_DELETE, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_MOVE, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_ROTATE, nEnable);
			pCtxMenu->EnableMenuItem(ID_ALTOBJECT_FLIP, nEnable);
			pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, this );
		}

	}
	else if(pSel->GetSelectType() == Selectable::FILLET_TAXIWAY)
	{
		CFilletTaxiway3D * pFilletTaxiway  = (CFilletTaxiway3D*) pSel;
		CNewMenu menu, * pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_POPUP);
		pCtxMenu  = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(78));
		GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.SetInEditFillet(pFilletTaxiway->m_filetTaxway.GetID());
		if( GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.IsInEdit() )
		{
			pCtxMenu->SetMenuText(ID_FILLETTAXIWAY_EDITCONTROLPOINT, "Disable Edit Control Point",MF_BYCOMMAND );
		}
		else 
		{
			pCtxMenu->SetMenuText(ID_FILLETTAXIWAY_EDITCONTROLPOINT, "Enable Edit Control Point",MF_BYCOMMAND );
		}
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, this );
	}
	else if(pSel->GetSelectType() == Selectable::ALT_OBJECT_PATH_CONTROLPOINT)
	{
		CALTObjectPathControlPoint2008* pCtrlPt = (CALTObjectPathControlPoint2008*)pSel;
		ALTObject3D * pObj3D = pCtrlPt->GetObject();
		ALTObject * pObj = pObj3D->GetObject();

		CNewMenu menu, *pCtxMenu = NULL;
		menu.LoadMenu(IDR_MENU_REMOVEEDITPOINT);
		pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu, menu.GetSubMenu(0));
		pCtxMenu->SetMenuTitle(_T("Remove Edit Point"),MFT_GRADIENT | MFT_TOP_TITLE);
		UINT nEnable = pObj->GetLocked() ? MF_BYCOMMAND | MF_DISABLED | MF_GRAYED : MF_BYCOMMAND | MF_ENABLED;
		pCtxMenu->EnableMenuItem(ID_REMOVE_POINT, nEnable);
		pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
	//else if (pSel->GetSelectType() == Selectable::INTERSECTED_STRETCH)
	//{
	//	CIntersectedStretch3D * pStretch3D = (CIntersectedStretch3D*)pSel;
	//	CNewMenu menu,*pCtxMenu = NULL;
	//	menu.LoadMenu(IDR_MENU_POPUP);
	//	pCtxMenu = DYNAMIC_DOWNCAST(CNewMenu,menu.GetSubMenu(84));
	//	GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vIntersectedStretch.SetInEditFillet(pStretch3D->m_interStretch.GetID());
	//	if(GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vIntersectedStretch.IsInEdit())
	//	{
	//		pCtxMenu->SetMenuText(ID_INTERSECTEDSTRETCH_EDITPOINT, "Disable Edit Control Point",MF_BYCOMMAND );
	//	}
	//	else
	//	{
 //           pCtxMenu->SetMenuText(ID_INTERSECTEDSTRETCH_EDITPOINT, "Enable Edit Control Point",MF_BYCOMMAND );
	//	}
	//	pCtxMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON | TPM_LEFTBUTTON, point.x, point.y, this);
	//}
}

CAirside3D * C3DView::GetAirside3D()
{
	return GetParentFrame()->GetAirside3D();
}
void C3DView::OnALTObjectProperty()
{
	// TODO: Add your command handler code here
	if(GetParentFrame()->m_vSelected.size()<1)return;
	Selectable *pSel = ( *GetParentFrame()->m_vSelected.begin() ).get();

	if( pSel->GetSelectType() == Selectable::ALT_OBJECT ){
		ALTObject3D * pObj3D = (ALTObject3D*)pSel;		
		{
			ALTObject* pObj = pObj3D->GetObject();
			CDialog * pDlg = CAirsideObjectBaseDlg::NewObjectDlg(pObj->getID(),pObj->GetType(),pObj->getAptID(),pObj->getAptID(),GetDocument()->GetProjectID(), this);
			if( pDlg->DoModal() == IDOK ){
				if( ((CAirsideObjectBaseDlg*)pDlg)->m_bNameModified )
				{					
					GetDocument()->UpdateAllViews(NULL,VM_CHANGENAME_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg*)pDlg)->GetObject() );			
				}

				if( ((CAirsideObjectBaseDlg*)pDlg)->m_bPropModified || ((CAirsideObjectBaseDlg*)pDlg)->m_bPathModified ) 
				{
					ALTObject3DList vObj3Ds;
					vObj3Ds.push_back(pObj3D);					
					GetAirside3D()->UpdateObject(pObj3D->GetID());
					GetAirside3D()->ReflectChangeOf(vObj3Ds);
					GetDocument()->UpdateAllViews(this,VM_MODIFY_ALTOBJECT,(CObject*)((CAirsideObjectBaseDlg*)pDlg)->GetObject() );		
				}				
			}
			else 
			{
				GetAirside3D()->UpdateObject(pObj3D->GetID());
			}

			delete pDlg;
		}
	}
	
}
void C3DView::OnALTObjectDisplayProperty()
{
	if(GetParentFrame()->m_vSelected.size()<1)return;
	Selectable *pSel = ( *GetParentFrame()->m_vSelected.begin() ).get();

	if( pSel->GetSelectType() == Selectable::ALT_OBJECT ){
		ALTObject3D * pObj3D = (ALTObject3D*)pSel;
		
		CDlgAirsideObjectDisplayProperties dlg(pObj3D);
		dlg.DoModal();

	}
}
void C3DView::OnAltobjectCopy()
{
	// TODO: Add your command handler code here
	if(GetParentFrame()->m_vSelected.size()<1)return;
	
	ALTObjectList newObjList;
	ALTObjectUIDList newObjIDList;

	for(SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin();itr!=GetParentFrame()->m_vSelected.end();itr++){
		Selectable  * pSel = (*itr).get();
		if(pSel->GetSelectType() == Selectable::ALT_OBJECT ){
			ALTObject3D * pObj3D = (ALTObject3D*) pSel;
			ALTObject * pObj = pObj3D->GetObject();
			ALTObject * pObjCopy = pObj->NewCopy();
			if (NULL == pObjCopy)
				continue; // some noncopyable ALTObject, such as airroute
			try
			{
				CADODatabase::BeginTransaction() ;

				ALTObjectID newName;
				pObj->GetNextObjectName(newName);				
				pObjCopy->setObjName(newName);
				
				int id = pObjCopy->SaveObject(pObj->getAptID());

				//copy display prop
				ALTObjectDisplayProp* pDislplay =  pObj3D->GetDisplayProp();
				pDislplay->m_nID = -1;
				pDislplay->SaveData(id);
				pDislplay->ReadData(pObj->getID());

				newObjList.push_back(pObjCopy);
				newObjIDList.push_back(id);
				CADODatabase::CommitTransaction() ;
			}
			catch (CADOException& e) 
			{
				CADODatabase::RollBackTransation() ;
				e.ErrorMessage();
				MessageBox(_T("Some DataBase Error happned When Copying"),MB_OK) ;
			}	
			
		}
	}
	
	if(newObjList.size())
	{
		
		GetAirside3D()->UpdateAddorRemoveObjects();
		GetDocument()->UpdateAllViews(this,VM_NEW_MULT_ALTOBJECTS,(CObject*)&newObjList);
		GetParentFrame()->UnSelectAll();

		for(int i =0;i<(int)newObjIDList.size();i++){
			ALTObject3D * pObj = NULL;
			if( pObj = GetAirside3D()->GetObject3D(newObjIDList[i]) ){			
				GetParentFrame()->Select(pObj);
			}
		}
	}

	

}

void C3DView::OnAltobjectMove()
{
	// TODO: Add your command handler code here
	if( GetDocument()->m_bLayoutLocked ) 
		return;
}

void C3DView::OnAltobjectRotate()
{
	if( GetDocument()->m_bLayoutLocked ) 
		return;
	// TODO: Add your command handler code here
	ALTObject3DList SelectedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);
	m_dRotate = 0.0;
	ASSERT( SelectedList.size()>0 );
	m_eMouseState = NS3DViewCommon::_rotateairsideobject;
	DWORD dwPos = ::GetMessagePos();
	m_ptFrom.x = (int) LOWORD(dwPos);
	m_ptFrom.y = (int) HIWORD(dwPos);
	this->ScreenToClient(&m_ptFrom);
	SetCapture();
}

void C3DView::OnAltobjectDelete()
{
	// TODO: Add your command handler code here
	if(GetParentFrame()->m_vSelected.size()<1)return;
	
	
	ALTObject3DList SelectedList;
	ALTObjectList DeletedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);

	ALTObjectList delteObjs;

	for(ALTObject3DList::iterator itr = SelectedList.begin();itr!= SelectedList.end();itr++)
	{
		ALTObject3D * pObj3D = (*itr).get();
		delteObjs.push_back(pObj3D->GetObject());		
	}
	GetDocument()->DeleteALTObjects(delteObjs);
	if(delteObjs.size())
	{
		GetAirside3D()->UpdateAddorRemoveObjects();
		GetDocument()->UpdateAllViews(this,VM_DELECT_ALTOBJECT,(CObject*)&delteObjs);
		GetParentFrame()->UnSelectAll();
	}
	
}


//void C3DView::OnAltobjectCopyClipboard()
//{
//	// TODO: Add your command handler code here
//}
//
//void C3DView::OnAltobjectReflect()
//{
//	// TODO: Add your command handler code here
//	if( GetDocument()->m_bLayoutLocked ) 
//		return;
//}
//
//void C3DView::OnAltobjectFlip()
//{	
//	if( GetDocument()->m_bLayoutLocked ) 
//		return;
//	// TODO: Add your command handler code here
//}


void C3DView::OnAltobjectEditcontrolpoint()
{
	if( GetDocument()->m_bLayoutLocked ) 
		return;
	// TODO: Add your command handler code here
	ALTObject3DList selectObject;
	GetParentFrame()->m_vSelected.GetSelectObjects(selectObject);
	if(selectObject.size()){
		ALTObject3D * pObj3D = selectObject.begin()->get();
		if(pObj3D)
			pObj3D->SetInEdit(!pObj3D->IsInEdit());
	}
}

void C3DView::OnAltobjectAddeditpoint()
{
	// TODO: Add your command handler code here
	if(GetDocument()->m_bLayoutLocked)
		return;
	ALTObject3DList selectObject;
	GetParentFrame()->m_vSelected.GetSelectObjects(selectObject);
	if(selectObject.size())
	{
		ALTObject3D* pObj3D = selectObject.begin()->get();
		if(pObj3D)
		{
			pObj3D->AddEditPoint(m_ptMousePos[VX], m_ptMousePos[VY], 0.0);
			pObj3D->FlushChange();
		}
	}
	Invalidate(FALSE);
}

void C3DView::OnRemovePoint()
{
	// TODO: Add your command handler code here
	for(SelectableList::iterator itr = GetParentFrame()->m_vSelected.begin(); itr!=GetParentFrame()->m_vSelected.end(); itr++)
	{
		Selectable * pSel = (*itr).get();
		if (pSel->GetSelectType() == Selectable::ALT_OBJECT_PATH_CONTROLPOINT)
		{
			CALTObjectPathControlPoint2008 * altObj = (CALTObjectPathControlPoint2008 *)pSel;
			ALTObject3D * pObj3D = altObj->GetObject();
			(*itr)->DeleteEditPoint(pObj3D);	
			(*itr)->FlushChange();
		}
//		(*itr)->DeleteEditPoint();

	}

	Invalidate(FALSE);
}


void C3DView::RotateSelectObject( double dx )
{
	if( GetDocument()->m_bLayoutLocked )
		return;

	ALTObject3DList SelectedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);
	for(int i=0;i<(int)SelectedList.size();i++)
	{
		ALTObject3D * pObj3D = SelectedList.at(i).get();
		pObj3D->OnRotate(dx);
	}
	m_dRotate += dx;
}

std::vector<CFloor2*> C3DView::GetFloorsByMode( EnvironmentMode mode ) 
{	

	if(mode == EnvMode_AirSide)
	{
		return GetParentFrame()->GetAirside3D()->GetAllAirportLevels();
	}
	else if(mode == EnvMode_LandSide)
	{
		return std::vector<CFloor2*> (GetDocument()->GetFloorByMode(EnvMode_LandSide).m_vFloors);
	}
	else if(mode == EnvMode_Terminal)
	{
		return std::vector<CFloor2*> (GetDocument()->GetFloorByMode(EnvMode_Terminal).m_vFloors);
	}
	return std::vector<CFloor2*> ();
}

std::vector<CFloor2*> C3DView::GetCurModeFloors()
{
	return GetFloorsByMode(GetDocument()->GetCurrentMode());
}

void C3DView::OnSubmenuEnableeditcontrolpoint()
{
	if(m_procCurEditCtrlPoint)
	{
		m_procCurEditCtrlPoint->SetProCtrlPointVisible(!(m_procCurEditCtrlPoint->GetProCtrlPointVisible()));
		if(m_procCurEditCtrlPoint->GetProCtrlPointVisible() == false)
		{
			//save move result
			if(GetDocument())
			{
				CString sPath = GetDocument()->m_ProjInfo.path;

				if(GetDocument()->GetCurrentPlacement())
					GetDocument()->GetCurrentPlacement()->saveDataSet( sPath, false);

				GetDocument()->GetProcessorList().saveDataSet(sPath, true);

				if(GetDocument()->GetTerminal().pRailWay)
					GetDocument()->GetTerminal().pRailWay->saveDataSet( sPath, false );

				if(GetDocument()->GetTerminal().m_pAllCarSchedule)
					GetDocument()->GetTerminal().m_pAllCarSchedule->saveDataSet( sPath,false );

				GetDocument()->m_portals.saveDataSet(sPath,false);

				if(GetDocument()->GetTerminal().m_pAreas)
					GetDocument()->GetTerminal().m_pAreas->saveDataSet( sPath,false );

				if(GetDocument()->GetTerminal().m_pStructureList)
					GetDocument()->GetTerminal().m_pStructureList->saveDataSet( sPath,false );

				if(GetDocument()->GetTerminal().m_pPipeDataSet)
					GetDocument()->GetTerminal().m_pPipeDataSet->saveDataSet( sPath,false );

				if(GetDocument()->GetTerminal().m_pWallShapeList)
					GetDocument()->GetTerminal().m_pWallShapeList->saveDataSet( sPath,false );
			}

		}
	}

}

void C3DView::OnUpdateSubmenuEnableeditcontrolpoint(CCmdUI *pCmdUI)
{
	if(m_procCurEditCtrlPoint)
	{
		CString strTitle = _T("");		
		if( !(m_procCurEditCtrlPoint->GetProCtrlPointVisible()) )
			strTitle = _T("Enable Edit Control Point");
		else
			strTitle = _T("Disable Edit Control Point");
		pCmdUI->SetText(strTitle);
	}
}

//
void DrawOnGroundCube()
{
	const static float flen = 0.5f;
	float pts[8][3] = 
	{
		{ flen, flen, 0}, { -flen, flen, 0} , { -flen, -flen, 0}, { flen, -flen, 0 },
		{ flen, flen, 1}, { -flen, flen, 1} , { -flen, -flen, 1}, { flen, -flen, 1 },
	};
	glBegin(GL_QUADS);
	//top
	glNormal3f(0,0,1.0f);
	glVertex3fv(pts[4]);
	glVertex3fv(pts[5]);
	glVertex3fv(pts[6]);
	glVertex3fv(pts[7]);
	//bottom
	glNormal3f(0,0,-1.0f);
	glVertex3fv(pts[3]);
	glVertex3fv(pts[2]);
	glVertex3fv(pts[1]);
	glVertex3fv(pts[0]);
	//-y
	glNormal3f(0,-1,0);
	glVertex3fv(pts[2]);
	glVertex3fv(pts[6]);
	glVertex3fv(pts[5]);
	glVertex3fv(pts[1]);
	//+y
	glNormal3f(0,1,0);
	glVertex3fv(pts[0]);
	glVertex3fv(pts[4]);
	glVertex3fv(pts[7]);
	glVertex3fv(pts[3]);
	//-x
	glNormal3f(-1,0,0);
	glVertex3fv(pts[2]);
	glVertex3fv(pts[3]);
	glVertex3fv(pts[7]);
	glVertex3fv(pts[6]);
	//+x
	glNormal3f(1,0,0);
	glVertex3fv(pts[0]);
	glVertex3fv(pts[1]);
	glVertex3fv(pts[5]);
	glVertex3fv(pts[4]);
	
	glEnd();

}


bool RenderSpecialVehicle(const AirsideVehicleDescStruct& desc, CString sShape,const ARCVector3& pos, const ARCVector3& dangles, 
						  const CAirsideEventLogBuffer<AirsideVehicleEventStruct>::ELEMENTEVENT& ItemEventList, int pesAIdx, C3DView* pView )
{
	CString cartOrDolly = _T("Cart");
	int cartCount = 3;
	bool bSpecial  = false;
	
	if( sShape == _T("Baggage Cart x4"))
	{
		cartCount = 4;
		bSpecial = true;
	}
	else if( sShape == _T("Baggage Cart x3"))
	{ 
		bSpecial = true;
	}
	else if( sShape == _T("Baggage Cart x2") )
	{
		cartCount = 2;
		bSpecial = true;
	}
	if( sShape == _T("Cargo Dolly x3"))
	{
		bSpecial = true;
		cartCount = 3;
		cartOrDolly = _T("Dolly");
	}
	if( sShape == _T("Cargo Dolly x2") )
	{
		bSpecial = true;
		cartCount = 2;
		cartOrDolly = _T("Dolly");
	}
	if(!bSpecial)
		return false;


	double tugWidth = 120;
	double tugLength = 300;
	double tugHeight = desc.vehicleheight;

	double cartwidth = desc.vehiclewidth;
	double cartLength = (desc.vehiclelength-tugLength)/cartCount;
	double cartHeight = desc.vehicleheight;

	//draw tug
	CglShape * pTugShape =  pView->GetShapeResource()->GetShape( "Tug" );
	CglShape* pCartShape = pView->GetShapeResource()->GetShape(cartOrDolly);
	if(!pTugShape || !pCartShape)
		return false;

	glPushMatrix();
	glTranslated(pos.x,pos.y,pos.z);
	glRotated(dangles[VZ],0,0,1);
	glScaled(tugLength,tugWidth,tugHeight);
	pTugShape->display();
	glPopMatrix();
	

	//draw carts
	DistanceUnit dist = (cartLength-tugLength)*0.5;
	ARCVector3 prePt = pos;
	ARCVector2 preDir = ARCVector2(-dist+cartLength,0);
	double dAngle = dangles[VZ];
	preDir.Rotate(-dAngle);
	
	ARCVector3 cartPos = pos;
	for(int i = pesAIdx;i>=0;i--)
	{
		const AirsideVehicleEventStruct& evt = ItemEventList.second[i];
		ARCVector3 pti = ARCVector3(evt.x,evt.y,evt.z);
		DistanceUnit disti = pti.DistanceTo(prePt);
		dist += disti;
		while(dist>=cartLength)
		{
			
			double dr = (dist-cartLength)/disti;
			ARCVector3 nextPos = dr*prePt + (1.0-dr)*pti; //point in trace
			preDir = ( cartPos -nextPos).xy();
			dAngle = preDir.AngleFromCoorndinateX();
			preDir.SetLength(cartLength);

			////draw a cart
			glPushMatrix();
			glTranslated(cartPos.x,cartPos.y,cartPos.z);
			glRotated(dAngle,0,0,1);
			glScaled(cartLength,cartwidth,cartHeight);
			pCartShape->display();
			glPopMatrix();
			cartPos += ARCVector3(-preDir.x,-preDir.y,0);//nextPos;
			//
			dist-=cartLength;
			cartCount--;

			if(cartCount<=0)
				break;	
		}
		if(cartCount<=0)
			break;

		prePt = pti;
	}
	
	//draw left cart
	for(int i=0;i<cartCount;i++)
	{
		///draw a cart 
		preDir.SetLength(cartLength);
		glPushMatrix();
		glTranslated(cartPos.x,cartPos.y,cartPos.z);
		glRotated(dAngle,0,0,1);
		glScaled(cartLength,cartwidth,cartHeight);
		pCartShape->display();
		glPopMatrix();
		//
		cartPos+= ARCVector3(-preDir.x,-preDir.y,0);
	}

	return true;
}



void C3DView::DrawVehicle(const DistanceUnit& dAirprotAlt, BOOL bSelectMode,BOOL _bShowTag , const double* mvmatrix , const double* projmatrix, const int* viewport)
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc != NULL);
	//#if 0	
	CAirsideSimLogs& airsideSimLogs = pDoc->GetAirsideSimLogs();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	CVehicleDispProps& vehicledispprops = pDoc->m_vehicleDispProps;
	std::vector<int>& vehicledisppropidx = pDoc->m_vAVehicleDispPropIdx;
	std::vector<CString>& vehicleshapeidx = pDoc->m_vAVehicleShapeNameIdx;

	AirsideVehicleLogEntry element;	
	element.SetEventLog(&(airsideSimLogs.m_airsideVehicleEventLog));


	AirsideVehicleLog& airsideVehicleLog = airsideSimLogs.m_airsideVehicleLog;
	const CAirsideEventLogBuffer<AirsideVehicleEventStruct>& airsideVehicleEventLog = GetDocument()->GetOutputAirside()->GetLogBuffer()->m_vehicleLog;
	
	//int nCount = airsideVehicleLog.getCount();
	size_t nCount = airsideVehicleEventLog.m_lstElementEvent.size();
	for (size_t i = 0; i < nCount; i++)
	{
		const CAirsideEventLogBuffer<AirsideVehicleEventStruct>::ELEMENTEVENT& ItemEventList = airsideVehicleEventLog.m_lstElementEvent[i];
		airsideVehicleLog.getItem(element, ItemEventList.first);	

		const AirsideVehicleDescStruct& desc = element.GetAirsideDesc();
		double vehicleheight = desc.vehicleheight;
		double vehiclelength = desc.vehiclelength;
		double vehiclewidth  = desc.vehiclewidth;

		long nStartTime = (long) element.getEntryTime();
		long nEndTime = (long) element.getExitTime();

		if (nStartTime <= nCurTime && nEndTime >= nCurTime) 
		{	
			ARCVector3 pos;
			ARCVector3 dangles;
			int pesAIdx = -1;
			double dSpeed =0;
			if( CAnimationManager::GetVehicleShow(pDoc,ItemEventList,nCurTime,dAirprotAlt
												 ,pos,dangles,dSpeed,pesAIdx)	)
			{
			
				const AirsideVehicleEventStruct& pesA = ItemEventList.second[pesAIdx];
				if( GetCamera()->SphereInCamera( Point(pos[VX],pos[VY],pos[VZ]),5000) )
				{	
					//if(bSelectMode)	glLoadName(i);					
					glColor4ub(255,255,255,255);

					CString vehicleShapeName = element.GetAirsideDesc().vehicleType;

					CVehicleDispPropItem * pDispItem = vehicledispprops.FindBestMatch(element.GetAirsideDesc().id);
					CString shapeName;
					if(pDispItem)
					{
						ARCColor4 col(pDispItem->GetColor());
						glColor4ubv(col);	
						shapeName = SHAPESMANAGER->GetShapeName( pDispItem->GetShape().first, pDispItem->GetShape().second );	
						if(bSelectMode)
						{
							glLoadName(i+SEL_VEHICLE_OFFSET);
						}

						if (!bSelectMode&& m_nPaxSeledID==(i+SEL_VEHICLE_OFFSET))
						{
							if(m_nTickTotal/8%2==0)
							{
								glColor4ub(255-col[RED],255-col[GREEN],255-col[BLUE],col[ALPHA]);
							}

							m_nTickTotal++;
							if(m_nTickTotal>=2E15)
								m_nTickTotal=0;

						}
					}


					CVehicleDispProps& vehicleDispprops = pDoc->m_vehicleDispProps;					
					//draw to color buffer
					glEnable(GL_LIGHTING);					
					if( !RenderSpecialVehicle(desc, shapeName , pos,dangles,ItemEventList,pesAIdx ,this) ) //draw cargo baggage 
					{
						glPushMatrix();
						glTranslated(pos[VX],pos[VY],pos[VZ]);				
						glRotated(dangles[VZ],0,0,1);
						glRotated(dangles[VY],0,1,0);
						glRotated(dangles[VX],1,0,0);
						CglShape * pVehicleshape =  GetShapeResource()->GetShape( shapeName );
						glScaled(vehiclelength, vehiclewidth, vehicleheight);	
						if(pVehicleshape)
						{	
							pVehicleshape->display();					
						}
						else
						{
							DrawOnGroundCube();						
						}
						glPopMatrix();
					}
					CVehicleTag& vehicleTag = pDoc->m_vehicleTags;
					std::vector<int>& vehicleDisppropidx = pDoc->m_vAVehicleDispPropIdx;

					int CurNodeIndex = vehicleTag.GetCurSelNodeIndex();
					int CurItemIndex = vehicleTag.GetCurSelItemIndexInCurSelNode();
					DWORD dwTagInfo;
					AirsideVehicleDescStruct& getdesc  = element.GetAirsideDesc();	
					bool bShowTag = _bShowTag && vehicleTag.GetItemTagInfoInNode(CurNodeIndex,CurItemIndex,dwTagInfo);
					if(bShowTag)
					{					
						glDisable(GL_LIGHTING);
						glDisable(GL_DEPTH_TEST);
						DrawVehicleTag( getdesc,pesA,dwTagInfo,pos+ARCVector3(0,0,vehicleheight*0.5) );
						glEnable(GL_DEPTH_TEST);
					}					
				}				


				CHECK_GL_ERRORS("vehicle display");
 

			}
		}
	}
}
void C3DView::OnFillettaxiwayEditcontrolpoint()
{
	// TODO: Add your command handler code here
	if( GetParentFrame()->m_vSelected.size() < 1 ) return;

	Selectable * pSel = GetParentFrame()->m_vSelected.at(0).get();
	if(pSel && pSel->GetSelectType() == Selectable::FILLET_TAXIWAY)
	{
		CFilletTaxiway3D * pFillet = (CFilletTaxiway3D*)pSel;
		bool bInEdit = GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.IsInEdit();
		GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.SetEdit(!bInEdit);
		GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vFilletTaxiways.SetInEditFillet(pFillet->m_filetTaxway.GetID());
	}

}

void C3DView::OnWallshapeCopy()
{
	 //TODO: Add your command handler code here
	WallShape *pWall = GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape);
    WallShape *pWallCopy = pWall->NewCopy();
    if(pWallCopy->setID(GetDocument()->GetCurWallShapeList().GetNextWallID(pWall->getID()).GetIDString()))
	{
		GetDocument()->GetCurWallShapeList().addShape(pWallCopy);
		CStructureID id = pWallCopy->getID();

		std::stack<CString> vList;
		int nLength = id.idLength();
		for (int i = 0; i < nLength; i++)
		{
			vList.push(id.GetLeafName());
			id.clearLevel(id.idLength() - 1);
		}

		int nIndex = 0;
		CTVNode* pNode = NULL;
		CTVNode* pTempNode = NULL;
		int idLength = id.idLength();
		CString strNodeName = _T("");

		CString strProcTypeName;
		if (strProcTypeName.LoadString(IDS_TVNN_WALLSHAPE))
		{
			CTVNode* pWallNode = GetDocument()->m_msManager.FindNodeByName(strProcTypeName);
			if (pWallNode == NULL)
				return;

			pWallNode->m_eState = CTVNode::expanded;
			pNode = pWallNode;
			while(vList.size())
			{
				pTempNode = pNode;
				strNodeName = vList.top();
				vList.pop();
				pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
				if (pNode == NULL)
				{
					if (vList.size() == 0)
					{
						CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_WALLSHAPE);
						pTempNode->AddChild(pChildNode);
						pChildNode->m_eState = CTVNode::expanded;
						pChildNode->m_dwData = (DWORD) pWallCopy;
					}
					else
					{
						pNode = new CTVNode(strNodeName);
						pTempNode->AddChild(pNode);
						pNode->m_eState = CTVNode::expanded;
					}
				}	
			}
			GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pWallNode);
		}
	}
	Invalidate(FALSE);
}

void C3DView::OnSubmenuCopysurface()
{
	 //TODO: Add your command handler code here
	CStructure *pStructure = GetDocument()->GetCurStructurelist().getStructureAt(m_nSelectedStructure);
	CStructure *pStructCopy = pStructure->GetNewCopy();
	if (pStructCopy->setID(GetDocument()->GetCurStructurelist().GetNextStructureID(pStructure->getID()).GetIDString()))
	{
		GetDocument()->GetCurStructurelist().addStructure(pStructCopy);
		GetDocument()->UpdateStructureChange(false);
		CStructureID id = pStructCopy->getID();
		std::stack<CString> vList;
		int nLength = id.idLength();
		for (int i = 0; i < nLength; i++)
		{
			vList.push(id.GetLeafName());
			id.clearLevel(id.idLength() - 1);
		}

		int nIndex = 0;
		CTVNode* pNode = NULL;
		CTVNode* pTempNode = NULL;
		int idLength = id.idLength();
		CString strNodeName = _T("");

		CTVNode* pSurNode = GetDocument()->m_msManager.GetSurfaceAreasNode();
		pNode = pSurNode;
		pNode->m_eState = CTVNode::expanded;
		while(vList.size())
		{
			pTempNode = pNode;
			strNodeName = vList.top();
			vList.pop();
			pNode = (CTVNode*)pNode->GetChildByName(strNodeName);
			if (pNode == NULL)
			{
				if (vList.size() == 0)
				{
					CTVNode* pChildNode = new CTVNode(strNodeName,IDR_CTX_STRUCTURE);
					pTempNode->AddChild(pChildNode);
					pChildNode->m_eState = CTVNode::expanded;
					pChildNode->m_dwData = (DWORD) pStructCopy;
				}
				else
				{
					pNode = new CTVNode(strNodeName);
					pTempNode->AddChild(pNode);
					pNode->m_eState = CTVNode::expanded;
				}
			}
		}
		GetDocument()->UpdateAllViews(NULL,NODE_HINT_REFRESHNODE,(CObject*)pSurNode);
	}
	Invalidate(FALSE);
}




void C3DView::OnIntersectedstretchEditpoint()
{
	//// TODO: Add your command handler code here
	//if( GetParentFrame()->m_vSelected.size() < 1 ) return;

	//Selectable * pSel = GetParentFrame()->m_vSelected.at(0).get();
	//if(pSel && pSel->GetSelectType() == Selectable::INTERSECTED_STRETCH)
	//{
	//	CIntersectedStretch3D * pStretch = (CIntersectedStretch3D*)pSel;
	//	bool bInEdit = GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vIntersectedStretch.IsInEdit();
	//	GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vIntersectedStretch.SetEdit(!bInEdit);
	//	GetParentFrame()->GetAirside3D()->GetActiveAirport()->m_vIntersectedStretch.SetInEditFillet(pStretch->m_interStretch.GetID());
	//}
}

void C3DView::OnUpdatePro2Name(CCmdUI* pCmdUI)
{
	CObjectDisplay *pObjectDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	CProcessor2 * pProc2 =	(CProcessor2 *)pObjectDisplay ;

	if (pProc2->GetProcessor() == NULL)
	{
		pCmdUI->Enable(TRUE);
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}
}

void C3DView::OnRenamePro2()
{
	CObjectDisplay* pObjDisplay = GetDocument()->GetSelectedObjectDisplay(0);
	if(pObjDisplay == NULL)
	{
		TRACE("Error occur in function void CNodeView::OnCtxDeleteProc() ");
		return;
	}
	if(pObjDisplay->GetType() == ObjectDisplayType_OtherObject)
		return;
	CProcessor2* pProc2 = (CProcessor2*)pObjDisplay;	
	CString shapeName = pProc2->ShapeName().GetIDString();
	ASSERT(pProc2 != NULL);
	CDlgPro2Name dlg(pProc2,this);
	if (IDOK == dlg.DoModal())
	{
		CTVNode* pLayoutNode = NULL;
		CTVNode* pUCParent = GetDocument()->UnderConstructionNode();
		pLayoutNode = (CTVNode*) pUCParent->Parent();
		CString newShapeName = pProc2->ShapeName().GetIDString();
		pProc2->ShapeName(shapeName);
		GetDocument()->GetCurrentPlacement()->m_vUndefined.removePro2(pProc2);
		pProc2->ShapeName(newShapeName);
		GetDocument()->GetCurrentPlacement()->m_vUndefined.setNode(pProc2->ShapeName());
		GetDocument()->GetCurrentPlacement()->m_vUndefined.InsertUnDefinePro2(std::make_pair(pProc2->ShapeName().GetIDString(),pProc2));
		GetDocument()->GetCurrentPlacement()->m_vUndefined.InsertShapeName(newShapeName);
		GetDocument()->m_msManager.m_msImplTerminal.RebuildProcessor2Tree();
		GetDocument()->UpdateAllViews(NULL, NODE_HINT_REFRESHNODE, (CObject*) pLayoutNode);
		GetDocument()->GetCurrentPlacement()->saveDataSet(GetDocument()->m_ProjInfo.path, true);
		GetDocument()->RefreshProcessorTree();
	}
}

void C3DView::OnUpdateStandAlign(CCmdUI* pCmdUI)
{
	ALTObject3DList SelectedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);
	if (SelectedList.size() <= 1)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	for (int i = 0; i < (int)SelectedList.size(); i++)
	{
		if (SelectedList[i]->GetObjectType() != ALT_STAND)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}
	pCmdUI->Enable(TRUE);
}

void C3DView::OnStandAlign()
{
	ALTObject3DList SelectedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);
	ASSERT(SelectedList.size() > 0);
	m_vMousePosList.clear();
	m_eMouseState = NS3DViewCommon::_getalignline;
}

void C3DView::DrawStairs()
{
	CTermPlanDoc* pDoc = GetDocument();
	AirsideFlightStairLogs& stairLog  = pDoc->GetAirsideSimLogs().GetFlightStairLog();
	long nCurTime = pDoc->m_animData.nLastAnimTime;

	for(int i=0;i<(int)stairLog.GetItemCount();i++)
	{
		AirsideFlightStairsLog* pLogItem = stairLog.GetItem(i);
		if(!pLogItem)
			continue;

		if( (long)pLogItem->m_tStartTime <= nCurTime && nCurTime <= (long)pLogItem->m_tEndTime)
		{
			CRenderUtility::DrawStair(pLogItem->m_DoorPos,pLogItem->m_GroundPos,pLogItem->m_dStairwidth*SCALE_FACTOR);
		}
	}

}

void C3DView::OnALTObjectLock()
{
	// TODO: Add your command handler code here
	ALTObject3DList SelectedList;
	GetParentFrame()->m_vSelected.GetSelectObjects(SelectedList);
	if (SelectedList.size())
	{
		ALTObject* pObj = SelectedList.at(0).get()->GetObject();
		pObj->SetLocked(!pObj->GetLocked());
		ALTObject::SetLockedByID(pObj->getID(), pObj->GetLocked());
		GetDocument()->UpdateAllViews(this,VM_MODIFY_ALTOBJECT,(CObject*)pObj);
	}
	SetCapture();
}

void C3DView::OnNewMouseState()
{
	ClearPointPickStruct();
	m_vSelectLandProcs.clear();
	m_vSelectedStretchPos.clear();
}

void C3DView::OnUpdateAddPoint( CCmdUI* pCmdUI )
{
	if (m_nSelectWallShape >= 0)
	{
		if( GetDocument()->GetCurWallShapeList().getShapeAt(m_nSelectWallShape)->IsEditMode())
			pCmdUI->Enable(TRUE);
		else
			pCmdUI->Enable(FALSE);
	}
	
}

CCameraData* C3DView::GetCameraData() const
{
	return m_pCamera;
}

void C3DView::OnBeginCrossWalk()
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	if (pDoc == NULL)
		return;

	CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(m_nSelectedPipe >=0 && m_nSelectedPipe < nPipeCount);
	CPipe* pPipe = pPipeDS->GetPipeAt(m_nSelectedPipe);
	ASSERT(pPipe);
	if (pPipe == NULL)
		return;
	
	InputLandside* pInputLandside = pDoc->GetInputLandside();
	CDlgSelectLandsideObject dlg(&pInputLandside->getObjectList(),this);
	dlg.AddObjType(ALT_LCROSSWALK);
	if(dlg.DoModal() == IDOK)
	{
		if (dlg.getSelectObject().IsBlank())
		{
			MessageBox(_T("Please select single crosswalk"),_T("Warning"));
			return;
		}
		LandsideCrosswalk* pCrossWalk = (LandsideCrosswalk*)pInputLandside->getObjectList().getObjectByName(dlg.getSelectObject());
		if (pCrossWalk == NULL)
		{
			MessageBox(_T("You can't select group crosswalk"),_T("Warning"));
			return;
		}
		CrossWalkAttachPipe crossPipe(GetDocument()->GetInputLandside());
		crossPipe.AttachCrossWalk(pCrossWalk);
		GroupIndex groupIndex;
		groupIndex.start = m_nSelectedPipePt;
		groupIndex.end = -1;
		crossPipe.SetGroupIndex(groupIndex);
		pPipe->AddAttachCrosswalk(crossPipe);

		pPipeDS->saveDataSet(pDoc->m_ProjInfo.path,true);
	}
}

void C3DView::OnEndCrossWalk()
{
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	if (pDoc == NULL)
		return;

	CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(m_nSelectedPipe >=0 && m_nSelectedPipe < nPipeCount);
	CPipe* pPipe = pPipeDS->GetPipeAt(m_nSelectedPipe);
	ASSERT(pPipe);
	if (pPipe == NULL)
		return;

	CDlgPipeAttachCrosswalk dlg(pPipe,m_nSelectedPipePt,this);
	dlg.DoModal();
}

void C3DView::OnDetachCrossWalk(UINT nID)
{
	int nIndex=nID-ID_DETACH_CROSSWALK_START;
	CTermPlanDoc* pDoc = GetDocument();
	ASSERT(pDoc);
	if (pDoc == NULL)
		return;

	CPipeDataSet* pPipeDS = pDoc->GetTerminal().m_pPipeDataSet;
	int nPipeCount = pPipeDS->GetPipeCount();
	ASSERT(m_nSelectedPipe >=0 && m_nSelectedPipe < nPipeCount);
	CPipe* pPipe = pPipeDS->GetPipeAt(m_nSelectedPipe);
	ASSERT(pPipe);
	if (pPipe == NULL)
		return;
	pPipe->RemoveAttachCrosswalk(nIndex);
	pPipeDS->saveDataSet(pDoc->m_ProjInfo.path,true);
}


C3DView::~C3DView()
{
	//	delete m_pDlgMovie;
	delete m_pPickConveyorTree;

	if(m_pViewPBuffer)delete m_pViewPBuffer;
	if(m_pFloorPBuffer)delete m_pFloorPBuffer;

}