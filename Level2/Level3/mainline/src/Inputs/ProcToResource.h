// ProcToResource.h: interface for the CProcToResource class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROCTORESOURCE_H__636CEE79_1491_433D_9CAF_762986B317B4__INCLUDED_)
#define AFX_PROCTORESOURCE_H__636CEE79_1491_433D_9CAF_762986B317B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include <set>
#include <CommonData/procid.h>
#include "common\dataset.h"
#include "inputs\MobileElemConstraint.h"


// declare
class CResourcePool;
class ProbabilityDistribution;

#define USE_NOTHING				0
#define USE_PIPE_SYSTEM			1
#define USE_USER_SELECTED_PIPES	2
typedef std::vector<int> PIPES;
class CProcToResource  
{
public:
	CProcToResource();
	CProcToResource( const CProcToResource& _prc2Res );
	CProcToResource( const ProcessorID& _id, const CMobileElemConstraint& _mob, const CString& _strName = "", ProbabilityDistribution* _pro = NULL );
	CProcToResource& operator=( const CProcToResource& _prc2Res ); 
	virtual ~CProcToResource();

// date member
private:
	ProcessorID m_procID;
	CMobileElemConstraint m_mobileCon;
	CString		m_strResourcePoolName;
	ProbabilityDistribution* m_proServiceTime;
	int m_iTypeOfUsingPipe;	
	// m_iTypeOfUsingPipe = 0 use nothing
	// m_iTypeOfUsingPipe = 1 use pipe system
	// m_iTypeOfUsingPipe = 2 use user selected pipes
	PIPES m_vUsedPipes;
	BOOL m_isChecked; // is checked in engine setting

public:
	// get & set
	const ProcessorID& getProcessorID( void ) const;
	const CMobileElemConstraint& getMobileConstraint( void ) const;
	const CString& getResourcePoolName( void ) const;
	const ProbabilityDistribution* getProServiceTime( void ) const;
	BOOL getChecked() const { return m_isChecked; }

	void setProcessorID( const ProcessorID& _procID );
	void setMobileElemConstraint( const CMobileElemConstraint& _mob);
	void setResourcePoolName( const CString& _strPoolName );
	void setServiceTime( ProbabilityDistribution* _pro );
	void setChecked(BOOL val) { m_isChecked = val; }
	
	long getRandomServiceTime( void ) const;

	// Route condition
	void SetTypeOfUsingPipe( int _iType ) { m_iTypeOfUsingPipe = _iType; }
	int GetTypeOfUsingPipe() const { return m_iTypeOfUsingPipe;	}

	void AddUsedPipe(int _iIdxOfPipe ){ m_vUsedPipes .push_back( _iIdxOfPipe );}
	void ClearAllPipe(){ m_vUsedPipes.clear();}
	PIPES& GetPipeVector() { return m_vUsedPipes;}
	int GetUsedPipeCount() const { return m_vUsedPipes.size();	}
	int GetPipeAt( int _iIdx ) const ;
	CString GetPipeString(InputTerminal* pTerm) const;

	// operator
	bool operator<( const CProcToResource& _proc2Res )  const;
	bool operator==( const CProcToResource& _proc2Res ) const;
};


/************************************************************************/
/*        CProcToResourceDataSet class                                  */
/************************************************************************/
typedef std::multiset< CProcToResource > PROC2RESSET;

class CProcToResourceDataSet : public DataSet
{
public:
	CProcToResourceDataSet();
	virtual ~CProcToResourceDataSet();

private:
	PROC2RESSET m_Proc2ResList;
	BOOL m_isChecked; // is checked in engine setting
public:
	// get proc to resource list's size
	int getProc2ResListSize( void ) const;

	// get proc to resource list
	PROC2RESSET& getProc2ResList( void );

	// get best resource pool
	bool getBestResourcePoolAndTime( const ProcessorID& _procID, const CMobileElemConstraint& _mobCon, CResourcePool* _resPool, long& _lTime );

	// add new proc2pool to dataset
	bool addNewProcToRes( const CProcToResource& _proc2Res );

	// if exist in the list
	bool ifRepeat( const ProcessorID& _procID, const CMobileElemConstraint& _mobCon ) const; 
	
	// remove item by name
	bool removeItemByName( const CString& _strProcID, const CString& _strPaxType );

	// get item by name
	CProcToResource* getItemByName( const CString& _strProcID, const CString& _strPaxType );

	// modify paxType 
	bool modifyItemPaxType( const CString& _strProcID, const CString& _strPaxType, const CMobileElemConstraint& _mobCon );

	// get checked flag
	BOOL getChecked() const { return m_isChecked; }

	// set checked flag
	void setChecked(BOOL val) { m_isChecked = val; }

	//read and write file
	virtual void clear (void);
    virtual void readData (ArctermFile& p_file);
	virtual void readObsoleteData (ArctermFile& p_file);
    virtual void writeData (ArctermFile& p_file) const;
	
	virtual const char *getTitle (void) const;
    virtual const char *getHeaders (void) const;	
private:
	void readObsoleteData22 (ArctermFile& p_file);
	void readObsoleteData23 (ArctermFile& p_file);
};
#endif // !defined(AFX_PROCTORESOURCE_H__636CEE79_1491_433D_9CAF_762986B317B4__INCLUDED_)
