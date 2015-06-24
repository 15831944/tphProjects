// ResourcePool.cpp: implementation of the CResourcePool class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "ResourcePool.h"
#include "../Common/ProbabilityDistribution.h"
#include "probab.h"
#include "common\pollygon.h"
#include "engine\terminal.h"
#include "engine\resourceElement.h"
#include "common\states.h"
#include "../Common/ARCTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/************************************************************************/
/*		CResourcePool class                                             */
/************************************************************************/
CResourcePool::CResourcePool()
{
	m_proSpeed = new ConstantDistribution(1.0);
	m_proCount = new ConstantDistribution(1.0);
	m_emPoolType = ConcurrentType;
}

CResourcePool::CResourcePool( const CString& _str ):m_strPoolName(_str)
{
	m_proSpeed = new ConstantDistribution(0);
	m_proCount = new ConstantDistribution(0);
	m_emPoolType = ConcurrentType;
}

CResourcePool::~CResourcePool()
{
	if( m_proSpeed )
		delete m_proSpeed;
	if( m_proCount )
		delete m_proCount;

	clearEngineDate();
}

CResourcePool::CResourcePool( const CResourcePool& _pool)
{
	m_strPoolName = _pool.m_strPoolName;
	m_proSpeed = ProbabilityDistribution::CopyProbDistribution( _pool.m_proSpeed );
	m_proCount = ProbabilityDistribution::CopyProbDistribution( _pool.m_proCount );
	m_poolBasePath = _pool.m_poolBasePath;
	m_emPoolType = _pool.m_emPoolType;
}

CResourcePool& CResourcePool::operator=( const CResourcePool& _pool )
{
	if( this != &_pool )
	{
		m_strPoolName = _pool.m_strPoolName;
		if( m_proSpeed )
			delete m_proSpeed;
		m_proSpeed = ProbabilityDistribution::CopyProbDistribution( _pool.m_proSpeed );
		if( m_proCount )
			delete m_proCount;
		m_proCount = ProbabilityDistribution::CopyProbDistribution( _pool.m_proCount );
		m_poolBasePath = _pool.m_poolBasePath;
		m_emPoolType = _pool.m_emPoolType;
	}

	return *this;
}

// get & set 
void CResourcePool::setPoolType(const PoolType& emPoolType)
{
	m_emPoolType = emPoolType;
}

const PoolType& CResourcePool::getPoolType()const
{
	return m_emPoolType;
}

const CString& CResourcePool::getPoolName( void ) const 
{
	return m_strPoolName;
}

const ProbabilityDistribution* CResourcePool::getProSpeed( void ) const
{
	return m_proSpeed;
}

const ProbabilityDistribution* CResourcePool::getProCount( void ) const
{
	return m_proCount;
}

const Path& CResourcePool::getPoolBasePath( void ) const
{
	return m_poolBasePath;
}

void CResourcePool::setPoolName( const CString& _strName )
{
	m_strPoolName = _strName;
}

void CResourcePool::setProSpeed( ProbabilityDistribution* _pro )
{
	if( m_proSpeed )
		delete m_proSpeed;
		
	m_proSpeed = _pro;
}

void CResourcePool::setProCount( ProbabilityDistribution* _pro )
{
	if( m_proCount )
		delete m_proCount;
	
	m_proCount = _pro;
}

void CResourcePool::setPoolBasePath( const Path& _path )
{
	m_poolBasePath = _path;
}

double CResourcePool::getRandomSpeed( void ) const
{
	if( m_proSpeed )
		return m_proSpeed->getRandomValue();
	else
		return 0.0;
}

int	CResourcePool::getRandomCount( void ) const
{
	if( m_proCount )
		return (int) m_proCount->getRandomValue();
	else
		return 0;
}

// for sort
bool CResourcePool::operator<( const CResourcePool& _pool) const
{ 
	return m_strPoolName < _pool.m_strPoolName;
}

bool CResourcePool::operator==( const CResourcePool& _pool ) const
{
	return m_strPoolName == _pool.m_strPoolName;
}
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
/************************************************************************/
// for engine

