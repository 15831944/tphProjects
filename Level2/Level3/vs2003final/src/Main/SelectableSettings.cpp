#include "StdAfx.h"

#include "SelectableSettings.h"



void SelectableSettings::SetSelectable( ALTOBJECT_TYPE objType, BOOL b )
{
	if(objType == ALT_UNKNOWN)
	{
		for(int iType = ALT_UNKNOWN +1 ; iType < ALT_ATYPE_END; iType ++ )
		{
			m_ALTobjectSelectableMap[(ALTOBJECT_TYPE)iType] = b;
		}
	}
	else
	{
		m_ALTobjectSelectableMap[objType] = b;
	}
}

SelectableSettings::SelectableSettings()
{
	for(int iType = ALT_UNKNOWN +1 ; iType < ALT_ATYPE_END; iType ++ )
	{
		m_ALTobjectSelectableMap[(ALTOBJECT_TYPE)iType] = TRUE;
	}
}