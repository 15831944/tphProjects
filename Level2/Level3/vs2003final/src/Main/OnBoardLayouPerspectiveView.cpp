// OnBoardLayouPerspectiveView.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "OnBoardLayouPerspectiveView.h"
#include ".\onboardlayouperspectiveview.h"
#include "ViewMsg.h"
#include ".\getsectionnunmdlg.h"
#include ".\OnBoardGridOption.h"
#include "on_borad_helper.h"
#include "../common/AirportDatabase.h"
#include <InputOnboard/AircraftComponentModelDatabase.h>
#include "../InputOnBoard/cInputOnboard.h"
#include <common\Grid.h>

// COnBoardLayouPerspectiveView

IMPLEMENT_DYNCREATE(COnBoardLayouPerspectiveView, COnBoardLayoutView)

COnBoardLayouPerspectiveView::COnBoardLayouPerspectiveView()
{
	m_scene = 0;
}

COnBoardLayouPerspectiveView::~COnBoardLayouPerspectiveView()
{
}

BEGIN_MESSAGE_MAP(COnBoardLayouPerspectiveView, COnBoardLayoutView)
	ON_WM_CREATE()
	ON_WM_RBUTTONUP()
	ON_COMMAND(ID_ONBOARD_EDIT_MODE, OnPopupmenuonboradEditmode)
	ON_COMMAND(ID_ONBOARD_FRAME_MODE, OnPopupmenuonboradFramemode)
	ON_COMMAND(ID_ONBOARD_SOLID_MODE, OnPopupmenuonboradShadermode)
	ON_COMMAND(ID_ONBOARD_ADDSECTION, OnPopupmenuonboradCreatesection)
	ON_COMMAND(ID_POPUPMENUONBORAD_DELETESELECTSECTION, OnPopupmenuonboradDeleteselectsection)
	ON_COMMAND(ID_POPUPMENUONBORAD_DELETEALLSECTION, OnPopupmenuonboradDeleteallsection)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETSELECTEDSECTIONPOSITION, OnPopupmenuonboradSetselectedsectionposition)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETTEXTURENAME, OnPopupmenuonboradSettexturename)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETTEXTUREDIRX, OnPopupmenuonboradSettexturedirx)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETTEXTUREDIRY, OnPopupmenuonboradSettexturediry)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETTEXTUREDIRX34478, OnPopupmenuonboradSettexturedirz)
	ON_COMMAND(ID_POPUPMENUONBORAD_SETSECTIONPOINTNUM, OnPopupmenuonboradSetsectionpointnum)
	ON_COMMAND(ID_ONBOARD_MOVEHORITONTAL, OnPopupmenuonboradMoveprepxaxis)
	ON_COMMAND(ID_ONBOARD_MOVEVERTICAL, OnPopupmenuonboradMoveprepyaxis)
	ON_COMMAND(ID_ONBOARD_ROTATEY, OnPopupmenuonboradRotateyaxis)
	ON_COMMAND(ID_ONBOARD_ROTATEZ, OnPopupmenuonboradRotatezaxis)
	ON_COMMAND(ID_ONBOARD_ROTATEX, OnPopupmenuonboradRotatexaxis)
	ON_COMMAND(ID_POPUPMENUONBORAD_SCALEXAXIS, OnPopupmenuonboradScalexaxis)
	ON_COMMAND(ID_POPUPMENUONBORAD_SCALEYAXIS, OnPopupmenuonboradScaleyaxis)
	ON_COMMAND(ID_POPUPMENUONBORAD_SCALEALLXAXIS, OnPopupmenuonboradScaleallxaxis)
	ON_COMMAND(ID_POPUPMENUONBORAD_LOADCOMPONENT, OnPopupmenuonboradLoadcomponent)
	ON_COMMAND(ID_ONBOARD_SCALE, OnPopupmenuonboradScalezaxis)
	ON_COMMAND(ID_POPUPMENUONBORAD_LOCKUNLOCKEDITPOINT, OnPopupmenuonboradLockunlockeditpoint)
	ON_COMMAND(ID_POPUPMENUONBORAD_LOCKUNLOCKCOMPONET, OnPopupmenuonboradLockunlockcomponet)
	ON_COMMAND(ID_ONBOARD_GRID_SETTING, OnEditModelViewGrid)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_EDIT_MODE, OnUpdatePopupmenuonboradEditmode)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_FRAME_MODE, OnUpdatePopupmenuonboradFramemode)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_SOLID_MODE, OnUpdatePopupmenuonboradShadermode)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_MOVEHORITONTAL, OnUpdatePopupmenuonboradMoveprepxaxis)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_MOVEVERTICAL, OnUpdatePopupmenuonboradMoveprepyaxis)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_ROTATEX, OnUpdatePopupmenuonboradRotatexaxis)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_ROTATEY, OnUpdatePopupmenuonboradRotateyaxis)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_ROTATEZ, OnUpdatePopupmenuonboradRotatezaxis)
	ON_UPDATE_COMMAND_UI(ID_POPUPMENUONBORAD_SCALEXAXIS, OnUpdatePopupmenuonboradScalexaxis)
	ON_UPDATE_COMMAND_UI(ID_POPUPMENUONBORAD_SCALEYAXIS, OnUpdatePopupmenuonboradScaleyaxis)
	ON_UPDATE_COMMAND_UI(ID_POPUPMENUONBORAD_SCALEZAXIS, OnUpdatePopupmenuonboradScalezaxis)
	ON_UPDATE_COMMAND_UI(ID_ONBOARD_SCALE, OnUpdatePopupmenuonboradScaleallxaxis)
