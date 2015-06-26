#pragma once
#include "InputAirsideAPI.h"

///////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API PadInfo
{
public:
	PadInfo();
	virtual ~PadInfo();

	void setObjectID(int nObjectID) { m_nObjectID = nObjectID;}
	int  getObjectID() { return m_nObjectID;}

	void setAissgnmentPercent(double AssignmentPercent) { m_AssignmentPercent = AssignmentPercent;}
	double getAissgnmentPercent() { return m_AssignmentPercent;}

	void setID(int nID) { m_nID = nID;}
	int getID()	{ return m_nID;}

	void setIndex(int nIndex) { m_nIndex = nIndex;}
	int getIndex() { return m_nIndex;}
	
	void SaveData(int nID);
	void ReadData(CADORecordset& adoRecordset);
	void DelData();
	void UpdateData();
protected:

private:
	int m_nObjectID;
	double m_AssignmentPercent;
	int m_nID;
	int m_nIndex;
};


////////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API FlightTypeInfo
{
public:
	FlightTypeInfo();
	virtual ~FlightTypeInfo();

	void setFlightType(CString strFlightType) { m_strFlightType = strFlightType;}
	CString getFlightType() { return m_strFlightType;}

	PadInfo* getPadInfo(int nObjectID);
	int getCount(){ return (int)m_vObject.size();}

	void setFlightTypeID(int nFlightID) { m_nFlightID = nFlightID;}
	int getFlightTypeID(){ return m_nFlightID;}

	void setPadID(int nPadID) { m_nPadID = nPadID;}
	int getPadID() { return m_nPadID;}

	void AddNewItem(PadInfo* pPadInfo);
	void DelItem(int nIndex);
	void DelItem(PadInfo* pPadInfo);
	PadInfo* getItem(int nIndex);
	void DelAllItem();

	void SaveData(int nFligtID);
	void ReadData();
	void ReadData(CADORecordset& adoRecordset);
	void DelData();
	void UpdateData();

	std::vector<PadInfo*> m_vObject;
	std::vector<PadInfo*> m_vDelObject;
protected:
private:
	CString m_strFlightType;
	int m_nFlightID;
	int m_nPadID;
};

////////////////////////////////////////////////////////////////////////////////////

class INPUTAIRSIDE_API WeatherInfo
{
public:
	WeatherInfo();
	virtual ~WeatherInfo();

	void setWeatherCondition( WEATHERCONDITION emWeatherCondition) { m_emWeatherCondition = emWeatherCondition;}
	WEATHERCONDITION getWeatherCondition() { return m_emWeatherCondition;}

	void setID(int nID) { m_nID = nID;}
	int getID() { return m_nID;}

	void setProjID(int nProjID) { m_nProjID = nProjID;}
	int getProjID() { return m_nProjID;}
	
	FlightTypeInfo* getFlightTypeByCString(CString strFlightType);
	int getCount() { return (int)m_vFlightType.size();}

	void AddNewItem(FlightTypeInfo* pFlightType);
	void DelItem(int nIndex);
	void DelItem(FlightTypeInfo* pFlightType);
	FlightTypeInfo* getItem(int nIndex);

	void SaveData(int nProjID);
	void ReadData();
	void ReadData(CADORecordset& adoRecordset);
	void DelData();
	void UpdateData();
	void InitNewData();

	std::vector<FlightTypeInfo*> m_vFlightType;
	std::vector<FlightTypeInfo*> m_vDelFlightType;
protected:
private:
	WEATHERCONDITION m_emWeatherCondition;
	int m_nID;
	int m_nProjID;
};

//////////////////////////////////////////////////////////////////////////////////////////
class INPUTAIRSIDE_API WeatherListInfo
{
public:
	WeatherListInfo();
	virtual ~WeatherListInfo();

	void setPorjID(int nProjID) { m_nProjID = nProjID;}
	int getProjID(){ return m_nProjID;}

	void AddNewItem(WeatherInfo* pWeather);
	void DelItem(int nIndex);
	WeatherInfo* getItem(int nIndex);
	WeatherInfo* getItemByCondition(WEATHERCONDITION emWeather);

	int getCount() { return (int)m_vWeather.size();}

	void SaveData(int nProjID);
	void ReadData(int nProjID);
	void InitNewData(WEATHERCONDITION emWeather);
protected:
private:
	int m_nProjID;

public:
	std::vector<WeatherInfo*> m_vWeather;
	std::vector<WeatherInfo*> m_vDelWeather;
};