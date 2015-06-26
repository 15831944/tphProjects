#pragma once
#include <winuser.h>

enum TermPlanWinMsg
{
    // wParam = return data; pointer to vector of 3d values
    // lParam = lParam previously received (the reason)
    TP_DATA_BACK = WM_USER+1,

    // wParam = fallback data (type *FallbackData)
    // lParam = reason (type FallbackReason)
    TP_TEMP_FALLBACK,

    // wParam = index of selected item
    // lParam = empty
    WM_SLB_SELCHANGED,
    WM_AIR_SELCHANGED,
    WM_CABIN_SELCHANGED,
    WM_SIM_STATUS_MSG,
    WM_SIM_STATUS_FORMATEX_MSG,
    WM_SIM_STATUS_MSG2,
    WM_SIM_STATUS_EVENT,

    WM_SIM_STATUS_APPEND_DIAGNOSE,

    WM_GC_LCLICK,
    WM_GC_LDBCLICK,
    WM_LB_LCLICK,
    WM_INPLACE_SPIN,
    WM_INPLACE_DATETIME,
    WM_INPLACE_COMBO,
    WM_INPLACE_COMBO_KILLFOUCUS,
    WM_INPLACE_EDIT,
    WM_INPLACE_EDITSPIN,
    WM_END_EDIT,
    WM_PAXPROC_EDIT,
    //wParam = speed to set (ID_ANIMATION_SET1SPEED TO ID_ANIMATION_SET100SPEED)
    WM_SETANIMATIONSPEED,
    WM_UNSELECTABLELISTBOX_SELCHANGE,
    WM_UNSELECTABLELTREE_SELCHANGE,

    FCM_NOTIFYINSERT_KF, //wParam = insert time (long)
    FCM_NOTIFYDELETE_KF, //wParam = kf time (long)
    FCM_NOTIFYSELECT_TIME, //wParam = time (long), lParam = AutoGoto? (BOOL)
    FCM_NOTIFYMOVE_KF, //wParam = prev time (long), lParam = new time (long)

    TPWM_DESTROY_DLGWALKTHROUGH, 

    WM_EXPORT_MANUAL,
    WM_EXPORT_EMAIL,
    WM_INPLACE_COMBO2,
    WM_NOEDIT_DBCLICK,
    WM_CRASHCLOSE,

    WM_SIM_SORTEVENTLOG,
};