END_MESSAGE_MAP()


// COnBoardLayouPerspectiveView drawing

void COnBoardLayouPerspectiveView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// COnBoardLayouPerspectiveView diagnostics

#ifdef _DEBUG
void COnBoardLayouPerspectiveView::AssertValid() const
{
	COnBoardLayoutView::AssertValid();
}

void COnBoardLayouPerspectiveView::Dump(CDumpContext& dc) const
{
	COnBoardLayoutView::Dump(dc);
}
#endif //_DEBUG


// COnBoardLayouPerspectiveView message handlers

void COnBoardLayouPerspectiveView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if(this == pSender)
		return;
	on_update_view(m_scene, lHint, pHint);
}

int COnBoardLayouPerspectiveView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COnBoardLayoutView::OnCreate(lpCreateStruct) == -1)
		return -1;


	//create_scene("OnBoradWindow", "COnBoardLayouPerspectiveView", m_scene, this, ((CTermPlanDoc*)GetDocument())->m_doc_adapter);
	return 0;
}

LRESULT COnBoardLayouPerspectiveView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	scene_message(m_scene, message, wParam, lParam);
	return COnBoardLayoutView::WindowProc(message, wParam, lParam);
}

void COnBoardLayouPerspectiveView::OnInitialUpdate()
{
	//OnBoradACInfo* info = ((CTermPlanDoc*)GetDocument())->getARCport()->m_onborad_input->get_cur_info();
	//if(!info) return;
	//if(!info->m_3d_model_path.length()) return;
	//SAFE_CALL(m_scene)->load_from_file(info->m_3d_model_path.c_str());
}

