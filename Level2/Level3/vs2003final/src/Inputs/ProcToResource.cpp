// ProcToResource.cpp: implementation of the CProcToResource class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ProcToResource.h"
#include "../Common/ProbabilityDistribution.h"
#include "probab.h"
#include "inputs\resourcepool.h"
#include "in_term.h"
#include "..\Inputs\PipeDataSet.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcToResource::CProcToResource()
{
	m_proServiceTime = new ConstantDistribution(0);
	m_iTypeOfUsingPipe = 0;
	m_vUsedPipes.clear();
}

CProcToResource::CProcToResource( const CProcToResource& _prc2Res )
{
	m_procID = _prc2Res.m_procID;
	m_mobileCon = _prc2Res.m_mobileCon;
	m_strResourcePoolName = _prc2Res.m_strResourcePoolName;
	m_iTypeOfUsingPipe = _prc2Res.m_iTypeOfUsingPipe;
	m_vUsedPipes = _prc2Res.m_vUsedPipes;
	m_proServiceTime = ProbabilityDistribution::CopyProbDistribution( _prc2Res.m_proServiceTime);
}

CProcToResource::CProcToResource( const ProcessorID& _id, const CMobileElemConstraint& _mob, const CString& _strName, ProbabilityDistribution* _pro) 
				: m_procID( _id ), m_mobileCon( _mob), m_strResourcePoolName( _strName)
{
	if( _pro )
		m_proServiceTime = _pro;
	else
		m_proServiceTime = new ConstantDistribution(0);
	m_iTypeOfUsingPipe = 0;
	m_vUsedPipes.clear();
}

CProcToResource& CProcToResource::operator=( const CProcToResource& _prc2Res )
{
	if( this!= &_prc2Res )
	{
		m_procID = _prc2Res.m_procID;
		m_mobileCon = _prc2Res.m_mobileCon;
		m_strResourcePoolName = _prc2Res.m_strResourcePoolName;
		if( m_proServiceTime )
			delete m_proServiceTime;
		m_iTypeOfUsingPipe = _prc2Res.m_iTypeOfUsingPipe;
		m_vUsedPipes = _prc2Res.m_vUsedPipes;
		m_proServiceTime = ProbabilityDistribution::CopyProbDistribution( _prc2Res.m_proServiceTime);
	}
	
	return *this;
}

CProcToResource::~CProcToResource()
{
	if( m_proServiceTime )
		delete m_proServiceTime;
}

// get & set
const ProcessorID& CProcToResource::getProcessorID( void ) const
{
	return m_procID;
}

const CMobileElemConstraint& CProcToResource::getMobileConstraint( void ) const
{
	return m_mobileCon;
}

const CString& CProcToResource::getResourcePoolName( void ) const
{
	return m_strResourcePoolName;
}

const ProbabilityDistribution* CProcToResource::getProServiceTime( void ) const
{
	return m_proServiceTime;
}

void CProcToResource::setProcessorID( const ProcessorID& _procID )
{
	m_procID = _procID;
}

void CProcToResource::setMobileElemConstraint( const CMobileElemConstraint& _mob)
{
	m_mobileCon = _mob;
}

void CProcToResource::setResourcePoolName( const CString& _strPoolName )
{
	m_strResourcePoolName = _strPoolName;
}

void CProcToResource::setServiceTime( ProbabilityDistribution* _pro )
{
	m_proServiceTime = _pro;
}

long CProcToResource::getRandomServiceTime( void ) const
{
	if( m_proServiceTime )
		return (long)m_proServiceTime->getRandomValue();
	return 0;
}

bool CProcToResource::operator<( const CProcToResource& _proc2Res )  const
{
	return (_proc2Res.m_procID < m_procID ) || 
		   (  !( m_procID < _proc2Res.m_procID)  &&  m_mobileCon < _proc2Res.m_mobileCon );
}

bool CProcToResource::operator==( const CProcToResource& _proc2Res ) const
{
	return m_procID == _proc2Res.m_procID && m_mobileCon == _proc2Res.m_mobileCon;
}

int CProcToResource::GetPipeAt( int _iIdx ) const
{
	ASSERT( _iIdx >=0 && _iIdx < static_cast<int>(m_vUsedPipes.size()) );
	return m_vUsedPipes.at( _iIdx );
}

CString CProcToResource::GetPipeString(InputTerminal* pTerm)
{
	CString sReturnStr = "";
	if( GetTypeOfUsingPipe() == USE_PIPE_SYSTEM )// via pipe system
	{
		sReturnStr = " [ Via :Pipe System ] ";
		return sReturnStr;
	}
	if(GetTypeOfUsingPipe() == USE_NOTHING )
		return "None";

	int iCount = GetUsedPipeCount();	
	if( iCount <=0 )
		return "None";

	sReturnStr = " [ Via :";
	for( int i=0; i < iCount-1; ++i )
	{
		sReturnStr += pTerm->m_pPipeDataSet->GetPipeAt( GetPipeAt(i) )->GetPipeName();
		sReturnStr += " --> ";
	}
	sReturnStr += pTerm->m_pPipeDataSet->GetPipeAt( GetPipeAt(iCount-1))->GetPipeName();
	sReturnStr += " ] ";
	return sReturnStr;
}