void CResourcePool::clearEngineDate( void )
{
	// clear resource element
	for( UINT i=0; i< m_AllResourceElementList.size(); i++ )
	{
		m_AllResourceElementList[i]->M_DELIDX = 0;
		delete m_AllResourceElementList[i];
	}
	m_AllResourceElementList.clear();
	m_FreeResourceElementList.clear();
	
	m_ResourceRequestList.clear();
}

// get all resource count
int CResourcePool::getAllResourceCount( void ) const
{
	return m_AllResourceElementList.size();
}

// get free resource count
int CResourcePool::getFreeResourceCount( void ) const
{
	return m_FreeResourceElementList.size();
}

// generate resource for pool
int CResourcePool::generateAllResourceElement( Terminal* _pTerm, long& _lId, int _poolIdx )
{
	clearEngineDate( );
	
	int _iAllResourceElementCount = getRandomCount();
	int _index = 0;
	for( int i=0; i< _iAllResourceElementCount; i++ )
	{
		double _dSpeed = getRandomSpeed();
		ResourceElement* pResourceElement = new ResourceElement( _pTerm, this, _poolIdx,  ++_lId, _dSpeed, _index++ );
		m_AllResourceElementList.push_back( pResourceElement );
		m_FreeResourceElementList.push_back( pResourceElement );

		//////////////////////////////////////////////////////////////////////////
		pResourceElement->setState( Birth );
		pResourceElement->generateEvent( ElapsedTime(0l) ,false);
		//////////////////////////////////////////////////////////////////////////	
	}

	return _iAllResourceElementCount;
}


// get resource request num
int CResourcePool::getCurrentRequestNum( void ) const
{
	return m_ResourceRequestList.size();
}

// add a resource request to list
void CResourcePool::addRequestToList( const ResourceRequestItem& _item, const ElapsedTime& _time )
{
	m_ResourceRequestList.push_back( _item );
	handleRequestIfCan( _time ,m_emPoolType,FALSE);
}

void CResourcePool::addRequestToList(const ResourceRequestItem& _item)
{
	m_ResourceRequestList.push_back( _item );
}

// handle request if probability
bool CResourcePool::handleRequestIfCan( const ElapsedTime& _time,const PoolType& emPoolType,BOOL bWait)
{
	if( m_ResourceRequestList.size() == 0 )		// if no request , return
		return false;

	if( m_FreeResourceElementList.size() == 0)	// if no free resource , return
	{
		if (m_emPoolType == PostServiceType && !bWait)
		{
			m_ResourceRequestList.back().request_proc->noticeReleasePax(m_ResourceRequestList.back().request_mob,_time);
			
		}
		return false;
	}
		
	// let the first resource to service the first request
	ResourceElement* _pResource = m_FreeResourceElementList.front();
	_pResource->writeLogEntry( _time, false );
	// make sure resource pool to Resource_Stay_In_Base
	_pResource->setCurrentRequestItem( m_ResourceRequestList.front() );
	_pResource->WalkAlongShortestPath( m_ResourceRequestList.front().request_point, _time);
	if (!bWait && m_emPoolType == PostServiceType)
	{
		m_ResourceRequestList.front().request_proc->noticeReleasePax(m_ResourceRequestList.front().request_mob,_time);
	}
	// remove resource and request from list
	m_FreeResourceElementList.pop_front();
	m_ResourceRequestList.pop_front();
	
	return true;
}

// when a resource back to base, add it to free element list
void CResourcePool::resourceBackToBase( ResourceElement* _pBackResource, const ElapsedTime& _time )
{
	assert( _pBackResource );

	m_FreeResourceElementList.push_back( _pBackResource );
	handleRequestIfCan( _time ,m_emPoolType,TRUE);
}

// get a random point in the base
Point CResourcePool::getServiceLocation( void ) const
{
	int _pointCount = m_poolBasePath.getCount();

	assert( _pointCount>0 );

	if( _pointCount == 1 )
	{
		return m_poolBasePath.getPoint( 0 );
	}

	if( _pointCount == 2 )
	{
		double _dLength = m_poolBasePath.getPoint(0).distance( m_poolBasePath.getPoint(1) );
		return m_poolBasePath.getRandPoint( _dLength );
	}

	if( _pointCount >=3 )
	{
		Pollygon base;
		base.init ( m_poolBasePath.getCount(), m_poolBasePath.getPointList() );
		return base.getRandPoint();
	}

	return Point();
}

