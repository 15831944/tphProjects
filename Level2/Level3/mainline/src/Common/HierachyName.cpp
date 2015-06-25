#include "StdAfx.h"
#include ".\hierachyname.h"


void CHierachyName::fromString( const CString& s ,const CString&sep)
{
	mLevelNames.clear();
	CString strTemp = s;	
	if (!strTemp.CompareNoCase(GetAllString()) )
	{
		return;
	}


	int itr = 0;
	while(itr<s.GetLength())
	{
		CString slevel = s.Tokenize(sep,itr);
		mLevelNames.push_back(slevel);
	}
	
}

CString CHierachyName::toString(const CString& sep) const
{
	if(mLevelNames.empty()){
		return GetAllString();
	}
	CString sRet = getLevel(0);
	for(int i=1;i<getLevelCount();i++){		
		sRet+=sep;
		sRet+= getLevel(i);
	}
	return sRet;
}

bool CHierachyName::fitIn( const CHierachyName& othername ) const
{
	if(othername.isAll())
		return true;
	if(isAll())
		return false;
	if (othername.getLevelCount() > getLevelCount())
		return false;

	for (int i = 0; i < getLevelCount(); i++)
	{
		if (othername.getLevelCount() <= i)
			return true;
		if ( getLevel(i).CompareNoCase(othername.getLevel(i)) )//!=
			return false;
	}
	return true;
}

bool CHierachyName::fitIn( const CString& s ) const
{
	CHierachyName name;
	name.fromString(s);
	return fitIn(name);
}

bool CHierachyName::operator<( const CHierachyName& otherName ) const
{
	return toString()<otherName.toString();
}

bool CHierachyName::operator==( const CHierachyName& other ) const
{
	return toString()==other.toString();
}

CHierachyName& CHierachyName::PopFront()
{
	if(!mLevelNames.empty())
	{
		mLevelNames.erase(mLevelNames.begin());
	}
	return *this;
}

CHierachyName::CHierachyName( const CString& sname )
{
	fromString(sname);
}
