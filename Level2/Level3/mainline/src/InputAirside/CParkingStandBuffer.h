#pragma  once 
#include "../common/flt_cnst.h" 
#include "altobjectgroup.h"
#include "AirsideImportExportManager.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API PSBArrivingFltType //arriving flight type
{
public:
	PSBArrivingFltType(CAirportDatabase *  _pAirportDatabase);
	~PSBArrivingFltType(void);

	FlightConstraint GetArrivingFltType(void);
public:
	CString	m_strFltTypeArriving;
	CAirportDatabase *  m_pAirportDatabase;
	FlightConstraint    m_fltconstrain;
	long m_lBufferTime;
};

class INPUTAIRSIDE_API PSBDepartingFltType //departing flight type
{
public:
	PSBDepartingFltType(CAirportDatabase *  _pAirportDatabase);
	~PSBDepartingFltType(void);
	void Clear(void);
	void AddArrivingFltType(FlightConstraint ArrFltType,long lBufferTime);
	void DeleteArrivingFltType(FlightConstraint ArrFltType);
	int UpdateArrivingFltType(FlightConstraint OldArrFltType,FlightConstraint NewArrFltType);//success return 0 

	long GetArrivingFltTypeCount(void);
	PSBArrivingFltType * GetArrivingFltType(long lIndex);
	FlightConstraint GetDepartingFltType(void);
public:
	CString	m_strFltTypeDeparting;
	CAirportDatabase *  m_pAirportDatabase;
	FlightConstraint    m_fltconstrain;
	std::vector < PSBArrivingFltType *> m_vrArrivingFltType;
};

class INPUTAIRSIDE_API PSBStdGrp //stand group
{
public:
	PSBStdGrp(CAirportDatabase *  _pAirportDatabase);
	PSBStdGrp(int nStdGrp,CAirportDatabase *  _pAirportDatabase);
	~PSBStdGrp(void);
	void Clear(void);
	void AddDepartingFltType(FlightConstraint DepFltType,FlightConstraint ArrFltType,long lBufferTime);
	int UpdateDepartingFltType(FlightConstraint OldDepFltType,FlightConstraint NewDepFltType);//success return 0 
	void DeleteDepartingFltType(FlightConstraint DepFltType);
	long GetDepartingFltTypeCount(void);
	PSBDepartingFltType * GetDepartingFltType(long lIndex);

	ALTObjectGroup GetStandGroup(void);
public:
	int m_nStandGroupID; 
	CAirportDatabase *  m_pAirportDatabase;
	std::vector < PSBDepartingFltType *> m_vrDepartingFltType;
};

class INPUTAIRSIDE_API CParkingStandBuffer
{
public:
	CParkingStandBuffer(CAirportDatabase *  _pAirportDatabase);
	CParkingStandBuffer(int nProjID,CAirportDatabase *  _pAirportDatabase);
	~CParkingStandBuffer(void);

	void ReadData(int nProjID);
	void SaveData(void);
	void ImportData(CAirsideImportFile& importFile);
	void ImportData_Old(CAirsideImportFile& importFile);
	void ExportData(CAirsideExportFile& exportFile);
	static void ExportParkingStandBuffer(CAirsideExportFile& exportFile,CAirportDatabase* pAirportDB);
	static void ImportParkingStandBuffer(CAirsideImportFile& importFile);	

	void AddStandGroup(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType,long lBufferTime = 0L);	
	int UpdateStandGroup(int nOldStdGrpID,int nNewStrGrpID);//success return 0
	int UpdateDepartingFltType(int nStdGrpID,FlightConstraint OldDepFltType,FlightConstraint NewDepFltType);//success return 0 
	int UpdateArrivingFltType(int nStdGrpID,FlightConstraint DepFltType, FlightConstraint OldArrFltType,FlightConstraint NewArrFltType);//success return 0 
	long GetStandBufferTime(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType);
	void DeleteStandGroup(int nStdGrpID);
	void DeleteDepartingFltType(int nStdGrpID,FlightConstraint DepFltType);
	void DeleteArrivingFltType(int nStdGrpID,FlightConstraint DepFltType,FlightConstraint ArrFltType);

	PSBDepartingFltType* GetDepartingFltType(int nStdGrpID,FlightConstraint DepFltType);

	long GetParkingStandgroupCount(void);
	PSBStdGrp * GetParkingStandgroup(long lIndex);
protected:	
	CString GetSelectScript(int nProjID) const;
	CString GetDeleteScript() const;
	void Clear(void);
protected:
	CAirportDatabase *  m_pAirportDatabase;
	int m_nProjID;
	std::vector < PSBStdGrp *> m_vrPushBackStandgroup;
};

//-----------------------------------------------------------------------------------------
//ParkingStandBufferList
// Facade pattern usage on CParkingStandBuffer
class INPUTAIRSIDE_API ParkingStandBufferList
{
public:
	ParkingStandBufferList(int nProjID,CAirportDatabase *  pAirportDatabase);
	~ParkingStandBufferList(void);

public:
	void LoadData();
	ElapsedTime GetBufferTime(FlightDescStruct& depFlight, FlightDescStruct& arrFlight);

protected:
	CAirportDatabase *  m_pAirportDatabase;
	int m_nProjID;
	CParkingStandBuffer* m_pParkingStandBuffer;

};