// kill resource element and flush resource elemet log
void CResourcePool::killAndFlushLog( const ElapsedTime& _killTime )
{
	for( UINT i=0; i< m_AllResourceElementList.size(); i++ )
	{
		m_AllResourceElementList[i]->kill( _killTime );
	}
}
/************************************************************************/
/*  CResourcePoolDataSet class                                          */
/************************************************************************/

CResourcePoolDataSet::CResourcePoolDataSet() : DataSet( ResourecPoolFile, (float)2.3 )
{
	m_ResourcePoolList.clear();
}

CResourcePoolDataSet::~CResourcePoolDataSet()
{
	m_ResourcePoolList.clear();
}

// get ResourcePool count
int	CResourcePoolDataSet::getResourcePoolCount( void ) const
{
	return m_ResourcePoolList.size();
}

// get resource pool list
const RESOURCEPOOLSET& CResourcePoolDataSet::getResourcePoolList( void ) const
{
	return m_ResourcePoolList;
}

// add new pool
bool CResourcePoolDataSet::addNewPool( const CResourcePool& _pool )
{
	if( checkNameIfExist(_pool.getPoolName()) )
		return false;
	m_ResourcePoolList.insert( m_ResourcePoolList.end(), _pool );
	return true;
}

// check the _strName if already exist in the list
bool CResourcePoolDataSet::checkNameIfExist( const CString& _strName ) const
{
	CResourcePool pool( _strName );
	RESOURCEPOOLSET::const_iterator find_iter =  std::find( m_ResourcePoolList.begin(), m_ResourcePoolList.end(), pool );
	return find_iter != m_ResourcePoolList.end();
}

// get resource pool by name,
CResourcePool* CResourcePoolDataSet::getResourcePoolByName( const CString& _strName )
{
	//CResourcePool pool( _strName );
	for( UINT i=0; i<m_ResourcePoolList.size(); i++ )
	{
		if( m_ResourcePoolList[i].getPoolName() == _strName )
			return &m_ResourcePoolList[i];
	}
	
	return NULL;
}

// get resource pool by index
CResourcePool* CResourcePoolDataSet::getResourcePoolByIndex( int _idx )
{
	if( _idx>=0 && (UINT)_idx< m_ResourcePoolList.size() )
	{
		return &m_ResourcePoolList[_idx];
	}
	return NULL;
}

// remove pool by name
bool CResourcePoolDataSet::removeResourcePoolByName( const CString& _strName )
{
	CResourcePool pool( _strName );
	RESOURCEPOOLSET::iterator find_iter =  std::find( m_ResourcePoolList.begin(), m_ResourcePoolList.end(), pool );
	
	if( find_iter == m_ResourcePoolList.end() )
		return false;
	
	m_ResourcePoolList.erase( find_iter );
	return true;
}

