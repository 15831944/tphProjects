#pragma once
#include "../Common/FLT_CNST.H"

#include "InputAirsideAPI.h"

enum Precipitationtype{ 
	 TY_Nonfreezing = 0
	,TY_Freezingdrizzle
	,TY_LightSnow
	,TY_MediumSnow
	,TY_Heavysnow
	//
	,PREC_TYPECOUNT
};
enum AircraftSurfacecondition{ 
	  TY_Clear=0
	 ,TY_Light_Frost 
	 ,TY_Ice_layer
	 ,TY_Dry_powder_snow 
	 ,TY_Wet_Snow
	 //
	 ,ACSURCONDT_TYPECOUNT
 };

class CProDistrubution;
class INPUTAIRSIDE_API CPrecipitationtype
{
public:
	CPrecipitationtype() ;
	virtual ~CPrecipitationtype() ;
public:
	//enum { TY_Nonfreezing , TY_Freezingdrizzle ,TY_LightSnow ,TY_MediumSnow ,TY_Heavysnow};
protected:
	int m_ID ;
	float m_holdoverTime ;
	float m_De_iceTime ;
	float m_Anti_iceTime ;
	int m_Type ;
	CProDistrubution* m_InspectionTime ;
    CString m_Name ; 
public:
	int GetID() { return m_ID ;} ;
	void SetID(int id) { m_ID = id ;} ;

	float GetHoldOverTime() { return m_holdoverTime ;} ;
	void SetHoldOverTime(float _val) { m_holdoverTime = _val ;} ;

	float GetDeIceTime() { return m_De_iceTime ;} ;
	void SetDeIceTime(float _val) { m_De_iceTime = _val ;} ;

	float GetAntiIceTime() { return m_Anti_iceTime ;} ;
	void SetAntiIceTime(float _val) { m_Anti_iceTime = _val ;} ;

    CProDistrubution* GetInspection() { return m_InspectionTime ;} ; 

	 void ReadData(int flightTyID , int conditionTy) ;
	 void WriteData(int flightTyID , int conditionTy) ;

	 CString GetName() { return m_Name ;} ;

	static CString GetTypeString(Precipitationtype type);
	static Precipitationtype GetStringType(const CString& str);
};


class INPUTAIRSIDE_API CNonfreezingType : public CPrecipitationtype
{
public:
     CNonfreezingType() ;
	 ~CNonfreezingType(){} ;
};

class INPUTAIRSIDE_API CFreezingdrizzleType : public CPrecipitationtype
{
public:
	CFreezingdrizzleType() ;
	~CFreezingdrizzleType() {};

};
class INPUTAIRSIDE_API CLightSnowType : public CPrecipitationtype
{
public:
	CLightSnowType() ;
	~CLightSnowType(){} ;

};
class INPUTAIRSIDE_API CMediumSnowType : public CPrecipitationtype
{
public:
	CMediumSnowType() ;
	~CMediumSnowType() {};

};
class INPUTAIRSIDE_API CHeavysnowType: public CPrecipitationtype
{
public:
	CHeavysnowType() ;
	~CHeavysnowType() {};

};
class INPUTAIRSIDE_API CAircraftSurfacecondition
{
public:
	CAircraftSurfacecondition() {};
	virtual ~CAircraftSurfacecondition() {};
	// enum { TY_Clear ,TY_Light_Frost ,TY_Ice_layer,TY_Dry_powder_snow ,TY_Wet_Snow};
protected:
     CNonfreezingType m_NonFreezing ;
	 CFreezingdrizzleType m_FreezingDrizzle ;
     CLightSnowType m_LightSnow ;
     CMediumSnowType m_MediumSnow ;
	 CHeavysnowType m_HeavySnow ;
	 int m_Type ;
	 CString m_Name ;
public:
	void ReadData(int flightID) ;
	void SaveData(int flightID) ;
public:
	CString GetName() { return  m_Name ;} ;
	CPrecipitationtype* GetNonFreezing() { return &m_NonFreezing ;} ;
	CPrecipitationtype* GetFreezingDrizzle() { return &m_FreezingDrizzle ;} ;
	CPrecipitationtype* GetLightSnow() { return &m_LightSnow ;} ;
	CPrecipitationtype* GetMediumSnow() { return &m_MediumSnow ;} ;
	CPrecipitationtype* GetHeavySnow() { return &m_HeavySnow ;} ;

	static CString GetTypeString(AircraftSurfacecondition type);
	static AircraftSurfacecondition GetStringType(const CString& str);

	CPrecipitationtype* GetData(Precipitationtype preT);
};

