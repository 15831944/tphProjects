#ifndef _XIAOHUAPIECE_HEADER_
#define _XIAOHUAPIECE_HEADER_

#include "../InputAirside/ARCUnitManager.h"
#include "UnitButton.h"
#include "AirsideGUIAPI.h"

//小花的点心
#define XIAOHUABUTTON_SIZE_CX  48
#define XIAOHUABUTTON_SIZE_CY  22
#define XIAOHUABUTTON_ID 3324


class AIRSIDEGUI_API CUnitPiece
{
public:
	CUnitPiece(void);
	virtual ~CUnitPiece(void);
public:
	bool InitializeUnitPiece(int nProjID,long lUnitManagerID ,CWnd* pParentWnd);
	bool MoveUnitButton(int xPos,int yPos,int cxWidth = XIAOHUABUTTON_SIZE_CX,int cyHeight = XIAOHUABUTTON_SIZE_CY);
	bool IsInitialize(void);
public:
	 ARCUnit_Length GetCurSelLengthUnit(void) ;
	bool SetCurSelLengthUnit(ARCUnit_Length umlCurSelLength);

	ARCUnit_Weight GetCurSelWeightUnit(void) ;
	bool SetCurSelWeightUnit(ARCUnit_Weight umwCurSelWeight);

	ARCUnit_Time GetCurSelTimeUnit(void) ;
	bool SetCurSelTimeUnit(ARCUnit_Time umtCurSelTime);

	ARCUnit_Velocity GetCurSelVelocityUnit(void) ;
	bool SetCurSelVelocityUnit(ARCUnit_Velocity umvCurSelVelocity);

	ARCUnit_Acceleration GetCurSelAccelerationUnit(void);
	bool SetCurSelAccelerationUnit(ARCUnit_Acceleration auaCurSelAcceleration);

	void SaveData(void); 

	long GetUnitInUse(void);
public:
	virtual bool ConvertUnitFromDBtoGUI(void) = 0;
	virtual bool RefreshGUI(void) = 0;
	virtual bool ConvertUnitFromGUItoDB(void) = 0;
	virtual bool SetUnitInUse(long lFlag);



	double ConvertLength( double d );

	double UnConvertLength( double d );

protected:
	void Clear(void); 
protected:
	CARCUnitBaseManager * m_pumWindowUnits;
	CUnitButton * m_pwndUnitCmdButton;
private:
	bool m_bInitialize;
};

#endif