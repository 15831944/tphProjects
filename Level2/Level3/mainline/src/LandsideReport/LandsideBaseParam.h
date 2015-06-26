#pragma once
#include "LandsideReportAPI.h"
#include "LandsideReportType.h"
#include "Common\elaptime.h"
#include "../Common/ALTObjectID.h"
#include "Common/HierachyName.h"

#include <vector>


class CAirportDatabase;
class ArctermFile;
class ProjectCommon;
class InputLandside;


#define LANDSIDE_ALLVEHICLETYPES -1
//#define LANDSIDE_ALLOBJECTS "LS_ALLOBJECTS"

class LANDSIDEREPORT_API LandsideBaseParam
{
public:
	LandsideBaseParam(void);
	virtual ~LandsideBaseParam(void);


public:
	void SetReportFileDir(const CString& strReportPath);
	virtual CString GetReportParamName() = 0;

	CString GetReportParamPath();

	void setSubType(int nSubType){m_nSubType = nSubType ;}
	int getSubType(){ return m_nSubType;}


	//the following functions are used in new structure
	//read and save data from the report path, in the project path
	void LoadParameterFile();
	void SaveParameterFile();

	//export and import parameter
	void LoadParameterFromFile(ArctermFile& _file);
	void SaveParameterToFile(ArctermFile& _file);

	//write the parameter data
	virtual void WriteParameter(ArctermFile& _file);
	virtual void ReadParameter(ArctermFile& _file);


public:
	static CString GetVehicleTypeName(InputLandside *pLandside, int nType);

public:

	ProjectCommon * GetCommonData() const;
	void SetCommonData(ProjectCommon * pCommon);


	void SetProjID(int nProiID) ;
	int GetProjID();

	ElapsedTime getStartTime(){ return m_startTime;}
	void setStartTime(ElapsedTime time){ m_startTime = time;}
	ElapsedTime getEndTime(){ return m_endTime;}
	void setEndTime(ElapsedTime time){ m_endTime = time;}
	long getInterval(){ return m_lInterval;}
	void setInterval(long  interval){ m_lInterval = interval;}

	//----------------------------------------------------------------------------------
	//Summary:
	//		set/get distance interval data
	//----------------------------------------------------------------------------------
	void setDistanceInterval(long lDistance) {m_lIntervalDistance = lDistance;}
	long getDistanceInterval()const {return m_lIntervalDistance;}

	void setReportType(LandsideReport_Detail_Summary rpType);
	LandsideReport_Detail_Summary getReportType();

	//-----------------------------------------------------------------------------------
	//Summary:
	//		construct onboard passenger type
	//-----------------------------------------------------------------------------------
	//	void SetInputTerminal(InputTerminal* pInputTerm);
	//----------------------------------------------------------------------------------
	//Summary:
	//		construct flight constrint for flight type
	//-----------------------------------------------------------------------------------
	void SetAirportDatabase(CAirportDatabase* pDatabase);
	CString GetParameterString();
	//-----------------------------------------------------------------------------------
	//Summary:
	//		check user whether input
	//Returns:
	//		true: does not define parameter
	//		false: define parameter
	//-----------------------------------------------------------------------------------
	
//Vehicle Type
	bool HasVehicleTypeDefined();

	int GetVehicleTypeCount() const;
	int GetVehicleType(int nIndex) const;
	void ClearVehicleType();
	bool AddVehicleType(int nType);//if duplicate, return false

	bool ContainVehicleType(int nType) const;


//From
	int GetFromObjectCount() const;
	ALTObjectID GetFromObject(int nIndex) const;
	void ClearFromObject();
	bool AddFromObject(ALTObjectID nObject);//if duplicate, return false

	bool ContainFromObject(ALTObjectID nObject) const;
//To Object

	int GetToObjectCount() const;
	ALTObjectID GetToObject(int nIndex) const;
	void ClearToObject();
	bool AddToObject(ALTObjectID nObject);//if duplicate, return false

	bool ContainToObject(ALTObjectID nObject) const;

	void AddFilterObject(const ALTObjectID& altObj);
	bool FilterObject(const ALTObjectID& altObj) const;
	int GetFilterObjectCount() const;
	ALTObjectID GetFilterObject( int nIndex ) const;
	void ClearFilterObject();

	void AddVehicleTypeName(const CHierachyName& vehicleName);
	int GetVehicleTypeNameCount()const;
	CHierachyName GetVehicleTypeName(int idx)const;
	bool FilterName(const CHierachyName& vehicleName)const;
	void ClearVehicleName();

	CHierachyName GetFilterName(const CHierachyName& vehicleName)const;
protected:


	ElapsedTime						m_startTime;
	ElapsedTime						m_endTime;
	long							m_lInterval;

	LandsideReport_Detail_Summary	m_reportType;

	int								m_nProjID;
	//chart type
	int								m_nSubType;
	CString							m_strReportPath;

	ProjectCommon*					m_pCommonData;

	long							m_lIntervalDistance;//interval for distance,queue length, int


	//Vehicle type list 
	std::vector<int>				m_vVehicleType;
	std::vector<CHierachyName> m_vVehicleTypeName;

	//From Object
	std::vector<ALTObjectID>		m_vFromObject;

	//To Object
	std::vector<ALTObjectID>		m_vToObjects;

	std::vector<ALTObjectID>		m_vObjectsFilter;



};