class INPUTAIRSIDE_API CClearCondition : public CAircraftSurfacecondition
{
public:
	CClearCondition(): CAircraftSurfacecondition() {  m_Type = TY_Clear ; m_Name.Format(_T("Clear")) ;} ;
	~CClearCondition(){} ;
};
class INPUTAIRSIDE_API CLightFrostCondition : public CAircraftSurfacecondition
{
public:
	CLightFrostCondition():CAircraftSurfacecondition() { m_Type = TY_Light_Frost ; m_Name.Format(_T("Light-Frost")) ; };
	~CLightFrostCondition() {};
};
class INPUTAIRSIDE_API CIcelayerCondition : public CAircraftSurfacecondition
{
public:
	CIcelayerCondition():CAircraftSurfacecondition() {  m_Type = TY_Ice_layer ;  m_Name.Format(_T("Ice-Layer")) ;} ;
	~CIcelayerCondition() {};
};
class INPUTAIRSIDE_API CDry_powdersnowCondition : public CAircraftSurfacecondition
{
public:
	CDry_powdersnowCondition():CAircraftSurfacecondition() { m_Type = TY_Dry_powder_snow ; m_Name.Format(_T("DryPowder-snow")) ; };
	~CDry_powdersnowCondition() {};
};
class INPUTAIRSIDE_API CWetSnowCondition : public CAircraftSurfacecondition
{
public:
	CWetSnowCondition():CAircraftSurfacecondition() { m_Type = TY_Wet_Snow ; m_Name.Format(_T("Wet-Snow")) ; }  ;
	~CWetSnowCondition() {};
};



class INPUTAIRSIDE_API CAircraftDeicingAndAnti_icingsSetting
{
public:
	CAircraftDeicingAndAnti_icingsSetting():m_FlightID(-1) {} ;
	~CAircraftDeicingAndAnti_icingsSetting() {};
protected:
	int m_FlightID ;
	FlightConstraint m_FlightTy ;
protected:
    CClearCondition m_ClearCondition ;
    CLightFrostCondition m_LightFrost ;
	CIcelayerCondition m_IceLayer ;
    CDry_powdersnowCondition m_Dry_powdersnow ;
    CWetSnowCondition m_WetSnow ;
public:
	int GetFlightTypeID() { return m_FlightID ;} ;
	void SetFlightTypeID(int _flightID) { m_FlightID = _flightID ;} ;
	CAircraftSurfacecondition* GetClearCondition() {return &m_ClearCondition ;} ;
	CAircraftSurfacecondition* GetLightFrostCondition() { return &m_LightFrost ;} ;
	CAircraftSurfacecondition* GetIceLayer() { return &m_IceLayer ;} ;
	CAircraftSurfacecondition* GetDry_powdersnowCondition() { return &m_Dry_powdersnow ;} ;
	CAircraftSurfacecondition* GetWetSnow() { return &m_WetSnow ;} ;
	FlightConstraint* GetFlightTy() { return &m_FlightTy ;} ;
	void SetFlightTy( FlightConstraint& flightTy)  {  m_FlightTy = flightTy ;} ;

	CPrecipitationtype* GetData(AircraftSurfacecondition surf, Precipitationtype preT);
public:
	void ReadData() ;
	void SaveData() ;
	void DelData() ;
};

class INPUTAIRSIDE_API CAircraftDeicingAndAnti_icingsSettings
{
public:
	CAircraftDeicingAndAnti_icingsSettings(CAirportDatabase* _database);
	~CAircraftDeicingAndAnti_icingsSettings(void);
	typedef std::vector<CAircraftDeicingAndAnti_icingsSetting*>  TY_SETTING ;
	typedef TY_SETTING::iterator TY_SETTING_ITER ;
protected:
	TY_SETTING m_Settings ; 
	TY_SETTING m_DelSettings ; 
	CAirportDatabase* m_database ;
public:

    void ReadData() ;
	void WriteData() ;
	CAircraftDeicingAndAnti_icingsSetting* AddFlightTy(FlightConstraint _constraint) ;
	void DelFlightTy(CAircraftDeicingAndAnti_icingsSetting* setting) ;
	TY_SETTING* GetDataSetting() { return &m_Settings ;} ;
   CAircraftDeicingAndAnti_icingsSetting* FindSettingByFlightTy(FlightConstraint& _constraint) ;
   CAircraftDeicingAndAnti_icingsSetting* FindSettingByFlightTy(int _flightTyID) ;
   void ResetData() ;
   void SortByFlightTy();
protected:
	void ClearData() ;
	void CLearDelData() ;

	void WriteFLightTy(CAircraftDeicingAndAnti_icingsSetting* Setting) ;
	void DelFlightTyFromDB(CAircraftDeicingAndAnti_icingsSetting* Setting) ; 

};