// read & write file;
// empties the databases
void CResourcePoolDataSet::clear (void)
{
	m_ResourcePoolList.clear();
}
void CResourcePoolDataSet::readData2_2(ArctermFile& p_file)
{
	int _size;
	char szBuffer[512];
	ProbabilityDistribution* _pro;

	p_file.getLine();
	p_file.getInteger( _size );

	for( int i=0; i< _size; i++ )
	{
		m_ResourcePoolList.push_back( CResourcePool() );
		CResourcePool& pool= m_ResourcePoolList.back();

		p_file.getLine();
		p_file.getField( szBuffer, 512 );
		pool.setPoolName( CString( szBuffer ) );

		p_file.setToField( 1 );
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);
		pool.setProSpeed( _pro );

		p_file.setToField( 2 );
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);
		pool.setProCount( _pro );

		int _iPointCount;
		p_file.setToField( 3 );
		p_file.getInteger( _iPointCount );
		std::vector< Point > _poolBase;
		for( int i=0; i<_iPointCount; i++ )
		{
			Point pt;
			p_file.getLine();
			p_file.getPoint( pt );
			_poolBase.push_back( pt );
		}

		Path _path;
		_path.init( _iPointCount, (const Point*) (&_poolBase[0]) );
		pool.setPoolBasePath( _path );
	}
}
void CResourcePoolDataSet::readData (ArctermFile& p_file)
{
	int _size;
	char szBuffer[512];
	ProbabilityDistribution* _pro;

	p_file.getLine();
	p_file.getInteger( _size );

	for( int i=0; i< _size; i++ )
	{
		m_ResourcePoolList.push_back( CResourcePool() );
		CResourcePool& pool= m_ResourcePoolList.back();

		p_file.getLine();
		p_file.getField( szBuffer, 512 );
		pool.setPoolName( CString( szBuffer ) );
		
		p_file.setToField( 1 );
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);
		pool.setProSpeed( _pro );

		p_file.setToField( 2 );
		_pro = GetTerminalRelateProbDistribution (p_file,m_pInTerm);
		pool.setProCount( _pro );

		int _iPointCount;
		p_file.setToField( 3 );
		p_file.getInteger( _iPointCount );
		std::vector< Point > _poolBase;
		for( int i=0; i<_iPointCount; i++ )
		{
			Point pt;
			p_file.getLine();
			p_file.getPoint( pt );
			_poolBase.push_back( pt );
		}

		Path _path;
		_path.init( _iPointCount, (const Point*) (&_poolBase[0]) );
		pool.setPoolBasePath( _path );
		p_file.getLine();
		int nType;
		p_file.getInteger(nType);
		pool.setPoolType((PoolType)nType);
	}
	 
}	

void CResourcePoolDataSet::writeData (ArctermFile& p_file) const
{
	const ProbabilityDistribution* _pro = NULL;
	RESOURCEPOOLSET::const_iterator con_iter;
	// write size of the set
	int _size = m_ResourcePoolList.size();
	p_file.writeInt( _size );
	p_file.writeLine();

	for( con_iter = m_ResourcePoolList.begin(); con_iter != m_ResourcePoolList.end(); ++con_iter )
	{
		// name 
		p_file.writeField( con_iter->getPoolName() );
		// speed
		_pro = con_iter->getProSpeed();
		assert( _pro );
		_pro->writeDistribution( p_file );
		// count
		_pro = con_iter->getProCount();
		assert( _pro );
		_pro->writeDistribution( p_file );
		// base path
		int _PointCount = con_iter->getPoolBasePath().getCount();
		p_file.writeInt( _PointCount );
		p_file.writeLine();
		for( int i=0; i< _PointCount; i++ )
		{
			p_file.writePoint( con_iter->getPoolBasePath().getPoint(i) );
			p_file.writeLine();
		}
		p_file.writeInt((int)con_iter->getPoolType());
		p_file.writeLine();
	}
}
 
const char* CResourcePoolDataSet::getTitle (void) const
{ 
	return "all resource pool list"; 
}

const char* CResourcePoolDataSet::getHeaders (void) const
{ 
	return "Name, Speed, Count, PoolBasePath"; 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// for engine
// create resource element for each pool
long CResourcePoolDataSet::createResourceElement( Terminal* _pTerm, long _lId )
{
	PLACE_METHOD_TRACK_STRING();
	long _lElementNum = 0;

	for( UINT i=0; i< m_ResourcePoolList.size(); i++ )
	{
		_lElementNum += m_ResourcePoolList[i].generateAllResourceElement( _pTerm, _lId, i );
	}
	/*
	RESOURCEPOOLSET::iterator iter;	

	for( iter = m_ResourcePoolList.begin(); iter != m_ResourcePoolList.end(); ++iter )
	{
		_lElementNum += iter->generateAllResourceElement( _pTerm, _lId );
	}
	*/
	return _lElementNum;
}

// clear all pool's engine data
void CResourcePoolDataSet::clearAllPoolEngineData( const ElapsedTime& _killTime )
{
	PLACE_METHOD_TRACK_STRING();
	RESOURCEPOOLSET::iterator iter;	
	
	for( iter = m_ResourcePoolList.begin(); iter != m_ResourcePoolList.end(); ++iter )
	{
		iter->killAndFlushLog( _killTime );
		iter->clearEngineDate();
	}
	return;
}