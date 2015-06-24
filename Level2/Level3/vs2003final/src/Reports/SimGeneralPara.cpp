// SimGeneralPara.cpp: implementation of the CSimGeneralPara class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "SimGeneralPara.h"

#include "common\termfile.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "inputs\in_term.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimGeneralPara::CSimGeneralPara( CStartDate _startDate )
:m_iRandomSpeed( 3 ),m_lMaxCount( -1 ),m_bHaveNoPax( FALSE)
,m_bEnableBarriers( FALSE ),m_bEnableHubbing( FALSE ),m_bEnableCongestionImpact(FALSE)
,m_bEnableCollision(FALSE),m_bEnableArea(FALSE),m_bEnablePipe(FALSE)
,m_iNumberOfRun( 1 )
{
	m_timeStart.set(0, 0, 0);
//	m_timeStart = ElapsedTime(WholeDay);
	m_timeEnd.set(23, 59, 59);
	m_timeEnd += m_timeStart;

	m_initPeriod.set(0, 0, 0);
	m_followupPeriod.set(0, 0, 0);
	m_dateStart = _startDate;
	m_vArea.clear();
	m_vPipe.clear();
}

CSimGeneralPara::~CSimGeneralPara()
{
	m_vArea.clear();
	m_vPipe.clear();
}

void CSimGeneralPara::WriteData( ArctermFile& _file )
{
	//_file.writeField( m_strParaName );
	_file.writeInt( m_iNumberOfRun );
	_file.writeInt( m_iRandomSpeed );

	_file.writeTime( m_timeStart, TRUE);
	_file.writeTime( m_timeEnd, TRUE );

	_file.writeTime( m_initPeriod );
	_file.writeTime( m_followupPeriod );


	_file.writeInt( m_lMaxCount );
	_file.writeInt( m_bHaveNoPax );
	_file.writeInt( m_bEnableBarriers );
	_file.writeInt( m_bEnableHubbing );	
	_file.writeInt(m_bEnableCongestionImpact);
	_file.writeInt(m_bEnableCollision);
	_file.writeInt(m_bEnableArea);
	_file.writeInt(m_bEnablePipe);

	int iArea = (int)m_vArea.size();
	std::map<CString,int>::const_iterator iter = m_vArea.begin();
	_file.writeInt(iArea);
	for (int i = 0; i < iArea; ++i,++iter)
	{	
		_file.writeField((*iter).first);
		_file.writeInt((*iter).second);
	}

	int iPipe = (int)m_vPipe.size();
	iter = m_vPipe.begin();
	_file.writeInt(iPipe);
	for (i = 0; i < iPipe; ++i,++iter)
	{
		_file.writeField((*iter).first);
		_file.writeInt((*iter).second);
	}

	int iNoPaxDetailFlagSize = m_vNonPaxDetailFlag.size();
	_file.writeInt( iNoPaxDetailFlagSize );
	for( i=0; i<iNoPaxDetailFlagSize; ++i )
	{
		_file.writeInt( m_vNonPaxDetailFlag[i] );
	}
	_file.writeLine();
	
}

void CSimGeneralPara::read2_6Data(ArctermFile& _file,InputTerminal* _interm )
{
	_file.getLine();

	_file.getInteger( m_iNumberOfRun );
	_file.getInteger( m_iRandomSpeed );

	if( 2.59f <= _file.getVersion() && _file.getVersion() <= 2.61f )
	{
		//	get start time
		_file.getTime( m_timeStart, TRUE );

		//	get end time
		_file.getTime( m_timeEnd, TRUE );

		//	get init period
		_file.getTime( m_initPeriod );

		//	get followup period
		_file.getTime( m_followupPeriod );
	}

	_file.getInteger( m_lMaxCount );
	_file.getInteger( m_bHaveNoPax );
	_file.getInteger( m_bEnableBarriers );
	_file.getInteger( m_bEnableHubbing );

	//	get start date

	if( _file.getVersion() > 2.355f )
	{
		int iNoPaxDetailFlagSize = -1;
		_file.getInteger( iNoPaxDetailFlagSize );
		int iValue=-1;
		for( int i=0; i<iNoPaxDetailFlagSize; ++i )
		{
			_file.getInteger( iValue );
			m_vNonPaxDetailFlag.push_back( iValue );
		}
	}
	else if(_file.getVersion() == 2.3000f)
	{
		if(m_bHaveNoPax==1)
		{
			m_vNonPaxDetailFlag.push_back(TRUE);
			m_vNonPaxDetailFlag.push_back(TRUE);
			for( int i=2; i<NOPAX_COUNT; ++i	)
				m_vNonPaxDetailFlag.push_back( FALSE );
		}
		else
		{
			for( int i=0; i<NOPAX_COUNT; ++i	)
				m_vNonPaxDetailFlag.push_back( FALSE );
		}
	}
	else
	{
		for( int i=0; i<NOPAX_COUNT; ++i	)
		{
			m_vNonPaxDetailFlag.push_back( FALSE );
		}
	}

	while( m_vNonPaxDetailFlag.size() < NOPAX_COUNT )
		m_vNonPaxDetailFlag.push_back( FALSE );
}