/************************************************************************/
/*                      CProcToResourceDataSet                          */
/************************************************************************/

CProcToResourceDataSet::CProcToResourceDataSet():DataSet( ProcessorToResourcePoolFile, 2.3f )
{
	m_Proc2ResList.clear();
}

CProcToResourceDataSet::~CProcToResourceDataSet()
{
	m_Proc2ResList.clear();
}

// get proc to resource list's size
int CProcToResourceDataSet::getProc2ResListSize( void ) const
{
	return m_Proc2ResList.size();
}

// get proc to resource list
PROC2RESSET& CProcToResourceDataSet::getProc2ResList( void )
{
	return m_Proc2ResList;
}


// get best resource pool
bool CProcToResourceDataSet::getBestResourcePoolAndTime( const ProcessorID& _procID, const CMobileElemConstraint& _mobCon, CResourcePool* _resPool, long& _lTime )
{
	PROC2RESSET::iterator iter;
	
	for( iter = m_Proc2ResList.begin(); iter!= m_Proc2ResList.end(); ++iter )
	{
		if( iter->getProcessorID().idFits( _procID) && iter->getMobileConstraint().fits( _mobCon ) )
		{
			CString strPoolName = iter->getResourcePoolName();
			CResourcePool* _pPool = m_pInTerm->m_pResourcePoolDB->getResourcePoolByName( strPoolName );
			
			if( _pPool == NULL )
				return false;
			_resPool = _pPool;

			_lTime = iter->getRandomServiceTime();
			return true;
		}
	}

	return false;
}

// add new proc2pool to dataset
bool CProcToResourceDataSet::addNewProcToRes( const CProcToResource& _proc2Res )
{
	if( ifRepeat( _proc2Res.getProcessorID(), _proc2Res.getMobileConstraint()) )
		return false;

	m_Proc2ResList.insert(  _proc2Res );
	return true;
}

// if exist in the list
bool CProcToResourceDataSet::ifRepeat( const ProcessorID& _procID, const CMobileElemConstraint& _mobCon ) const
{
	PROC2RESSET::const_iterator iter;
	for( iter = m_Proc2ResList.begin(); iter != m_Proc2ResList.end(); ++iter )
	{
		if( iter->getProcessorID() == _procID && iter->getMobileConstraint().isEqual( &_mobCon ) )
			return true;
	}

	return false;

	/*
	CProcToResource proc2res( _procID, _mobCon );

	PROC2RESSET::const_iterator const_iter = m_Proc2ResList.find( proc2res );
	
	return const_iter != m_Proc2ResList.end(); 
	*/
}

// remove item by name
bool CProcToResourceDataSet::removeItemByName( const CString& _strProcID, const CString& _strPaxType )
{
	CString szBuffer;
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( _strProcID );

	PROC2RESSET::iterator iter;
	for( iter = m_Proc2ResList.begin(); iter != m_Proc2ResList.end(); ++iter )
	{
		iter->getMobileConstraint().screenPrint( szBuffer, 0, 256 );
		if( iter->getProcessorID() == procID && _strPaxType == szBuffer )
		{
			m_Proc2ResList.erase( iter );
			return true;
		}
	}

	return false;
}


// get item by name
CProcToResource* CProcToResourceDataSet::getItemByName( const CString& _strProcID, const CString& _strPaxType )
{
	CString szBuffer;
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( _strProcID );
	
	PROC2RESSET::iterator iter;
	for( iter = m_Proc2ResList.begin(); iter != m_Proc2ResList.end(); ++iter )
	{
		iter->getMobileConstraint().screenPrint( szBuffer, 0, 256 );
		if( iter->getProcessorID() == procID && _strPaxType == szBuffer )
		{
			return &(*iter);
		}
	}
	return NULL;
}


// modify paxType 
bool CProcToResourceDataSet::modifyItemPaxType( const CString& _strProcID, const CString& _strPaxType, const CMobileElemConstraint& _mobCon )
{
	CString szBuffer;
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( _strProcID );
	
	if( ifRepeat( procID, _mobCon ) )
		return false;

	PROC2RESSET::iterator iter;
	for( iter = m_Proc2ResList.begin(); iter != m_Proc2ResList.end(); ++iter )
	{
		iter->getMobileConstraint().screenPrint( szBuffer, 0, 256 );
		if( iter->getProcessorID() == procID && _strPaxType == szBuffer )
		{
			ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution( const_cast< ProbabilityDistribution*>(iter->getProServiceTime()) );
			CProcToResource proToRes( procID, _mobCon, iter->getResourcePoolName(),pDist );
			m_Proc2ResList.erase( iter );
			m_Proc2ResList.insert( proToRes );
			return true;
		}
	}

	return false;
}

void CProcToResourceDataSet::clear (void)
{
	m_Proc2ResList.clear();
}

void CProcToResourceDataSet::readObsoleteData (ArctermFile& p_file)
{
	float version = p_file.getVersion();
	if(-0.00001f < (version-2.2f) && (version-2.2f) < 0.00001f) // version 2.20
	{
		readObsoleteData22(p_file);
	}
}

