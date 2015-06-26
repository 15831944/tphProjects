// ResourceElementLogEntry.cpp: implementation of the ResourceElementLogEntry class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourceElementLogEntry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ResourceElementLogEntry::ResourceElementLogEntry()
						:TerminalLogItem<ResDescStruct,ResEventStruct>()
{
	item.id			= 0l;
	item.pool_idx   = -1;
	item.name[0]	= '\0';
	item.startTime	= 0l;
	item.endTime	= 0l;
	item.startPos	= -1;
	item.endPos		= -1;
	item.speed		= 0.0;
}

ResourceElementLogEntry::~ResourceElementLogEntry()
{

}


void ResourceElementLogEntry::setID( long _lId )
{
	item.id = _lId;
}

void ResourceElementLogEntry::setPoolIdx(int _iIdx )
{
	item.pool_idx = _iIdx;
}

void ResourceElementLogEntry::setName( const CString& _str )
{
	if( _str.GetLength() > RESOURCE_NAME_LEN-1 )
	{
		CString _name;
		_name.Format("...%s", _str.Right( RESOURCE_NAME_LEN -5  ) );
		strcpy( item.name, _name );
	}
	else
	{
		strcpy( item.name, _str) ;
	}
}

void ResourceElementLogEntry::setStartTime( const ElapsedTime& _time )
{
	item.startTime = (long) _time;
}

void ResourceElementLogEntry::setEndTime( const ElapsedTime& _time )
{
	item.endTime = (long) _time;
}

void ResourceElementLogEntry::setSpeed( float _speed)
{
	item.speed = _speed;
}

void ResourceElementLogEntry::setIndex( long _lIdx )
{
	item.indexNum = _lIdx;
}

int ResourceElementLogEntry::getPoolIdx( void ) const
{
	return item.pool_idx;
}

CString ResourceElementLogEntry::getName( void ) const
{
	return CString( item.name );
}

ElapsedTime ResourceElementLogEntry::getStartTime( void ) const
{
	ElapsedTime time;
	time.setPrecisely( item.startTime );
	return time;
}

ElapsedTime ResourceElementLogEntry::getEndTime( void ) const
{
	ElapsedTime time;
	time.setPrecisely( item.endTime );
	return time;
}

float ResourceElementLogEntry::getSpeed( void ) const
{
	return item.speed;
}

long ResourceElementLogEntry::getIndex( void ) const
{
	return item.indexNum;
}

void ResourceElementLogEntry::echo (ofsstream& p_stream, const CString& _csProjPath) const
{
	assert( m_pOutTerm );
    p_stream << item.id << ',' 
			 << item.name<< ','
			 << getStartTime()<< ',' 
			 << getEndTime()<< '\n';
}