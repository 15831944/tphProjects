// ResourcePool.h: interface for the CResourcePool class.
#pragma once
#include "common\path.h"
#include "common\dataset.h"
#include "common\elaptime.h"

// declare
class ProbabilityDistribution;
class ResourceElement;
class Terminal;
class Processor;
enum PoolType
{
	ConcurrentType,
	PostServiceType
};

struct ResourceRequestItem
{
public:
	MobileElement*	request_mob;
	Processor*		request_proc;
	ElapsedTime		request_time;
	long			request_servicetime;	// (s)
	Point			request_point;

public:
	void init()
	{ 
		request_mob  = NULL;
		request_proc = NULL;
		request_time = 0l;
		request_servicetime	 = 0l;
		request_point = Point();
	};
};

/************************************************************************/
/*                              CResourcePool                           */
/************************************************************************/
class CResourcePool  
{
public:
	CResourcePool();
	CResourcePool( const CString& _str );
	virtual ~CResourcePool();
	CResourcePool( const CResourcePool& _pool);
	CResourcePool& operator=( const CResourcePool& _pool );

// data member
private:
	CString						m_strPoolName;
	ProbabilityDistribution*	m_proSpeed;
	ProbabilityDistribution*	m_proCount;
	Path						m_poolBasePath;
	PoolType					m_emPoolType;

// function
private:
	double getRandomSpeed( void ) const;
	int	 getRandomCount( void ) const;

public:	
	// get & set 
	const PoolType& getPoolType(void) const;
	const CString& getPoolName( void ) const;
	const ProbabilityDistribution* getProSpeed( void ) const;
	const ProbabilityDistribution* getProCount( void ) const;
	const Path& getPoolBasePath( void ) const;

	void setPoolType(const PoolType& emPoolType);
	void setPoolName( const CString& _strName );
	void setProSpeed( ProbabilityDistribution* _pro );
	void setProCount( ProbabilityDistribution* _pro );
	void setPoolBasePath( const Path& _path );

	// for sort
	bool operator<( const CResourcePool& _pool) const;
	bool operator==( const CResourcePool& _pool ) const;
	

	//////////////////////////////////////////////////////////////////////////
	// for engine
private:
	// all child resource
	std::vector< ResourceElement* > m_AllResourceElementList;

	// all free resource list
	std::list< ResourceElement* > m_FreeResourceElementList;

	// resource request list
	std::list< ResourceRequestItem > m_ResourceRequestList;

private:	
	// handle request if probability
	bool handleRequestIfCan( const ElapsedTime& _time ,const PoolType& emPoolType,BOOL bWait);

public:
	// clear engine data
	void clearEngineDate( void );

	// kill resource element and flush resource elemet log
	void killAndFlushLog( const ElapsedTime& _killTime );

	// get all resource count
	int getAllResourceCount( void ) const;

	// get free resource count
	int getFreeResourceCount( void ) const;

	// generate resource for pool
	int generateAllResourceElement( Terminal* _pTerm, long& _lId, int _poolIdx );

	// get resource request num
	int getCurrentRequestNum( void ) const;

	// add a resource request to list
	void addRequestToList( const ResourceRequestItem& _item, const ElapsedTime& _time );
	void addRequestToList( const ResourceRequestItem& _item);
	// when a resource back to base, add it to free element list
	void resourceBackToBase( ResourceElement* _pBackResource, const ElapsedTime& _time );
	
	// get a random point in the base
	Point getServiceLocation( void ) const;
};


/************************************************************************/
/*                       CResourcePoolDataSet	                        */
/************************************************************************/
// declare
typedef std::vector< CResourcePool > RESOURCEPOOLSET;
 
class CResourcePoolDataSet : public DataSet
{
public:
	CResourcePoolDataSet();
	virtual ~CResourcePoolDataSet();

private:
	RESOURCEPOOLSET m_ResourcePoolList;

public:
	// get ResourcePool count
	int	getResourcePoolCount( void ) const;

	// get resource pool list
	const RESOURCEPOOLSET& getResourcePoolList( void ) const;

	// add new pool
	bool addNewPool( const CResourcePool& _pool );

	// check the _strName if already exist in the list
	bool checkNameIfExist( const CString& _strName ) const;

	// get resource pool by name,
	CResourcePool* getResourcePoolByName( const CString& _strName );

	// get resource pool by index
	CResourcePool* getResourcePoolByIndex( int _idx );
	
	// remove pool by name
	bool removeResourcePoolByName( const CString& _strName );

	// read & write file;
    virtual void clear (void);
    virtual void readData (ArctermFile& p_file);
	void readData2_2(ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData ( ArctermFile& p_file ){readData2_2(p_file);}
	virtual const char *getTitle (void) const;
    virtual const char *getHeaders (void) const;	

//////////////////////////////////////////////////////////////////////////
// for engine
public:
	// create resource element for each pool
	long  createResourceElement( Terminal* _pTerm, long _lId );

	// clear all pool's engine data
	void clearAllPoolEngineData( const ElapsedTime& _killTime );
};
