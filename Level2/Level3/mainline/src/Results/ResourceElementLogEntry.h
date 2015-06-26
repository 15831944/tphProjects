// ResourceElementLogEntry.h: interface for the ResourceElementLogEntry class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESOURCEELEMENTLOGENTRY_H__18FA40DB_6780_4C90_A3F4_523B0A9499D7__INCLUDED_)
#define AFX_RESOURCEELEMENTLOGENTRY_H__18FA40DB_6780_4C90_A3F4_523B0A9499D7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "common\template.h"
#include "common\elaptime.h"
#include "results\logitem.h"
#include "common\term_bin.h"

/************************************************************************/
/*                             ResourceElementLogEntry                  */
/************************************************************************/
class ResourceElementLogEntry : public TerminalLogItem<ResDescStruct,ResEventStruct>
{
public:
	ResourceElementLogEntry();
	virtual ~ResourceElementLogEntry();

public:
	void setID( long _lId );
	void setPoolIdx( int _iIdx );
	void setName( const CString& _str );
	void setStartTime( const ElapsedTime& _time );
	void setEndTime( const ElapsedTime& _time );
	void setSpeed( float _speed);
	void setIndex( long _lIdx );

	int getPoolIdx( void ) const;
	CString getName( void ) const;
	ElapsedTime getStartTime( void ) const;
	ElapsedTime getEndTime( void ) const;
	float getSpeed( void ) const;
	long getIndex( void ) const;

	virtual void echo (ofsstream& p_stream, const CString& _csProjPath) const;
};

#endif // !defined(AFX_RESOURCEELEMENTLOGENTRY_H__18FA40DB_6780_4C90_A3F4_523B0A9499D7__INCLUDED_)
