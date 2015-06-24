// SimParameter.cpp: implementation of the CSimParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "SimParameter.h"
#include "MobileElemTypeStrDB.h"

//#include "common\ARCCriticalException.h"
#include "reports\SimAutoReportPara.h"
#include "reports\SimLevelOfService.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimParameter::CSimParameter()
	:DataSet( SimulationParametersFile, 2.8f )
{
	m_pSimPara = NULL;
	//m_timeStart will be automaticly set as 00:00:00
	//m_timeEnd.set(23,59,59);

	/*
	m_timeEnd.set(23,59,59);

	m_timeStart.set(0, 0, 0);
	m_initPeriod.set(0, 0, 0);
	m_followupPeriod.set(0, 0, 0);*/

}

CSimParameter::~CSimParameter()
{
	clear();
}


void CSimParameter::readObsoleteData(ArctermFile& p_file)
{
	if( p_file.getVersion() <= 2.2f )
	{
		m_pSimPara = new CSimGeneralPara( m_startDate );
		p_file.getLine();

		// random number seed
		int nVal;
		p_file.getInteger( nVal );
		m_pSimPara->SetRamdomSpeed( nVal );	

		// true random
		p_file.getInteger( nVal );
		if( nVal == 1 )//true random
		{
			m_pSimPara->SetRamdomSpeed( -1 );
		}

		long lPaxCount;
		// passenger count
		p_file.getInteger( lPaxCount );
		m_pSimPara->SetMaxCount( lPaxCount );

		// visitors flag
		p_file.getInteger( nVal );
		BOOL bCheckVisitor=(nVal==1?TRUE:FALSE);
		m_pSimPara->EnableNoPax( nVal );

		// baggage flag
		p_file.getInteger( nVal );//no use in new version	
		BOOL bCheckBaggage=(nVal==1?TRUE:FALSE);
		
		// barrier
		p_file.getInteger( nVal );
		m_pSimPara->EnableBarriers( nVal );

		// hubbing operations flag
		p_file.getInteger( nVal );
		m_pSimPara->EnableHubbing( nVal );

		m_pSimPara->SetNumberOfRun( 1 );

		std::vector<BOOL>vNopaxDetailFlag;
		//int iCount = m_pInTerm->m_pMobElemTypeStrDB->GetCount();
		//for( int i=0; i<iCount-1; ++i )
		for( int i=0; i<NOPAX_COUNT; ++i )	
		{
			if(i==0)//Visitor
				vNopaxDetailFlag.push_back(bCheckVisitor);
			else if(i==1)//Baggage
			{
				m_pSimPara->EnableNoPax( 1);
				vNopaxDetailFlag.push_back(bCheckBaggage);
			}
			else
				vNopaxDetailFlag.push_back(FALSE);
		}
		m_pSimPara->SetNoPaxDetailFlag( vNopaxDetailFlag );
		//pPara->SetParaName("Default");
		//m_vAllSimPara.push_back( pPara );

	}
	else if( p_file.getVersion() <= 2.7f )
	{
		char strType[256];
		p_file.getLine();
		p_file.getField( strType,256 );
		CString strTemp= strType;
		strTemp.TrimLeft();
		strTemp.TrimRight();
		if( strTemp == "GENERAL" )
		{
			m_pSimPara = new CSimGeneralPara( m_startDate );
		}
		else if ( strTemp == "AUTOREPORT" )
		{
			m_pSimPara = new CSimAutoReportPara( m_startDate );
		}
		else
		{
			m_pSimPara = new CSimLevelOfService( m_startDate );
		}

		m_pSimPara->readObsoleteData( p_file ,m_pInTerm);
	}
}
void CSimParameter::readData(ArctermFile& p_file)
{
	char strType[256];
	p_file.getLine();
	p_file.getField( strType,256 );
	CString strTemp= strType;
	strTemp.TrimLeft();
	strTemp.TrimRight();
	if( strTemp == "GENERAL" )
	{
		m_pSimPara = new CSimGeneralPara( m_startDate );
	}
	else if ( strTemp == "AUTOREPORT" )
	{
		m_pSimPara = new CSimAutoReportPara( m_startDate );
	}
	else
	{
		m_pSimPara = new CSimLevelOfService( m_startDate );
	}

	m_pSimPara->ReadData( p_file ,m_pInTerm);
}


void CSimParameter::writeData(ArctermFile& p_file) const
{
	
	if( !m_pSimPara )
	{
		(const_cast<CSimParameter*>(this))->m_pSimPara = new CSimGeneralPara( m_startDate );
		std::vector<BOOL>vNopaxDetailFlag;
		//int iCount = m_pInTerm->m_pMobElemTypeStrDB->GetCount();
		//for( int i=0; i<iCount-1; ++i	)
		for( int i=0; i<NOPAX_COUNT; ++i	)
		{
			vNopaxDetailFlag.push_back( FALSE );
		}
		(const_cast<CSimParameter*>(this))->m_pSimPara->SetNoPaxDetailFlag( vNopaxDetailFlag );
	}
	p_file.writeField( m_pSimPara->GetClassType() );		
	p_file.writeLine();
	//ASSERT( m_pInTerm->m_pMobElemTypeStrDB->GetCount()-1 == m_pSimPara->GetNoPaxDetailFlag().size() );
	m_pSimPara->WriteData( p_file );
}
int CSimParameter::GetRandomFlag()
{
	if( m_pSimPara->GetRamdomSpeed() == -1 )	
	{
		return 1;
	}

	return 0;
	
}

void CSimParameter::SetRandomFlag( int _nRandomFlag )
{
	if( _nRandomFlag == 1 )
	{
		m_pSimPara->SetRamdomSpeed( -1 );
	}
}
void CSimParameter::clear ()
{
	if( m_pSimPara )
	{
		delete m_pSimPara;
		m_pSimPara = NULL;
	}
	/*
	int iSize = m_vAllSimPara.size();
	for( int i=0; i<iSize; ++i )
	{
		delete m_vAllSimPara[ i ];
	}
*/
}
/*
CSimGeneralPara* CSimParameter::operator[] ( int _iIdx )
{
	if( _iIdx<0 || _iIdx>m_vAllSimPara.size() )
	{
		throw new ARCCriticalException(Error_OutOf_Bound," Occurrence Place : Report Parameter DataBase");
	}

	return m_vAllSimPara[ _iIdx ];
}

bool CSimParameter::IfExist( CString _strName )
{
	int iSize = m_vAllSimPara.size();
	for( int i=0; i<iSize; ++i )
	{
		if( m_vAllSimPara[i]->GetParaName() == _strName )
		{
			return true;
		}
	}
	return false;
}
*/

const CStartDate& CSimParameter::GetStartDate() const
{
	return m_pSimPara->GetStartDate();
}


const ElapsedTime& CSimParameter::GetInitPeriod() const
{
	return m_pSimPara->GetInitPeriod();
}

const ElapsedTime& CSimParameter::GetFollowupPeriod() const
{
	return m_pSimPara->GetFollowupPeriod();
}

void CSimParameter::SetInitPeriod(const ElapsedTime &_t)
{
	m_pSimPara->SetInitPeriod(_t);
}

void CSimParameter::SetFollowupPeriod(const ElapsedTime &_t)
{
	m_pSimPara->SetFollowupPeriod(_t);
}
