#pragma once
#include "..\Inputs\IN_TERM.H"

#define UM_SHOW_CONTROL_BAR WM_USER+1000
#define UM_CHANGE_ANIM_TOOLBTN_STATE WM_USER+1001
#define UM_ANIMATION_PICK_SPEED WM_USER+1002
#define UM_SETTOOLBTN_RUNDELTA_RUN WM_USER+1003
#define UM_SETTOOLBTN_RUNDELTA_DELTA WM_USER+1004

//parameters to notify mainframe bar to update
class CTermPlanDoc;
class ToolBarUpdateParameters
{
public:
	ToolBarUpdateParameters()
	{
		m_operatType = UnKnown;
		m_nCurMode = EnvMode_Unknow;
		m_pDoc = NULL;
	};

	enum OPT_T 
	{ 
		UnKnown,
		OpenApplication,

		OpenProject,
		OpenComponentEditView,
		Open3DViewFrm,

		Close3DViewFrm,
		CloseComponentEditView,
		CloseProject,

		ChangeEnvMode,
	};	
	EnvironmentMode m_nCurMode;
	OPT_T m_operatType;
	CTermPlanDoc* m_pDoc;
};