#pragma once
#include "../Common/tinyxml/tinyxml.h"
#include "../InputAirside/AircraftDeicingAndAnti_icingsSettings.h"
#include "../Common/AirportDatabase.h"
class InputTerminal ;
class CProbDistEntry ;
class CDeicingAndAnticingsXMLGenerator
{
public:
	CDeicingAndAnticingsXMLGenerator(CAirportDatabase* _database);
	~CDeicingAndAnticingsXMLGenerator(void);
protected:
	 TiXmlDocument* m_document ;
	 CAircraftDeicingAndAnti_icingsSettings* m_Settings ;
	 CAirportDatabase* m_AirportDataBase ;
public:
	BOOL LoadFile(CString _FilePath,CAircraftDeicingAndAnti_icingsSettings&  _settings) ;
	BOOL SaveFile(CString _FilePath, CAircraftDeicingAndAnti_icingsSettings* _settings)  ;
protected:
	void FormatAircraftDeicingAndAnti_icingsSetting( CAircraftDeicingAndAnti_icingsSettings* _settings) ;

	void FormatFlightType(CAircraftDeicingAndAnti_icingsSetting* _setting) ;
	void FormatConditionType(CAircraftSurfacecondition* _condition,TiXmlElement* _element) ;
	void FormatPrecipitationType(CPrecipitationtype* _type,TiXmlElement* _element) ;
    CProbDistEntry*  GetProcdisIndex(CString name);
protected:
	void AddAircraftDeicingAndAnti_icingsSetting() ;
	void AddFlightType(TiXmlElement* _element) ;
	void AddConditionType(TiXmlElement* _element,CAircraftSurfacecondition* _setting) ;
	void AddPrecipitationType(TiXmlElement* _element,CPrecipitationtype* _type) ;
};
