#include "StdAfx.h"
#include ".\aircraftdeicingandanti_icingssettings.h"
#include "../Common/ProDistrubutionData.h"
#include "../Database/ADODatabase.h"
CPrecipitationtype::CPrecipitationtype():m_ID(-1),m_InspectionTime(NULL)
{
	m_InspectionTime = new CProDistrubution ;
}
CPrecipitationtype::~CPrecipitationtype()
{
	delete m_InspectionTime ;
}


void CPrecipitationtype::ReadData(int flightTyID , int conditionTy)
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_CPRECIPITATIONTYPE WHERE FLIGHTID = %d AND CONDITION_TYPE = %d AND TYPE = %d"),flightTyID,conditionTy,m_Type) ;
	CADORecordset dataset ;
	long cout = -1 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,cout,dataset) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	double time ; 
	int id ;
	while (!dataset.IsEOF())
	{
		dataset.GetFieldValue(_T("HOLDOVER_TIME"),time) ;
		SetHoldOverTime((float)time) ;

		dataset.GetFieldValue(_T("DE_ICETIME"),time) ;
		SetDeIceTime((float)time) ;

		dataset.GetFieldValue(_T("ANTI_ICETIME"),time) ;
		SetAntiIceTime((float)time) ;

		dataset.GetFieldValue(_T("ID"),id) ;
		SetID(id) ;

		dataset.GetFieldValue(_T("INSPECTION_TIME"),id) ;
		GetInspection()->setID(id) ;
		GetInspection()->ReadData(id) ;
		break ;
	}
}
void CPrecipitationtype::WriteData(int flightTyID , int conditionTy)
{
	if(flightTyID == -1)
		return ; 
	CString SQL ; 
	int id  = -1 ;
	GetInspection()->SaveData() ;
	try
	{
		if(GetID() == -1)
		{
			SQL.Format(_T("INSERT INTO AS_CPRECIPITATIONTYPE (HOLDOVER_TIME,INSPECTION_TIME,DE_ICETIME,ANTI_ICETIME,TYPE,CONDITION_TYPE,FLIGHTID) VALUES(%f,%d,%f,%f,%d,%d,%d)"),
				GetHoldOverTime(),
				GetInspection()->getID(),
				GetDeIceTime(),
				GetAntiIceTime(),
				m_Type,
				conditionTy,
				flightTyID) ;
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			SetID(id) ;

		}else
		{
			SQL.Format(_T("UPDATE AS_CPRECIPITATIONTYPE SET HOLDOVER_TIME = %f ,INSPECTION_TIME=%d, DE_ICETIME = %f , ANTI_ICETIME = %f WHERE ID = %d AND CONDITION_TYPE = %d AND FLIGHTID = %d"),
				GetHoldOverTime(),
				GetInspection()->getID() ,
				GetDeIceTime(),
				GetAntiIceTime(),
				GetID(),
				conditionTy,
				flightTyID) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}
}

static CString _strTypes[PREC_TYPECOUNT] = 
{
	_T("Non-freezing")
		,_T("Freezing drizzle")
		,_T("Light Snow")
		,_T("Medium Snow")
		,_T("Heavy snow") 
};
CString CPrecipitationtype::GetTypeString( Precipitationtype type )
{
	return _strTypes[type];
}

Precipitationtype CPrecipitationtype::GetStringType( const CString& str )
{
	for(int itrT = 0;itrT < PREC_TYPECOUNT;itrT++)
	{
		if(GetTypeString((Precipitationtype)itrT).CompareNoCase(str)==0)
			return (Precipitationtype)itrT;
	}
	return TY_Nonfreezing;
}

CNonfreezingType::CNonfreezingType():CPrecipitationtype()
{
	m_holdoverTime = 60 ;
	m_De_iceTime = 0 ;
	m_Anti_iceTime = 0 ;
	m_Type = TY_Nonfreezing ;
	m_Name.Format(_T("Non-freezing")) ;
}
CFreezingdrizzleType::CFreezingdrizzleType():CPrecipitationtype()
{
	m_holdoverTime = 15 ;
	m_De_iceTime = 0 ;
	m_Anti_iceTime = 1000 ;
	m_Type = TY_Freezingdrizzle ;
	m_Name.Format(_T("Freezing-drizzle")) ;
}
CLightSnowType::CLightSnowType():CPrecipitationtype()
{
	m_holdoverTime = 25 ;
	m_De_iceTime = 0 ;
	m_Anti_iceTime = 1000 ;
	m_Type = TY_LightSnow ;
	m_Name.Format(_T("Light-Snow")) ;
}
CMediumSnowType::CMediumSnowType():CPrecipitationtype()
{
	m_holdoverTime = 15 ;
	m_De_iceTime = 0 ;
	m_Anti_iceTime = 1500 ;
	m_Type = TY_MediumSnow ;
	m_Name.Format(_T("Medium-Snow")) ;
}
CHeavysnowType::CHeavysnowType():CPrecipitationtype()
{
	m_holdoverTime = 10 ;
	m_De_iceTime = 0 ;
	m_Anti_iceTime = 2000 ;
	m_Type = TY_Heavysnow ;
	m_Name.Format(_T("Heavy-snow")) ;
}
//////////////////////////////////////////////////////////////////////////
void CAircraftDeicingAndAnti_icingsSetting::ReadData()
{
	int flightid = GetFlightTypeID() ;
	m_ClearCondition.ReadData(flightid) ;
	m_LightFrost.ReadData(flightid) ;
	m_IceLayer.ReadData(flightid) ;
	m_Dry_powdersnow.ReadData(flightid) ;
	m_WetSnow.ReadData(flightid) ;
}
void CAircraftDeicingAndAnti_icingsSetting::SaveData()
{
	int flightid = GetFlightTypeID() ;
	m_ClearCondition.SaveData(flightid) ;
	m_LightFrost.SaveData(flightid) ;
	m_IceLayer.SaveData(flightid) ;
	m_Dry_powdersnow.SaveData(flightid) ;
	m_WetSnow.SaveData(flightid) ;
}
void CAircraftDeicingAndAnti_icingsSetting::DelData()
{
	CString SQL ;
	SQL.Format(_T("DELETE * FROM AS_CPRECIPITATIONTYPE WHERE FLIGHTID = %d") ,GetFlightTypeID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
}

CPrecipitationtype* CAircraftDeicingAndAnti_icingsSetting::GetData( AircraftSurfacecondition surf, Precipitationtype preT )
{
	switch (surf)
	{
	case TY_Clear : 
		return m_ClearCondition.GetData(preT);
	case TY_Light_Frost :
		return m_LightFrost.GetData(preT);
	case TY_Ice_layer :
		return m_IceLayer.GetData(preT);
	case TY_Dry_powder_snow :
		return m_Dry_powdersnow.GetData(preT);
	case TY_Wet_Snow:
		return m_WetSnow.GetData(preT);
	default:
		return NULL;		
	}

}
//////////////////////////////////////////////////////////////////////////
void CAircraftSurfacecondition::ReadData(int flightID)
{
	m_NonFreezing.ReadData(flightID,m_Type) ;
	m_FreezingDrizzle.ReadData(flightID,m_Type) ;
	m_LightSnow.ReadData(flightID,m_Type) ;
	m_MediumSnow.ReadData(flightID,m_Type) ;
	m_HeavySnow.ReadData(flightID,m_Type) ;
}
void CAircraftSurfacecondition::SaveData(int flightID)
{
	m_NonFreezing.WriteData(flightID,m_Type) ;
	m_FreezingDrizzle.WriteData(flightID,m_Type) ;
	m_LightSnow.WriteData(flightID,m_Type) ;
	m_MediumSnow.WriteData(flightID,m_Type) ;
	m_HeavySnow.WriteData(flightID,m_Type) ;
}



static CString _strSufaceTypes[ACSURCONDT_TYPECOUNT] = 
{
	_T("Clear"),
	_T("Light Frost"),
	_T("Ice layer"),
	_T("Dry/powder snow"),
	_T("Wet Snow"),
};
CString CAircraftSurfacecondition::GetTypeString( AircraftSurfacecondition type )
{
	return _strSufaceTypes[type];
}

AircraftSurfacecondition CAircraftSurfacecondition::GetStringType( const CString& str )
{
	for(int itrT = 0;itrT < ACSURCONDT_TYPECOUNT;itrT++)
	{
		if(GetTypeString((AircraftSurfacecondition)itrT).CompareNoCase(str)==0)
			return (AircraftSurfacecondition)itrT;
	}
	return TY_Clear;
}

CPrecipitationtype* CAircraftSurfacecondition::GetData( Precipitationtype preT )
{
	switch (preT)
	{
	case TY_Nonfreezing :
		return &m_NonFreezing;
	case TY_Freezingdrizzle :
		return &m_FreezingDrizzle;
	case TY_LightSnow : 
		return & m_LightSnow;
	case TY_MediumSnow :
		 return &m_MediumSnow;
	case TY_Heavysnow :
		return &m_HeavySnow;
	default :
		 return NULL;
	}
}
//////////////////////////////////////////////////////////////////////////

CAircraftDeicingAndAnti_icingsSettings::CAircraftDeicingAndAnti_icingsSettings(CAirportDatabase* _database):m_database(_database)
{

}
CAircraftDeicingAndAnti_icingsSettings::~CAircraftDeicingAndAnti_icingsSettings()
{
	ClearData() ;
	CLearDelData() ;
}
void CAircraftDeicingAndAnti_icingsSettings::ClearData()
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ;iter != m_Settings.end() ;iter++)
	{
		delete *iter ;
	}
	m_Settings.clear() ;
}
void CAircraftDeicingAndAnti_icingsSettings::CLearDelData()
{
	TY_SETTING_ITER iter = m_DelSettings.begin() ;
	for ( ; iter != m_DelSettings.end() ;iter++)
	{
		delete *iter ;
	}
	m_DelSettings.clear() ;
}
CAircraftDeicingAndAnti_icingsSetting* CAircraftDeicingAndAnti_icingsSettings::AddFlightTy(FlightConstraint _constraint)
{
	CAircraftDeicingAndAnti_icingsSetting* setting = new CAircraftDeicingAndAnti_icingsSetting ;
	setting->SetFlightTy(_constraint) ;
	m_Settings.push_back(setting) ;
	return setting ;
}
void CAircraftDeicingAndAnti_icingsSettings::DelFlightTy(CAircraftDeicingAndAnti_icingsSetting* setting)
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ; iter != m_Settings.end() ;iter++)
	{
		if((*iter) == setting)
		{
			m_DelSettings.push_back(*iter) ;
			m_Settings.erase(iter) ;
			return ;
		}
	}
}
CAircraftDeicingAndAnti_icingsSetting* CAircraftDeicingAndAnti_icingsSettings::FindSettingByFlightTy(FlightConstraint& _constraint) 
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ; iter != m_Settings.end() ;iter++)
	{
		if((*iter)->GetFlightTy()->isEqual(&_constraint))
		{
			return *iter;
		}
	}
	return NULL ;
}
CAircraftDeicingAndAnti_icingsSetting* CAircraftDeicingAndAnti_icingsSettings::FindSettingByFlightTy(int _flightTyID) 
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ; iter != m_Settings.end() ;iter++)
	{
		if((*iter)->GetFlightTypeID() == _flightTyID)
		{

			return *iter;
		}
	}
	return NULL ;
}
void CAircraftDeicingAndAnti_icingsSettings::ReadData()
{
	CString SQL ;
	SQL.Format(_T("SELECT * FROM AS_AIRCRAFT_DEICINGANDANI_ICING")) ;
	CADORecordset dataSet   ;
	long count = -1 ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL,count,dataSet) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
		return ;
	}
	CAircraftDeicingAndAnti_icingsSetting* setting = NULL ;
	CString flightTy ;
	int id = -1 ;
	while(!dataSet.IsEOF())
	{
		setting = new CAircraftDeicingAndAnti_icingsSetting ;
		dataSet.GetFieldValue(_T("FLIGHT_TYPE"),flightTy) ;
		setting->GetFlightTy()->SetAirportDB(m_database) ;
		setting->GetFlightTy()->setConstraintWithVersion(flightTy) ;

		dataSet.GetFieldValue(_T("ID"),id) ;
		setting->SetFlightTypeID(id) ;

		setting->ReadData() ;
		m_Settings.push_back(setting) ;

		dataSet.MoveNextData() ;
	}
}
void CAircraftDeicingAndAnti_icingsSettings::WriteData()
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ; iter != m_Settings.end() ;iter++)
	{
		WriteFLightTy(*iter) ;
		(*iter)->SaveData() ;
	}
	iter = m_DelSettings.begin() ;
	for ( ; iter != m_DelSettings.end() ;iter++)
	{
		DelFlightTyFromDB(*iter) ;
		(*iter)->DelData() ;
	}
	CLearDelData() ;
}
void CAircraftDeicingAndAnti_icingsSettings::WriteFLightTy(CAircraftDeicingAndAnti_icingsSetting* Setting)
{
	if(Setting == NULL)
		return ;
	CString SQL ;
	TCHAR flightTy[1024] = {0} ;
	Setting->GetFlightTy()->WriteSyntaxStringWithVersion(flightTy) ;
	int id = -1 ;
	try
	{
		if(Setting->GetFlightTypeID() == -1)
		{
			SQL.Format(_T("INSERT INTO AS_AIRCRAFT_DEICINGANDANI_ICING (FLIGHT_TYPE) VALUES('%s')"),flightTy) ;
			id = CADODatabase::ExecuteSQLStatementAndReturnScopeID(SQL) ;
			Setting->SetFlightTypeID(id) ;
		}else
		{
			SQL.Format(_T("UPDATE AS_AIRCRAFT_DEICINGANDANI_ICING SET FLIGHT_TYPE = '%s' WHERE ID = %d"),flightTy,Setting->GetFlightTypeID()
				) ;
			CADODatabase::ExecuteSQLStatement(SQL) ;
		}
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}

}
void CAircraftDeicingAndAnti_icingsSettings::DelFlightTyFromDB(CAircraftDeicingAndAnti_icingsSetting* Setting)
{
	if(Setting == NULL || Setting->GetFlightTypeID() == -1)
		return ;
	CString SQL ;
	SQL.Format(_T("DELETE * FROM AS_AIRCRAFT_DEICINGANDANI_ICING WHERE ID = %d") ,Setting->GetFlightTypeID()) ;
	try
	{
		CADODatabase::ExecuteSQLStatement(SQL) ;
	}
	catch (CADOException e)
	{
		e.ErrorMessage() ;
	}
}
void CAircraftDeicingAndAnti_icingsSettings::ResetData()
{
	TY_SETTING_ITER iter = m_Settings.begin() ;
	for ( ; iter != m_Settings.end() ;iter++)
	{
		m_DelSettings.push_back(*iter) ;
	}
	m_Settings.clear() ;
}

struct FlightTyOderLess
{
public:
	bool operator()(CAircraftDeicingAndAnti_icingsSetting* pSet1, CAircraftDeicingAndAnti_icingsSetting*pSet2)
	{
		 if(pSet1 && pSet2)
			 return pSet1->GetFlightTy()->fits(*pSet2->GetFlightTy())!=0;
		 return false;
	}
};

void CAircraftDeicingAndAnti_icingsSettings::SortByFlightTy()
{
	std::sort(m_Settings.begin(), m_Settings.end(), FlightTyOderLess() );
}