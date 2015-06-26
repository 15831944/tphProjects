#include "stdafx.h"
#include ".\CSurfaceInitialXMLGenerator.h"
CSurfaceInitialXMLGenerator::CSurfaceInitialXMLGenerator(CAirportDatabase* _AirportBase):m_document(NULL),m_AirportBase(_AirportBase)
{

}
CSurfaceInitialXMLGenerator::~CSurfaceInitialXMLGenerator()
{

}
BOOL CSurfaceInitialXMLGenerator::LoadFile(const CString& _fileName , CAircraftSurfaceSetting* _setting)
{
	if(_setting == NULL)
		return FALSE;
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_fileName) ;
	if(!m_document->LoadFile(_fileName))
	{
		MessageBox(NULL,_T("Load File Error"),_T("Error"),MB_OK) ;
		return FALSE ;
	}
	_setting->ReSetData() ;
	TiXmlElement* p_tootEle = m_document->RootElement() ;
	if(p_tootEle == NULL)
		return FALSE;
	while (p_tootEle)
	{
		FormatEvent(p_tootEle,_setting) ;
		p_tootEle = p_tootEle->NextSiblingElement() ;
	}
	return TRUE ;
}
void CSurfaceInitialXMLGenerator::FormatEvent(TiXmlElement* _element , CAircraftSurfaceSetting* _setting)
{
	if(_element == NULL || _setting == NULL)
		return ;

	CSurfacesInitalEvent* _event =  _setting->AddSurfacesInitalEvent(_element->Attribute("Name")) ;

	TiXmlElement* childElement = _element->FirstChildElement(_T("StartDateAndTime")) ;
	if(childElement == NULL)
		return ;
	double time = atof(childElement->GetText()) ;
	_event->SetStartTime(ElapsedTime(time)) ;

	childElement  = _element->FirstChildElement(_T("EndDateAndTime")) ;
	if(childElement == NULL)
		return ;
	time = atof(childElement->GetText()) ;
	_event->SetEndTime(ElapsedTime(time)) ;

	childElement = _element->FirstChildElement(_T("FlightTypes")) ;

	TiXmlElement* filghtTyElement = childElement->FirstChildElement() ;
	while(filghtTyElement)
	{
		FormatFlightTy(filghtTyElement,_event) ;
		filghtTyElement = filghtTyElement->NextSiblingElement() ;
	}
}
void CSurfaceInitialXMLGenerator::FormatFlightTy(TiXmlElement* _element ,CSurfacesInitalEvent* _event)
{
	if(_element == NULL || _event == NULL)
		return ;
	FlightConstraint _constraint ;
	_constraint.SetAirportDB(m_AirportBase) ;
	CString flightTY = _element->Attribute(_T("Value")) ;
	_constraint.setConstraintWithVersion(flightTY) ;
	CConditionItem* _conditionItem =  _event->AddConditionItem(_constraint) ;

	TiXmlElement* childElement = _element->FirstChildElement(_T("Condition")) ;
	_conditionItem->SetCondition(childElement->GetText()) ;
}
BOOL CSurfaceInitialXMLGenerator::SaveFile( CString& _fileName ,  CAircraftSurfaceSetting* _setting)
{
	if(_setting == NULL)
		return FALSE;
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_fileName) ;
	CAircraftSurfaceSetting::TY_DATA_ITER iter = _setting->GetData()->begin() ;
	for ( ; iter != _setting->GetData()->end() ; iter++)
	{
		WriteEventXMLNode(*iter) ;
	}
	if(!m_document->SaveFile(_fileName))
	{
		MessageBox(NULL,_T("Save File Error"),_T("Error"),MB_OK) ;
		res = FALSE ;
	}
	res = TRUE ;
	delete m_document ;
	m_document = NULL ;
	return res ;
}
void CSurfaceInitialXMLGenerator::WriteEventXMLNode(CSurfacesInitalEvent* _event)
{
	if(_event == NULL)
		return ;
	TiXmlElement* EventNode = new TiXmlElement(_T("Event")) ;
	EventNode->SetAttribute(_T("Name"),_event->GetName()) ;
	CString str ; 
	//start time
	CString strtime ;
	TiXmlElement* startTime = new TiXmlElement(_T("StartDateAndTime")) ;
	strtime.Format(_T("%d"),_event->GetStartTime().asSeconds()) ;
	TiXmlText* name = new TiXmlText(strtime) ;
	startTime->LinkEndChild(name) ;
	EventNode->LinkEndChild(startTime) ;

	//end time
	TiXmlElement* endTime = new TiXmlElement(_T("EndDateAndTime")) ;
	strtime.Format(_T("%d"),_event->GetEndTime().asSeconds()) ;
	 name = new TiXmlText(strtime) ;
	endTime->LinkEndChild(name) ;
	EventNode->LinkEndChild(endTime) ;

	TiXmlElement* flightTypes = new TiXmlElement(_T("FlightTypes")) ;
	EventNode->LinkEndChild(flightTypes) ;

	CSurfacesInitalEvent::TY_DATA_ITER iter = _event->GetConditions()->begin() ;
	for ( ; iter != _event->GetConditions()->end() ; iter++)
	{
		WriteFlightTyXMLNode(*iter,flightTypes) ;
	}
	m_document->LinkEndChild(EventNode) ;
}
void CSurfaceInitialXMLGenerator::WriteFlightTyXMLNode(CConditionItem* _condition , TiXmlElement* _element)
{
	if(_condition == NULL || _element == NULL)
		return ;
	TCHAR flightTY[1024] = {0} ;
	_condition->GetFlightType()->WriteSyntaxStringWithVersion(flightTY) ;
	TiXmlElement* flightType = new TiXmlElement(_T("FlightType")) ;
	flightType->SetAttribute(_T("Value"),flightTY) ;
	_element->LinkEndChild(flightType) ;

	TiXmlElement* flightTypeElement = new TiXmlElement(_T("Condition")) ;
	TiXmlText* conditiontext = new TiXmlText(_condition->GetCondition()) ;
	flightTypeElement->LinkEndChild(conditiontext) ;
	flightType->LinkEndChild(flightTypeElement) ;
}