// ProbDistManager.h: interface for the CProbDistManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROBDISTMANAGER_H__3A1DC87C_608D_4E33_A98C_FF345917013A__INCLUDED_)
#define AFX_PROBDISTMANAGER_H__3A1DC87C_608D_4E33_A98C_FF345917013A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProbDistEntry.h"
#include "..\common\DataSet.h"

class CAirportDatabase;
#define PROBDISTMANAGER_  
#include "../Database/ADODatabase.h"
class COMMON_TRANSFER CProbDistManager : public DataSet
{
public:

	// Description: Constructor
	// Exception:	FileVersionTooNewError
	CProbDistManager();

	virtual ~CProbDistManager();

public:
	void DeleteItem( int _nIdx );
	CString GetNewName();
	CString GetCopyName(CString _strOri);

	size_t getCount()const{ return m_vProbDist.size();	}
	CProbDistEntry* getItem( int _nIdx )const{ return m_vProbDist[_nIdx];	}
	CProbDistEntry* getItemByName(const CString& s)const;
	void AddItem( CProbDistEntry* _pPDEntry ){ m_vProbDist.push_back(_pPDEntry); }
	virtual void clear();

	virtual void readData( ArctermFile& _file );
	virtual void readObsoleteData( ArctermFile& _file ){ readData( _file ); }
	virtual void writeData( ArctermFile& p_file ) const;

	//current version read and save
	bool loadDatabase(CAirportDatabase* pAirportDatabase); //save the data to new version database
	bool saveDatabase(CAirportDatabase* pAirportDatabase);
	bool deleteDatabase(CAirportDatabase* pAirportDatabase);

	virtual const char *getTitle() const;
	virtual const char *getHeaders() const;

private:
	CPROBDISTLIST m_vProbDist;
//the interface for database 
//////////////////////////////////////////////////////////////////////////
public:
	void ReadDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
	void WriteDataToDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
public:
	void ResetAllID() ;
	void DeleteAllDataFromDB(int _airportDBID,DATABASESOURCE_TYPE type = DATABASESOURCE_TYPE_ACCESS) ;
};

//global function

#endif // !defined(AFX_PROBDISTMANAGER_H__3A1DC87C_608D_4E33_A98C_FF345917013A__INCLUDED_)