void COnBoardLayouPerspectiveView::OnRButtonUp(UINT nFlags, CPoint point)
{
	TrackMenuHelper(IDR_MENU_POPUP_ONBORAD, point, this);
	COnBoardLayoutView::OnRButtonUp(nFlags, point);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradEditmode()
{
	SAFE_CALL(m_scene)->set_show_mode(EDIT_MODE);
	Invalidate(FALSE);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradFramemode()
{
	SAFE_CALL(m_scene)->set_show_mode(FRAME_MODE);
	Invalidate(FALSE);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradShadermode()
{
	SAFE_CALL(m_scene)->set_show_mode(SHADER_MODE);
	Invalidate(FALSE);
}
int nAddCnt = 0;
void COnBoardLayouPerspectiveView::OnPopupmenuonboradCreatesection()
{
	nAddCnt++;
	SAFE_CALL(m_scene)->create_section(nAddCnt*20.0f);
	nAddCnt%=10;
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradDeleteselectsection()
{
	SAFE_CALL(m_scene)->delete_select_obj();
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradDeleteallsection()
{
	SAFE_CALL(m_scene)->delete_all_section();
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSetselectedsectionposition()
{
	SAFE_CALL(m_scene)->set_select_pos(150);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSettexturename()
{
	SAFE_CALL(m_scene)->set_texture_name("test.bmp");
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSettexturedirx()
{
	SAFE_CALL(m_scene)->set_texture_wrap_dir(TD_X_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSettexturediry()
{
	SAFE_CALL(m_scene)->set_texture_wrap_dir(TD_Y_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSettexturedirz()
{
	SAFE_CALL(m_scene)->set_texture_wrap_dir(TD_Z_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradSetsectionpointnum()
{
	GetSectionNunmDlg dlg;
	dlg.DoModal();
	SAFE_CALL(m_scene)->set_section_point_num(dlg.m_point_num);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradMoveprepxaxis()
{
	SAFE_CALL(m_scene)->move(X_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradMoveprepyaxis()
{
	SAFE_CALL(m_scene)->move(Y_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradRotatexaxis()
{
	SAFE_CALL(m_scene)->rotate(X_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradRotateyaxis()
{
	SAFE_CALL(m_scene)->rotate(Y_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradRotatezaxis()
{
	SAFE_CALL(m_scene)->rotate(Z_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradScalexaxis()
{
	SAFE_CALL(m_scene)->scale(X_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradScaleyaxis()
{
	SAFE_CALL(m_scene)->scale(Y_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradScalezaxis()
{
	SAFE_CALL(m_scene)->scale(Z_AXIS);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradScaleallxaxis()
{
	SAFE_CALL(m_scene)->scale2();
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradLoadcomponent()
{
	CFileDialog dlg(TRUE,_T("xml"),("*.xml"),OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("xml(*.xml)|*.xml|"));
	if(dlg.DoModal() == IDCANCEL) return;
	CString path = dlg.GetPathName();
	CString name = dlg.GetFileTitle();
	force_cur_dir();
	if(!is_file_exsit(path.GetBuffer())) return;
	SAFE_CALL(m_scene)->create_componet_obj(path.GetBuffer(), name.GetBuffer());
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradLockunlockeditpoint()
{
	bool locked;
	SAFE_CALL_R(m_scene, locked)->get_lock_edit_point();
	SAFE_CALL(m_scene)->set_lock_edit_point(!locked);
}

void COnBoardLayouPerspectiveView::OnPopupmenuonboradLockunlockcomponet()
{
	bool locked;
	SAFE_CALL_R(m_scene, locked)->get_lock_componet_obj();
	SAFE_CALL(m_scene)->set_lock_componet_obj(!locked);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradEditmode(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_show_mode() == EDIT_MODE ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradFramemode(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_show_mode() == FRAME_MODE ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradShadermode(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_show_mode() == SHADER_MODE ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradMoveprepxaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == MOVE_OP  && m_scene->get_operation_axis() == X_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradMoveprepyaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == MOVE_OP && m_scene->get_operation_axis() == Y_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradRotatexaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == ROT_OP &&m_scene->get_operation_axis() == X_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradRotateyaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == ROT_OP && m_scene->get_operation_axis() == Y_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradRotatezaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == ROT_OP && m_scene->get_operation_axis() == Z_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradScalexaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == SCALE_OP && m_scene->get_operation_axis() == X_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradScaleyaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == SCALE_OP && m_scene->get_operation_axis() == Y_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradScalezaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == SCALE_OP && m_scene->get_operation_axis() == Z_AXIS ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnUpdatePopupmenuonboradScaleallxaxis(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	if( m_scene->get_operation_mode() == SCALE2_OP ) 
		pCmdUI->SetCheck(1);
	else
		pCmdUI->SetCheck(0);
}

void COnBoardLayouPerspectiveView::OnEditModelViewGrid()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)GetDocument();
	InputOnboard* pInputOnboard = pDoc->GetInputOnboard();
	bool bDataReady = pInputOnboard && pInputOnboard->GetAirportDB() && pInputOnboard->GetAirportDB()->getACCompModelDB();
	if(!bDataReady)
	{		
		return ;
	}
	/*CAircraftComponentModelDatabase* pModelDB = pInputOnboard->GetAirportDB()->getACCompModelDB();
	CAircraftComponentCategory* pActiveCat = pModelDB->GetCategory( pInputOnboard->GetActiveCategoryIndex() );
    if(!pActiveCat)return;
	CComponentMeshModel* pActiveModel = pActiveCat->GetModel( pInputOnboard->GetActiveModelIndex() ); 
	if(!pActiveModel)return;

	CModelGrid modegrid;
	COnBoardGridOption gridDlg(modegrid);
	if(gridDlg.DoModal() ==IDOK)
	{
		gridDlg.GetGridData(modegrid);
	}*/
}
