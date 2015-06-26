#ifndef _ARCUNITMANAGER_HEADER_
#define _ARCUNITMANAGER_HEADER_
#include "../Common/CARCUnit.h"
#include "ARCUnitManagerOwnerEnum.h"
//owner
enum ARCUnitManagerOwnerID;
class CAirsideExportFile;
class CAirsideImportFile;

enum AFX_DATA_EXPORT ARCUnit_InUse //emuiu
{
	ARCUnit_ALL_InUse = 0xFFFFFFFF,//All
	ARCUnit_Length_InUse = 0x1,//Length
	ARCUnit_Weight_InUse = 0x2,//Weight
	ARCUnit_Time_InUse = 0x4,//Time
	ARCUnit_Velocity_InUse = 0x8,//Velocity
	ARCUnit_Acceleration_InUse = 0x10,//Acceleration
};
//Unit Manager
class AFX_CLASS_EXPORT CARCUnitBaseManager
{
public:
	//initialize order:
	CARCUnitBaseManager(void);
	virtual ~CARCUnitBaseManager(void);
public:
	//import/export with DB
	static void ExportARCUnit(CAirsideExportFile& exportFile);
	static void ImportARCUnit(CAirsideImportFile& importFile);
	static void ClearUnitManagerSettingInDB(int nProjID);
public:
	void SetUnitInUse(long lFlag);
	long GetUnitInUse(void);

	ARCUnit_Length GetCurSelLengthUnit(void) const;
	bool SetCurSelLengthUnit(ARCUnit_Length aulCurSelLength);

	ARCUnit_Weight GetCurSelWeightUnit(void) const;
	bool SetCurSelWeightUnit(ARCUnit_Weight auwCurSelWeight);

	ARCUnit_Time GetCurSelTimeUnit(void) const;
	bool SetCurSelTimeUnit(ARCUnit_Time autCurSelTime);

	ARCUnit_Velocity GetCurSelVelocityUnit(void) const;
	bool SetCurSelVelocityUnit(ARCUnit_Velocity auvCurSelVelocity);
	
	ARCUnit_Acceleration GetCurSelAccelerationUnit(void)const;
	bool SetCurSelAccelerationUnit(ARCUnit_Acceleration auaCurSelAcceleration);

	void SaveData(void);
	void ReadData(int nProjID,long lUnitUserID,long lDefaultGlobalUnitUserID = UM_ID_GLOBAL_DEFAULT);
protected:
	int m_nID;//id in DB
	int m_nProjID;//project id
	long m_lUnitUserID;//id for owner
	ARCUnit_Length m_emCurSelLengthUnit;//current selected length unit
	ARCUnit_Weight m_emCurSelWeightUnit; //current selected weight unit
	ARCUnit_Time m_emCurSelTimeUnit;//current selected time unit
	ARCUnit_Velocity m_emCurSelVelocityUnit;//current selected velocity unit 
	ARCUnit_Acceleration m_emCurSelAccelerationUnit;//current selected acceleration unit
	long m_lFlagUnitInUse;//flag show that unit in use by ARCUnit_InUse enum
};
class AFX_CLASS_EXPORT CARCUnitManager : public CARCUnitBaseManager
{
protected:
	//initialize order:
	CARCUnitManager();
	static CARCUnitManager* m_Instance ;
public:
	virtual ~CARCUnitManager(void);;
public:
	static CARCUnitManager& GetInstance() ;

};
#endif