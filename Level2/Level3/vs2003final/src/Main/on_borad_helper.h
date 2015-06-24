#pragma once

//#include "../on_borad_impl/arc_engine_apdater.h"
//#include "../on_borad_impl/view_cmd.h"
#include "termplandoc.h"
#include "OnBoradInput.h"

//****************************************************************

template<class T>
inline void create_scene(const char* type, const char* name, T*& scene, CView* view, DocAdapter* doc)
{
	if(!type || !name || !view) return;
	CHLOE_SYSTEM_R2(ARCRSystem, scene, T*)->create_scene(name, type);
	SAFE_CALL(scene)->set_view_doc_adapter(view, doc);
	SAFE_CALL(scene)->create_device(view->m_hWnd);
	SAFE_CALL(scene)->message_proc(WM_CREATE, 0, 0);
}

//****************************************************************

template<class T>
inline void scene_message(T*& scene, UINT message, WPARAM wParam=0, LPARAM lParam=0)
{
	SAFE_CALL(scene)->message_proc(message, wParam, lParam);
	if(message == WM_DESTROY) 
	{
		CHLOE_SYSTEM(ARCRSystem)->destory_scene(scene);
		scene = 0;
	}
}

//****************************************************************

template<class T>
inline void on_update_view(T* scene, LPARAM lHint, CObject* pHint)
{
	if(!is_onborad_define_msg(lHint)) return;
	int* data = (int*)pHint;
	SAFE_CALL(scene)->message_proc((UINT)lHint, (WPARAM)data[0], (LPARAM)data[1]);
}

//****************************************************************

inline void TrackMenuHelper(int id, CPoint point, CView* view)
{
	if(GetAsyncKeyState(VK_MENU) & 0x8000) return;
	view->ClientToScreen(&point);
	CNewMenu menu;
	menu.LoadMenu(id);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, point.x, point.y, view);
}

//****************************************************************