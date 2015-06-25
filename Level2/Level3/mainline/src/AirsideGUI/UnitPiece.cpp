#include "stdafx.h"
#include "../AirsideGUI/Resource.h"
#include "UnitPiece.h"

CUnitPiece::CUnitPiece(void)
{
	m_pumWindowUnits = 0;
	m_pwndUnitCmdButton = 0;
	m_bInitialize = false;
}

CUnitPiece::~CUnitPiece(void)
{
	Clear();
}

void CUnitPiece::Clear(void)
{
	if(m_pumWindowUnits)
	{
		delete m_pumWindowUnits;
		m_pumWindowUnits = 0;
	}
	if(m_pwndUnitCmdButton)
	{
		delete m_pwndUnitCmdButton;
		m_pwndUnitCmdButton = 0;
	}
	m_bInitialize = false;
}

bool CUnitPiece::IsInitialize(void)
{
	return (m_bInitialize);
}

bool CUnitPiece::InitializeUnitPiece(int nProjID,long lUnitManagerID ,CWnd* pParentWnd)
{
	Clear();
	if(!pParentWnd || !pParentWnd->GetSafeHwnd())return (false);

	m_pumWindowUnits =  new CARCUnitBaseManager;
	if(!m_pumWindowUnits)return (false);
	m_pumWindowUnits->ReadData(nProjID,lUnitManagerID);

	m_pwndUnitCmdButton = new CUnitButton(this);
	if(!m_pwndUnitCmdButton)
	{
		delete m_pumWindowUnits;
		m_pumWindowUnits = 0;
		return (false);
	}
	if(!m_pwndUnitCmdButton->Create(_T("&Unit"),WS_TABSTOP|BS_CENTER|WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON ,CRect(0,0,0,0),pParentWnd,XIAOHUABUTTON_ID) )
	{
		Clear();
		return (false);
	}
	m_pwndUnitCmdButton->SetFont(pParentWnd->GetFont());
	m_pwndUnitCmdButton->SetAlwaysShowMenu(TRUE);
	m_bInitialize = true;
	return (true);
}

bool CUnitPiece::MoveUnitButton(int xPos,int yPos,int cxWidth /*= XIAOHUABUTTON_SIZE_CX*/,int cyHeight/* = XIAOHUABUTTON_SIZE_CY*/)
{
	if(!m_pwndUnitCmdButton || !m_pwndUnitCmdButton->GetSafeHwnd())return (false);
	m_pwndUnitCmdButton->MoveWindow(xPos,yPos,cxWidth,cyHeight);
	return (true);
}

ARCUnit_Length CUnitPiece::GetCurSelLengthUnit(void) 
{
	if(!IsInitialize())return (DEFAULT_DATABASE_LENGTH_UNIT);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->GetCurSelLengthUnit());
	else
		return (DEFAULT_DATABASE_LENGTH_UNIT);
}

bool CUnitPiece::SetCurSelLengthUnit(ARCUnit_Length umlCurSelLength)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->SetCurSelLengthUnit(umlCurSelLength));
	else
		return (false);
}

ARCUnit_Weight CUnitPiece::GetCurSelWeightUnit(void) 
{
	if(!IsInitialize())return (DEFAULT_DATABASE_WEIGHT_UNIT);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->GetCurSelWeightUnit());
	else
		return (DEFAULT_DATABASE_WEIGHT_UNIT);
}

bool CUnitPiece::SetCurSelWeightUnit(ARCUnit_Weight umwCurSelWeight)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->SetCurSelWeightUnit(umwCurSelWeight));
	else
		return (false);
}

ARCUnit_Time CUnitPiece::GetCurSelTimeUnit(void) 
{
	if(!IsInitialize())return (DEFAULT_DATABASE_TIME_UNIT);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->GetCurSelTimeUnit());
	else
		return (DEFAULT_DATABASE_TIME_UNIT);
}

bool CUnitPiece::SetCurSelTimeUnit(ARCUnit_Time umtCurSelTime)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->SetCurSelTimeUnit(umtCurSelTime));
	else
		return (false);
}

ARCUnit_Velocity CUnitPiece::GetCurSelVelocityUnit(void) 
{
	if(!IsInitialize())return (DEFAULT_DATABASE_VELOCITY_UNIT);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->GetCurSelVelocityUnit());
	else
		return (DEFAULT_DATABASE_VELOCITY_UNIT);
}

bool CUnitPiece::SetCurSelVelocityUnit(ARCUnit_Velocity umvCurSelVelocity)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->SetCurSelVelocityUnit(umvCurSelVelocity));
	else
		return (false);
}

ARCUnit_Acceleration CUnitPiece::GetCurSelAccelerationUnit(void)
{
	if(!IsInitialize())return (DEFAULT_DATABASE_ACCELERATION_UNIT);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->GetCurSelAccelerationUnit());
	else 
		return (DEFAULT_DATABASE_ACCELERATION_UNIT);
}

bool CUnitPiece::SetCurSelAccelerationUnit(ARCUnit_Acceleration auaCurSelAcceleration)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
		return (m_pumWindowUnits->SetCurSelAccelerationUnit(auaCurSelAcceleration));
	else
		return (false);
}

void CUnitPiece::SaveData(void)
{
	if(!IsInitialize())return ;
	if(m_pumWindowUnits)
		m_pumWindowUnits->SaveData();
}

bool CUnitPiece::SetUnitInUse(long lFlag)
{
	if(!IsInitialize())return (false);
	if(m_pumWindowUnits)
	{
		m_pumWindowUnits->SetUnitInUse(lFlag);
		return (true);
	}
	else
		return (false);
}

long CUnitPiece::GetUnitInUse(void)
{
	if(!IsInitialize())return (0x0L);
	if(m_pumWindowUnits)
	{
		return (m_pumWindowUnits->GetUnitInUse());
	}
	else
		return (0x0L);
}

double CUnitPiece::ConvertLength( double d )
{
	return CARCLengthUnit::ConvertLength(DEFAULT_DATABASE_LENGTH_UNIT,GetCurSelLengthUnit(),d);
}

double CUnitPiece::UnConvertLength( double d )
{
	return CARCLengthUnit::ConvertLength(GetCurSelLengthUnit(),DEFAULT_DATABASE_LENGTH_UNIT,d);
}