void CSimGeneralPara::read2_7Data(ArctermFile& _file,InputTerminal* _interm )
{
	_file.getLine();
	_file.getInteger( m_iNumberOfRun );
	_file.getInteger( m_iRandomSpeed );


	//	get start time
	_file.getTime( m_timeStart, TRUE );

	//	get end time
	_file.getTime( m_timeEnd, TRUE );

	//	get init period
	_file.getTime( m_initPeriod );

	//	get followup period
	_file.getTime( m_followupPeriod );


	_file.getInteger( m_lMaxCount );
	_file.getInteger( m_bHaveNoPax );
	_file.getInteger( m_bEnableBarriers );
	_file.getInteger( m_bEnableHubbing );
	_file.getInteger(m_bEnableCongestionImpact);

	int iNoPaxDetailFlagSize = -1;
	_file.getInteger( iNoPaxDetailFlagSize );
	int iValue=-1;
	for( int i=0; i<iNoPaxDetailFlagSize; ++i )
	{
		_file.getInteger( iValue );
		m_vNonPaxDetailFlag.push_back( iValue );
	}
	
	while( m_vNonPaxDetailFlag.size() < NOPAX_COUNT )
		m_vNonPaxDetailFlag.push_back( FALSE );
}

void CSimGeneralPara::readObsoleteData( ArctermFile& _file,InputTerminal* _interm )
{
	if( _file.getVersion() <= 2.6f )
	{
		read2_6Data(_file,_interm);
	}
	else if (_file.getVersion() <= 2.7f)
	{
		read2_7Data(_file,_interm);
	}
}

void CSimGeneralPara::ReadData( ArctermFile& _file, InputTerminal* _interm )
{
	_file.getLine();
	_file.getInteger( m_iNumberOfRun );
	_file.getInteger( m_iRandomSpeed );

	//	get start time
	_file.getTime( m_timeStart, TRUE );

	//	get end time
	_file.getTime( m_timeEnd, TRUE );

	//	get init period
	_file.getTime( m_initPeriod );

	//	get followup period
	_file.getTime( m_followupPeriod );


	_file.getInteger( m_lMaxCount );
	_file.getInteger( m_bHaveNoPax );
	_file.getInteger( m_bEnableBarriers );
	_file.getInteger( m_bEnableHubbing );
	_file.getInteger(m_bEnableCongestionImpact);
	_file.getInteger(m_bEnableCollision);
	_file.getInteger(m_bEnableArea);
	_file.getInteger(m_bEnablePipe);
	
	//read collision
	m_vArea.clear();
	int iArea = -1;
	_file.getInteger(iArea);
	char sArea[256];
	int bEnable = 0;
	for (int i = 0; i < iArea; ++i)
	{
		_file.getField(sArea,255);
		_file.getInteger(bEnable);
		m_vArea.insert(std::map<CString,int>::value_type(sArea,bEnable));
	}

	m_vPipe.clear();
	int iPipe = -1;
	char sPipe[256];
	_file.getInteger(iPipe);
	bEnable = 0;
	for (i = 0; i < iPipe; ++i)
	{
		_file.getField(sPipe,255);
		_file.getInteger(bEnable);
		m_vPipe.insert(std::map<CString,int>::value_type(sPipe,bEnable));
	}

	//	get start date
	int iNoPaxDetailFlagSize = -1;
	_file.getInteger( iNoPaxDetailFlagSize );
	int iValue=-1;
	for( int i=0; i<iNoPaxDetailFlagSize; ++i )
	{
		_file.getInteger( iValue );
		m_vNonPaxDetailFlag.push_back( iValue );
	}
	
	while( m_vNonPaxDetailFlag.size() < NOPAX_COUNT )
		m_vNonPaxDetailFlag.push_back( FALSE );
}

const CStartDate& CSimGeneralPara::GetStartDate() const
{
	return m_dateStart;
}

const ElapsedTime& CSimGeneralPara::GetStartTime() const
{
	return m_timeStart;
}


const ElapsedTime& CSimGeneralPara::GetEndTime() const
{
	return m_timeEnd;
}

const ElapsedTime& CSimGeneralPara::GetInitPeriod() const
{
	return m_initPeriod;
}

const ElapsedTime& CSimGeneralPara::GetFollowupPeriod() const
{
	return m_followupPeriod;
}

void CSimGeneralPara::SetStartTime(const ElapsedTime &_t)
{
	m_timeStart = _t;
}

void CSimGeneralPara::SetEndTime(const ElapsedTime &_t)
{
	m_timeEnd = _t;
}

void CSimGeneralPara::SetInitPeriod(const ElapsedTime &_t)
{
	m_initPeriod = _t;
}

void CSimGeneralPara::SetFollowupPeriod(const ElapsedTime &_t)
{
	m_followupPeriod = _t;
}

BOOL CSimGeneralPara::IfAreaEnable()const
{
	return m_bEnableArea;
}

BOOL CSimGeneralPara::IfPipeEnable()const
{
	return m_bEnablePipe;
}

BOOL CSimGeneralPara::IfCollisionEnable()const
{
	return m_bEnableCollision;
}

BOOL CSimGeneralPara::IfAreaItemEnable(const CString& sAreaName)const
{
	if(m_vArea.find(sAreaName) != m_vArea.end())
	{
		return m_vArea.find(sAreaName)->second;
	}
	return FALSE;
}

BOOL CSimGeneralPara::IfPipeItemEnable(const CString& sPipeName)const
{
	if (m_vPipe.find(sPipeName) != m_vPipe.end())
	{
		return m_vPipe.find(sPipeName)->second;
	}
	return FALSE;
}

void CSimGeneralPara::EnablePipeItem(const CString& sPipeName,int bEnable)
{
	m_vPipe[sPipeName] = bEnable;
}

void CSimGeneralPara::EnableAreaItem(const CString& sAreaName,int bEnable)
{
	m_vArea[sAreaName] = bEnable;
}