// FireEvacuationReportElement.cpp: implementation of the FireEvacuationReportElement class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "FireEvacuationReportElement.h"
#include "results\paxentry.h"
#include "results\fltlog.h"
#include "common\termfile.h"
#include "common\point.h"
#include "common\states.h"
#include "engine\proclist.h"
#include "engine\terminal.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FireEvacuationReportElement::FireEvacuationReportElement()
						   : m_lProcessorIndex( -1l), m_lExitTime( -1l), m_lExitDuration( -1l )
{

}

FireEvacuationReportElement::~FireEvacuationReportElement()
{
	
}


// get data from log
bool FireEvacuationReportElement::getDataFromPaxLog( void )
{
	m_lProcessorIndex	= -1l;
	m_lExitTime			= -1l;
	m_lExitDuration		= -1l;

	loadEvents( );

	int _iTrackCount = getCount();
	if( _iTrackCount <= 0 )
		return false;

	m_lExitTime = getEvent( _iTrackCount-1 ).time;
	bool _bDoFireEvacution = false;
	for( int i = _iTrackCount-2; i>=0; i-- )
	{
		MobEventStruct mobStruct = getEvent(i);
		if( mobStruct.state == EvacuationFire )
		{
			m_lProcessorIndex	= mobStruct.procNumber;
			m_lExitDuration		= m_lExitTime - mobStruct.time;
			_bDoFireEvacution = true;
			break;
		}
	}

	clearLog();
	return _bDoFireEvacution;
}

// write data to report file
void FireEvacuationReportElement::writeEntry( ArctermFile& p_file, Terminal* _pTerm  ) const
{	
	assert( _pTerm );
	char _szBuf[MAX_PAX_TYPE_LEN];

	p_file.writeInt( item.id );
	printFullType( _szBuf );
	p_file.writeField( _szBuf );

	Processor* _pProc = _pTerm->procList->getProcessor( m_lProcessorIndex );
	if( _pProc )
		p_file.writeField( _pProc->getID()->GetIDString() );
	else
		p_file.writeField(" " );

	p_file.writeTime( ElapsedTime(m_lExitTime/TimePrecision), TRUE );
	p_file.writeTime( ElapsedTime(m_lExitDuration/TimePrecision), TRUE );
	p_file.writeLine();
}