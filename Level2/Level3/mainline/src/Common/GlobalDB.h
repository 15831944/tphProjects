// GlobalDB.h: interface for the CGlobalDB class.
//
// Is a global db,
// id,
// name,
// folder
// airport and sector
// airline and airline group
// actype and ac category
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALDB_H__6B1915EF_3431_44A9_BB52_5B52D2A837A4__INCLUDED_)
#define AFX_GLOBALDB_H__6B1915EF_3431_44A9_BB52_5B52D2A837A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include

// declare
class CACTypesManager;
class CAirlinesManager;
class CAirportsManager;
class CProbDistManager;
class ArctermFile;
/************************************************************************/
/*                          class CGlobalDB                             */
/************************************************************************/
class CGlobalDB  
{
public:
	CGlobalDB();
	virtual ~CGlobalDB();

private:
	long			  m_lIndex;			// unique index
	CString			  m_strName;		// name
	CString			  m_strFolder;		// folder path
	CACTypesManager*  m_pAcMan;			
	CAirlinesManager* m_pAirlineMan;
	CAirportsManager* m_pAirportMan;
	CProbDistManager* m_pProbDistMan;

public:
	// set & get
	void setIndex( long _idx );
	void setName( const CString& _strName );
	void setFolder( const CString& _strFolder );
	
	long getIndex( void ) const;
	const CString& getName( void ) const;
	const CString& getFolder( void ) const;
	CACTypesManager*	getAcMan( void );
	CAirlinesManager*	getAirlineMan( void );
	CAirportsManager*	getAirportMan( void );
	CProbDistManager*	getProbDistMan( void );

	// save & load
	void saveToFile( ArctermFile& file ) const;
	void loadFromFile( ArctermFile& file );

	// free **Manager pointer 
	void clearData();
	
	// check if have load **Manager data 
	bool hadLoadDatabase() const;
	
	// load **Mamger data
	bool loadDatabase();
};

#endif // !defined(AFX_GLOBALDB_H__6B1915EF_3431_44A9_BB52_5B52D2A837A4__INCLUDED_)
