#include "stdafx.h"
#include ".\WeatherSciptXMLGenerator.h"
CWeatherSciptXMLGenerator::CWeatherSciptXMLGenerator():m_document(NULL)
{

}
CWeatherSciptXMLGenerator::~CWeatherSciptXMLGenerator()
{

}
BOOL CWeatherSciptXMLGenerator::LoadFile(CString& _fileName ,CWeatherScripts* _weatherScript)
{
	if(_weatherScript == NULL)
		return FALSE;
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_fileName) ;
	if(!m_document->LoadFile(_fileName))
	{
		MessageBox(NULL,_T("Load File Error"),_T("Error"),MB_OK) ;
		return FALSE ;
	}
	TiXmlElement* p_tootEle = m_document->RootElement() ;
	if(p_tootEle == NULL)
		return FALSE;
	_weatherScript->ReSetData() ;
	while (p_tootEle)
	{
		FormatEvent(p_tootEle,_weatherScript) ;
		p_tootEle = p_tootEle->NextSiblingElement() ;
	}
	delete m_document ;
	m_document = NULL ;
	return TRUE ;
}
BOOL CWeatherSciptXMLGenerator::SaveFile(CString& _fileName , CWeatherScripts* _weatherScript)
{
	if(_weatherScript == NULL)
		return FALSE ;
	BOOL res = FALSE ;
	if(m_document == NULL)
		m_document = new TiXmlDocument(_fileName) ;
	CWeatherScripts::TY_DATA_ITER iter = _weatherScript->GetData()->begin() ;
	for ( ; iter!= _weatherScript->GetData()->end() ;iter++)
	{
		WriteEventXMLNode(*iter) ;
	}
	if(m_document->SaveFile(_fileName))
		res = TRUE ;
	else
	{
		MessageBox(NULL,_T("Save File Name Error"),_T("Error"),MB_OK) ;
		res = FALSE ;
	}
	delete m_document ;
	m_document = NULL ;
	return res ;
}
void CWeatherSciptXMLGenerator::WriteEventXMLNode(CPrecipitationTypeEventItem* _eventItem)
{
	if(_eventItem == NULL)
		return ;
	TiXmlElement* EventNode = new TiXmlElement(_T("Event")) ;
	EventNode->SetAttribute(_T("Name"),_eventItem->GetName()) ;

	TiXmlElement* typenode = new TiXmlElement(_T("PrecipitationType")) ;
	typenode->SetAttribute(_T("Type") ,_eventItem->GetType() );

	//start time 
	CString strtime ;
	TiXmlElement* starttime = new TiXmlElement(_T("StartDateAndTime")) ;
	strtime.Format(_T("%d"),_eventItem->GetStartTime().asSeconds()) ;
	TiXmlText* time = new TiXmlText(strtime) ;
	starttime->LinkEndChild(time) ;

	TiXmlElement* endtime = new TiXmlElement(_T("EndDateAndTime")) ;
	strtime.Format(_T("%d"),_eventItem->GetEndTime().asSeconds()) ;
	time = new TiXmlText(strtime) ;
	endtime->LinkEndChild(time) ;


	typenode->LinkEndChild(starttime) ;
	typenode->LinkEndChild(endtime) ;

	EventNode->LinkEndChild(typenode) ;
	m_document->LinkEndChild(EventNode) ;
}
void CWeatherSciptXMLGenerator::FormatEvent(TiXmlElement* _element,CWeatherScripts* _weatherScript)
{
	if(_element == NULL || _weatherScript == NULL)
		return ;
	CString name ;
	name = _element->Attribute(_T("Name")) ;
	CPrecipitationTypeEventItem* _item =  _weatherScript->AddPrecipitationTypeEventItem(name) ;
	TiXmlElement* Preciptation =  _element->FirstChildElement(_T("PrecipitationType")) ;
	_item->SetType(Preciptation->Attribute(_T("Type")));

	TiXmlElement* startTime = Preciptation->FirstChildElement(_T("StartDateAndTime")) ;
	long val = atol(startTime->GetText()) ;
	_item->SetStartTime(ElapsedTime(val)) ;

	TiXmlElement* endTime = Preciptation->FirstChildElement(_T("EndDateAndTime")) ;
	val = atol(endTime->GetText()) ;
	_item->SetEndTime(ElapsedTime(val)) ;
}