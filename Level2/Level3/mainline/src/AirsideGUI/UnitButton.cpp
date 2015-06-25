//UnitButton.cpp : implementation file
//

#include "stdafx.h" 
#include "Resource.h"
#include "UnitPiece.h"
#include "UnitButton.h"
#include ".\unitbutton.h"

// CUnitButton
const int ID_FIRST_RANGE_START = 1;
const int ID_SECOND_RANGE_START = 1001;
const int ID_THIRD_RANGE_START = 2001;
const int ID_FOURTH_RANGE_START = 3001;
const int ID_FIFTH_RANGE_START = 4001;
const int ID_SIXTH_RANGE_START = 5001;

IMPLEMENT_DYNAMIC(CUnitButton, CButton)
CUnitButton::CUnitButton(CUnitPiece * pParent/* = 0*/):m_pParent(pParent)
{	
}

CUnitButton::~CUnitButton()
{
}


BEGIN_MESSAGE_MAP(CUnitButton, CButton)
	ON_WM_LBUTTONDOWN()
	ON_COMMAND_RANGE(ID_FIRST_RANGE_START,ID_SECOND_RANGE_START-1,OnUnitSettingFirstRange)
	ON_COMMAND_RANGE(ID_SECOND_RANGE_START,ID_THIRD_RANGE_START-1,OnUnitSettingSecondRange)
	ON_COMMAND_RANGE(ID_THIRD_RANGE_START,ID_FOURTH_RANGE_START-1,OnUnitSettingThirdRange)
	ON_COMMAND_RANGE(ID_FOURTH_RANGE_START,ID_FIFTH_RANGE_START-1,OnUnitSettingFourthRange)
	ON_COMMAND_RANGE(ID_FIFTH_RANGE_START,ID_SIXTH_RANGE_START-1,OnUnitSettingFifthRange)
END_MESSAGE_MAP()



