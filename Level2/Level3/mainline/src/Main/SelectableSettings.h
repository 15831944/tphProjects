#pragma  once

#include "../InputAirside/ALT_BIN.h"
#include <map>
class SelectableSettings
{
public:

	SelectableSettings();
	std::map<ALTOBJECT_TYPE, BOOL> m_ALTobjectSelectableMap; 
	
	void SetSelectable(ALTOBJECT_TYPE objType, BOOL b);

};