#include "StdAfx.h"
#include ".\deicingandanticingsxmlgenerator.h"
#include "../Common/ProDistrubutionData.h"
#include "../Common/ProbDistEntry.h"
#include "../Common/ProbDistManager.h"

CDeicingAndAnticingsXMLGenerator::CDeicingAndAnticingsXMLGenerator(CAirportDatabase* _database):m_document(NULL),m_AirportDataBase(_database)
{

}
CDeicingAndAnticingsXMLGenerator::~CDeicingAndAnticingsXMLGenerator()
{

}
BOOL CDeicingAndAnticingsXMLGenerator::LoadFile(CString _FilePath,CAircraftDeicingAndAnti_icingsSettings& _settings)
{
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_FilePath) ;
	if(!m_document->LoadFile(_FilePath) )
	{
		CString err ;
		err = m_document->ErrorDesc() ;
		int line = m_document->ErrorRow() ;
		delete m_document ;
		m_document = NULL ;
		return FALSE ;
	}
	_settings.ResetData() ;
	m_Settings = &_settings ;
	AddAircraftDeicingAndAnti_icingsSetting() ;
	delete m_document ;
	m_document = NULL ;
	m_Settings = NULL ;
	return TRUE ;
}
void CDeicingAndAnticingsXMLGenerator::AddAircraftDeicingAndAnti_icingsSetting()
{
	TiXmlElement* p_tootEle = m_document->RootElement() ;
	if(p_tootEle == NULL)
		return ;
	while (p_tootEle)
	{
		AddFlightType(p_tootEle) ;
		p_tootEle = p_tootEle->NextSiblingElement() ;
	}
}
void CDeicingAndAnticingsXMLGenerator::AddFlightType(TiXmlElement* _element)
{
	CAircraftDeicingAndAnti_icingsSetting* _setting = NULL ;
	FlightConstraint _constraint(m_AirportDataBase) ;
	CString flightTy ;
	flightTy = _element->Attribute(_T("FlightTy"));
	_constraint.setConstraintWithVersion(flightTy) ;
	_setting = m_Settings->AddFlightTy(_constraint) ;

	TiXmlElement* _conditionEle = _element->FirstChildElement(_setting->GetClearCondition()->GetName()) ;
	AddConditionType(_conditionEle,_setting->GetClearCondition()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetLightFrostCondition()->GetName()) ;
	AddConditionType(_conditionEle,_setting->GetLightFrostCondition()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetIceLayer()->GetName()) ;
	AddConditionType(_conditionEle,_setting->GetIceLayer()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetDry_powdersnowCondition()->GetName()) ;
	AddConditionType(_conditionEle,_setting->GetDry_powdersnowCondition()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetWetSnow()->GetName()) ;
	AddConditionType(_conditionEle,_setting->GetWetSnow()) ;
}
void CDeicingAndAnticingsXMLGenerator::AddConditionType(TiXmlElement* _element,CAircraftSurfacecondition* _setting)
{
	if(_element == NULL || _setting == NULL)
		return ;
	TiXmlElement* _conditionEle = _element->FirstChildElement(_setting->GetNonFreezing()->GetName()) ;
	AddPrecipitationType(_conditionEle,_setting->GetNonFreezing()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetFreezingDrizzle()->GetName()) ;
	AddPrecipitationType(_conditionEle,_setting->GetFreezingDrizzle()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetLightSnow()->GetName()) ;
	AddPrecipitationType(_conditionEle,_setting->GetLightSnow()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetMediumSnow()->GetName()) ;
	AddPrecipitationType(_conditionEle,_setting->GetMediumSnow()) ;

	_conditionEle = _element->FirstChildElement(_setting->GetHeavySnow()->GetName()) ;
	AddPrecipitationType(_conditionEle,_setting->GetHeavySnow()) ;
}
CProbDistEntry*  CDeicingAndAnticingsXMLGenerator::GetProcdisIndex(CString name)
{
	int ndx = m_AirportDataBase->getProbDistMan()->getCount() ;
	CProbDistEntry* pde  = NULL ;
	for ( int i = 0 ; i < ndx ;i++)
	{
        pde =  m_AirportDataBase->getProbDistMan()->getItem(i); 
		if(strcmp(name,pde->m_csName) == 0)
			return pde ;
	}
	return NULL ;
	
}
void CDeicingAndAnticingsXMLGenerator::AddPrecipitationType(TiXmlElement* _element,CPrecipitationtype* _type)
{
	if(_element == NULL || _type == NULL)
		return ;
	TiXmlElement* holdEle  = _element->FirstChildElement(_T("HoldoverTime")) ;
	_type->SetHoldOverTime( (float)atof(holdEle->GetText()) ) ;

	holdEle = _element->FirstChildElement(_T("InspectionTime")) ;
	CProbDistEntry* pdx = GetProcdisIndex(holdEle->GetText()) ;
	if(pdx != NULL)
	{
	  _type->GetInspection()->SetProDistrubution(pdx) ;
	  _type->GetInspection()->SaveData() ;
	}
	holdEle = _element->FirstChildElement(_T("De-icefluid")) ;
	_type->SetDeIceTime((float)atof(holdEle->GetText())) ;

	holdEle = _element->FirstChildElement(_T("Anti-icefluid")) ;
	_type->SetAntiIceTime((float)atof(holdEle->GetText())) ;
}
BOOL CDeicingAndAnticingsXMLGenerator::SaveFile(CString _FilePath, CAircraftDeicingAndAnti_icingsSettings* _settings)  
{
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_FilePath) ;
	FormatAircraftDeicingAndAnti_icingsSetting(_settings);
	res = m_document->SaveFile(_FilePath) ;
	delete m_document ;
	m_document = NULL ;
	return res ;
}
void CDeicingAndAnticingsXMLGenerator::FormatAircraftDeicingAndAnti_icingsSetting(CAircraftDeicingAndAnti_icingsSettings* _settings)
{
	CAircraftDeicingAndAnti_icingsSettings::TY_SETTING_ITER iter = _settings->GetDataSetting()->begin() ;
	for ( ; iter != _settings->GetDataSetting()->end() ;iter++)
	{
		FormatFlightType(*iter) ;
	}
}
void CDeicingAndAnticingsXMLGenerator::FormatFlightType(CAircraftDeicingAndAnti_icingsSetting* _setting)
{
	TCHAR flightTy[1024] = {0} ;
	_setting->GetFlightTy()->WriteSyntaxStringWithVersion(flightTy) ;
	TiXmlElement* FlightTYNode = new TiXmlElement(_T("FlightType")) ;
	FlightTYNode->SetAttribute(_T("FlightTy"),flightTy) ;
	m_document->LinkEndChild(FlightTYNode) ;

	FormatConditionType(_setting->GetClearCondition(),FlightTYNode) ;

	FormatConditionType(_setting->GetLightFrostCondition(),FlightTYNode) ;

	FormatConditionType(_setting->GetIceLayer(),FlightTYNode) ;

	FormatConditionType(_setting->GetDry_powdersnowCondition(),FlightTYNode) ;

	FormatConditionType(_setting->GetWetSnow(),FlightTYNode) ;

}
void CDeicingAndAnticingsXMLGenerator::FormatConditionType(CAircraftSurfacecondition* _condition,TiXmlElement* _element) 
{
	TiXmlElement* ConditionTYNode = new TiXmlElement(_condition->GetName()) ;
	_element->LinkEndChild(ConditionTYNode) ;

	FormatPrecipitationType(_condition->GetNonFreezing(),ConditionTYNode) ;

	FormatPrecipitationType(_condition->GetFreezingDrizzle() ,ConditionTYNode) ;

	FormatPrecipitationType(_condition->GetLightSnow() ,ConditionTYNode) ;

	FormatPrecipitationType(_condition->GetMediumSnow() ,ConditionTYNode) ;

	FormatPrecipitationType(_condition->GetHeavySnow(),ConditionTYNode) ;
}
void CDeicingAndAnticingsXMLGenerator::FormatPrecipitationType(CPrecipitationtype* _type,TiXmlElement* _element)
{
	TiXmlElement* PrecipitationTYNode = new TiXmlElement(_type->GetName()) ;
	_element->LinkEndChild(PrecipitationTYNode) ;

	CString str ; 
	str.Format(_T("%0.2f"),_type->GetHoldOverTime()) ;
	TiXmlElement* HoldTime = new TiXmlElement(_T("HoldoverTime")) ;
	TiXmlText* name = new TiXmlText(str) ;
	HoldTime->LinkEndChild(name) ;

	str.Format(_T("%s"),_type->GetInspection()->getDistName()) ;
	TiXmlElement* InSpectionTime = new TiXmlElement(_T("InspectionTime")) ;
	name = new TiXmlText(str) ;
	InSpectionTime->LinkEndChild(name) ;

	str.Format(_T("%0.2f"),_type->GetDeIceTime()) ;
	TiXmlElement* listfluid = new TiXmlElement(_T("De-icefluid")) ;
	name = new TiXmlText(str) ;
	listfluid->LinkEndChild(name) ;

	str.Format(_T("%0.2f"),_type->GetAntiIceTime()) ;
	TiXmlElement*  Antilistfluid = new TiXmlElement(_T("Anti-icefluid")) ;
	name = new TiXmlText(str) ;
	Antilistfluid->LinkEndChild(name) ;

	PrecipitationTYNode->LinkEndChild(HoldTime) ;
	PrecipitationTYNode->LinkEndChild(InSpectionTime) ;
	PrecipitationTYNode->LinkEndChild(listfluid) ;
	PrecipitationTYNode->LinkEndChild(Antilistfluid) ;

}
