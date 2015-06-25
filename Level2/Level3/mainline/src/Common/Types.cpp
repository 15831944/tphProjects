
#include "StdAfx.h"
#include "Types.hpp"

bool class_type::IsDeriveFrom( class_type* ct )const
{
	if(this == ct)
		return true;

	if(mplist.empty())
		return false;

	for(parentList_type::const_iterator itr=mplist.begin();itr!=mplist.end();++itr)
	{
		if( (*itr)->IsDeriveFrom(ct) )
			return true;
	}

	return false;
}

_regInherit::_regInherit( class_type* derive ,class_type* parent)
{
	if(derive!=parent)
		derive->mplist.insert(parent);
}