// CUnitButton message handlers
void CUnitButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(!m_pParent)return;
	long lFlag = m_pParent->GetUnitInUse();
	
	m_menu.DestroyMenu();
	m_menu.CreatePopupMenu();
	CMenu subMenu;
	int i = -1;
	int nArraySize = -1;
	UINT nMenuFlags = 0;
	CStringArray ayUnitStr;
	{
		//Length Unit
		if(lFlag & ARCUnit_Length_InUse)
		{
			subMenu.CreatePopupMenu();
			CARCLengthUnit::EnumLengthUnitString(ayUnitStr);
			nArraySize = ayUnitStr.GetCount();
			for(i = 0; i < nArraySize; i++)
			{
				nMenuFlags = MF_STRING;
				if(i == (int)m_pParent->GetCurSelLengthUnit())
					nMenuFlags = MF_STRING | MF_CHECKED;
				else
					nMenuFlags = MF_STRING;
				subMenu.AppendMenu(nMenuFlags,ID_FIRST_RANGE_START + i,ayUnitStr[i]);
			}
			m_menu.AppendMenu(MF_POPUP,(UINT_PTR)subMenu.m_hMenu,_T("Length Unit"));
			ayUnitStr.RemoveAll();
			subMenu.DestroyMenu();
		}
		if(lFlag & ARCUnit_Weight_InUse)
		{
			//Weight Unit			
			subMenu.CreatePopupMenu();
			CARCWeightUnit::EnumWeightUnitString(ayUnitStr);
			nArraySize = ayUnitStr.GetCount();
			for(i = 0; i < nArraySize; i++)
			{
				nMenuFlags = MF_STRING;
				if(i == (int)m_pParent->GetCurSelWeightUnit())
					nMenuFlags = MF_STRING | MF_CHECKED;
				else
					nMenuFlags = MF_STRING;
				subMenu.AppendMenu(nMenuFlags,ID_SECOND_RANGE_START + i,ayUnitStr[i]);
			}
			m_menu.AppendMenu(MF_POPUP,(UINT_PTR)subMenu.m_hMenu,_T("Weight Unit"));
			ayUnitStr.RemoveAll();
			subMenu.DestroyMenu();
		}
		if(lFlag & ARCUnit_Time_InUse)
		{
			//Time Unit
			subMenu.CreatePopupMenu();
			CARCTimeUnit::EnumTimeUnitString(ayUnitStr);
			nArraySize = ayUnitStr.GetCount();
			for(i = 0; i < nArraySize; i++)
			{
				nMenuFlags = MF_STRING;
				if(i == (int)m_pParent->GetCurSelTimeUnit())
					nMenuFlags = MF_STRING | MF_CHECKED;
				else
					nMenuFlags = MF_STRING;
				subMenu.AppendMenu(nMenuFlags,ID_THIRD_RANGE_START + i,ayUnitStr[i]);
			}
			m_menu.AppendMenu(MF_POPUP,(UINT_PTR)subMenu.m_hMenu,_T("Time Unit"));
			ayUnitStr.RemoveAll();
			subMenu.DestroyMenu();
		}
		if(lFlag & ARCUnit_Velocity_InUse)
		{
			//Velocity Unit
			subMenu.CreatePopupMenu();
			CARCVelocityUnit::EnumVelocityUnitString(ayUnitStr);
			nArraySize = ayUnitStr.GetCount();
			for(i = 0; i < nArraySize; i++)
			{
				nMenuFlags = MF_STRING;
				if(i == (int)m_pParent->GetCurSelVelocityUnit())
					nMenuFlags = MF_STRING | MF_CHECKED;
				else
					nMenuFlags = MF_STRING;
				subMenu.AppendMenu(nMenuFlags,ID_FOURTH_RANGE_START + i,ayUnitStr[i]);
			}
			m_menu.AppendMenu(MF_POPUP,(UINT_PTR)subMenu.m_hMenu,_T("Velocity Unit"));
			ayUnitStr.RemoveAll();
			subMenu.DestroyMenu();
		}
		if(lFlag & ARCUnit_Acceleration_InUse)
		{
			//Acceleration Unit
			subMenu.CreatePopupMenu();
			CARCAccelerationUnit::EnumAccelerationUnitString(ayUnitStr);
			nArraySize = ayUnitStr.GetCount();
			for(i = 0; i < nArraySize; i++)
			{
				nMenuFlags = MF_STRING;
				if(i == (int)m_pParent->GetCurSelAccelerationUnit())
					nMenuFlags = MF_STRING | MF_CHECKED;
				else
					nMenuFlags = MF_STRING;
				subMenu.AppendMenu(nMenuFlags,ID_FIFTH_RANGE_START + i,ayUnitStr[i]);
			}
			m_menu.AppendMenu(MF_POPUP,(UINT_PTR)subMenu.m_hMenu,_T("Acceleration Unit"));
			ayUnitStr.RemoveAll();
			subMenu.DestroyMenu();
		}
	} 
	CUnitMenuButton::OnLButtonDown(nFlags, point);
}

void CUnitButton::OnUnitSettingFirstRange(UINT nID)
{
	if(!m_pParent)return;
	m_pParent->SetCurSelLengthUnit(ARCUnit_Length(nID - ID_FIRST_RANGE_START));
	UpdataSetting();
}

void CUnitButton::OnUnitSettingSecondRange(UINT nID)
{
	if(!m_pParent)return;
	m_pParent->SetCurSelWeightUnit(ARCUnit_Weight(nID - ID_SECOND_RANGE_START));
	UpdataSetting();
}

void CUnitButton::OnUnitSettingThirdRange(UINT nID)
{
	if(!m_pParent)return;
	m_pParent->SetCurSelTimeUnit(ARCUnit_Time(nID - ID_THIRD_RANGE_START));
	UpdataSetting();
}

void CUnitButton::OnUnitSettingFourthRange(UINT nID)
{
	if(!m_pParent)return;
	m_pParent->SetCurSelVelocityUnit(ARCUnit_Velocity(nID - ID_FOURTH_RANGE_START));
	UpdataSetting();
}

void CUnitButton::OnUnitSettingFifthRange(UINT nID)
{
	if(!m_pParent)return;
	m_pParent->SetCurSelAccelerationUnit(ARCUnit_Acceleration(nID - ID_FIFTH_RANGE_START));
	UpdataSetting();
}

void CUnitButton::UpdataSetting(void)
{
	if(m_pParent)
	{
		m_pParent->SaveData();
		m_pParent->RefreshGUI();
	}
}