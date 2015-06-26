#pragma  once
#include "../Common/tinyxml/tinyxml.h"
#include "SurfacesInitalEvent.h"
#include "../Common/AirportDatabase.h"
#include "../Common/FLT_CNST.H"
#include "InputAirsideAPI.h"

class INPUTAIRSIDE_API CSurfaceInitialXMLGenerator 
{
public:
	CSurfaceInitialXMLGenerator(CAirportDatabase* _AirportBase) ;
	~CSurfaceInitialXMLGenerator() ;
public:
	BOOL LoadFile(const CString& _fileName , CAircraftSurfaceSetting* _setting) ;
	BOOL SaveFile( CString& _fileName ,  CAircraftSurfaceSetting* _setting) ;
protected:
	 TiXmlDocument* m_document ;
     CAirportDatabase* m_AirportBase ;
protected:
	void WriteEventXMLNode(CSurfacesInitalEvent* _event) ;
	void WriteFlightTyXMLNode(CConditionItem* _condition ,  TiXmlElement* _element) ;
protected:
	void FormatEvent(TiXmlElement* _element , CAircraftSurfaceSetting* _setting) ;
	void FormatFlightTy(TiXmlElement* _element ,CSurfacesInitalEvent* _event) ;
};
