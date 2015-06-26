#pragma once
#include "../Common/tinyxml/tinyxml.h"
#include "WeatherScripts.h"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CWeatherSciptXMLGenerator
{
public:
	CWeatherSciptXMLGenerator() ;
	~CWeatherSciptXMLGenerator() ;
public:
	BOOL LoadFile(CString& _fileName ,CWeatherScripts* _weatherScript) ;
	BOOL SaveFile(CString& _fileName , CWeatherScripts* _weatherScript) ;
protected:
	TiXmlDocument* m_document ;
protected:
	void WriteEventXMLNode(CPrecipitationTypeEventItem* _eventItem) ;
	void FormatEvent(TiXmlElement* _element,CWeatherScripts* _weatherScript) ;
};
