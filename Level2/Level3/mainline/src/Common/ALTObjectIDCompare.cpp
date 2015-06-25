#include "StdAfx.h"
#include ".\altobjectidcompare.h"
#include <common/ARCStringConvert.h>

bool ALTObjectIDCompareLess::operator()( const ALTObjectID& id1,const ALTObjectID& id2 )
{
	for(int i=0;i<OBJECT_STRING_LEVEL;i++ )	
	{
		CString s1 = id1.at(i).c_str();
		CString s2 = id2.at(i).c_str();
		if( s1.CompareNoCase(s2)==0 )
			continue;

		if( ARCStringConvert::isAllNumeric(s1.GetString()) && ARCStringConvert::isAllNumeric(s2.GetString()) )
		{
			int i1 = atoi(s1);
			int i2 = atoi(s2);
			return (i1<i2);					
		}

		s1.MakeLower();
		s2.MakeLower();
		return s1<s2;
	}
	return false;
}