void CProcToResourceDataSet::readObsoleteData22 (ArctermFile& p_file)
{
	char szBuffer[512];
	int _iCount;

	p_file.getLine();
	p_file.getInteger( _iCount );

	for( int i=0; i< _iCount; i++ )
	{
		p_file.getLine();
		// processor
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict);
		procID.readProcessorID( p_file );
		// mobile type
		CMobileElemConstraint mob_con(m_pInTerm);
		//mob_con.SetInputTerminal( m_pInTerm );
		p_file.getField( szBuffer,512 );
		mob_con.readConstraintWithVersion( szBuffer );	
		// resource name
		CString strResourceName;
		p_file.getField( szBuffer,512 );
		strResourceName = szBuffer;
		// service 
		p_file.setToField(3);
		ProbabilityDistribution* _pro = NULL;
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);

		m_Proc2ResList.insert( CProcToResource(procID, mob_con,  strResourceName, _pro) );
	}
}

void CProcToResourceDataSet::readData (ArctermFile& p_file)
{
	char szBuffer[512];
	int _iCount;

	p_file.getLine();
	p_file.getInteger( _iCount );

	for( int i=0; i< _iCount; i++ )
	{
		p_file.getLine();
		// processor
		ProcessorID procID;
		procID.SetStrDict( m_pInTerm->inStrDict);
		procID.readProcessorID( p_file );
		// mobile type
		CMobileElemConstraint mob_con(m_pInTerm);
		//mob_con.SetInputTerminal( m_pInTerm );
		p_file.getField( szBuffer,512 );
		mob_con.readConstraintWithVersion( szBuffer );	
		// resource name
		CString strResourceName;
		p_file.getField( szBuffer,512 );
		strResourceName = szBuffer;
		// service 
		p_file.setToField(3);
		ProbabilityDistribution* _pro = NULL;
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);

		CProcToResource procToRes(procID, mob_con,  strResourceName, _pro);

		// pipe
		p_file.getField(szBuffer, 512);
		int iStrlen = strlen(szBuffer);
		for(int i=0; i<iStrlen; i++)
		{
			if(szBuffer[i] == '-')
			{
				szBuffer[i] = ',';
			}
		}
		ArctermFile tempFile;
		tempFile.InitDataFromString(szBuffer);
		int typeOfUsingPipe, countOfPipe, pipeId;
		tempFile.getInteger(typeOfUsingPipe); // Get the type of using  pipe.
		procToRes.SetTypeOfUsingPipe(typeOfUsingPipe);
		if( typeOfUsingPipe == USE_USER_SELECTED_PIPES ) // Type of using pipe is Manual.
		{
			tempFile.getInteger(countOfPipe); // Get the count of pipes.
			for(int i=0; i<countOfPipe; i++)
			{
				tempFile.getInteger(pipeId);
				procToRes.AddUsedPipe(pipeId);
			}
		}

		m_Proc2ResList.insert( procToRes );
	}

}

void CProcToResourceDataSet::writeData (ArctermFile& p_file) const
{
	//char szBuffer[512]; matt
	char szBuffer[5120];
	char pipeBuf[16];

	int _iCount = m_Proc2ResList.size();
	p_file.writeInt( _iCount );
	p_file.writeLine();

	PROC2RESSET::const_iterator const_iter;
	for( const_iter = m_Proc2ResList.begin(); const_iter != m_Proc2ResList.end(); ++const_iter )
	{
		// processor ID
		const_iter->getProcessorID().writeProcessorID( p_file );
		// pax type
		const_iter->getMobileConstraint().WriteSyntaxStringWithVersion( szBuffer );
		p_file.writeField( szBuffer );
		// resourec name
		p_file.writeField( const_iter->getResourcePoolName() );
		// service time
		const_iter->getProServiceTime()->writeDistribution( p_file );
		// pipe
		int typeOfPipe = const_iter->GetTypeOfUsingPipe();
		memset(pipeBuf, 0, 16);
		itoa(typeOfPipe, pipeBuf, 10);
		memset(szBuffer, 0, 5120);
		strncpy( szBuffer, pipeBuf, 16 );
		if( typeOfPipe == USE_USER_SELECTED_PIPES )
		{
			int count = const_iter->GetUsedPipeCount();
			memset(pipeBuf, 0, 16);
			itoa(count, pipeBuf, 10);
			strcat( szBuffer, "-");
			strcat( szBuffer, pipeBuf );
			for(int i=0; i<count; i++)
			{
				itoa(const_iter->GetPipeAt(i), pipeBuf, 10);
				strcat( szBuffer, "-");
				strcat( szBuffer, pipeBuf);
			}
		}
		p_file.writeField( szBuffer );
		p_file.writeLine();
	}
}

const char* CProcToResourceDataSet::getTitle (void) const
{
	return "Processor' resource pool";
}

const char* CProcToResourceDataSet::getHeaders (void) const
{
	return "ProcessorID, PaxType, Resource,ServiceTime";
}

