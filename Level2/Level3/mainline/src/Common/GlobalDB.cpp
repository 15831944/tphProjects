// GlobalDB.cpp: implementation of the CGlobalDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GlobalDB.h"
#include "ACTypesManager.h"
#include "AirlineManager.h"
#include "AirportsManager.h"
#include "probdistmanager.h"
#include "common\termfile.h"
#include "exeption.h"

// define
static const CString _strProbDistFileName	= "probdist.pbd";

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalDB::CGlobalDB():m_lIndex(-1l),m_pAcMan( NULL), m_pAirlineMan( NULL ), m_pAirportMan( NULL ), m_pProbDistMan (NULL)
{

}

CGlobalDB::~CGlobalDB()
{
	clearData();
}


// set & get
void CGlobalDB::setIndex( long _idx )
{
	m_lIndex = _idx;
}

void CGlobalDB::setName( const CString& _strName )
{
	m_strName = _strName;
}

void CGlobalDB::setFolder( const CString& _strFolder )
{
	m_strFolder = _strFolder;
}

long CGlobalDB::getIndex( void ) const
{
	return m_lIndex;
}

const CString& CGlobalDB::getName( void ) const
{
	return m_strName;
}

const CString& CGlobalDB::getFolder( void ) const
{
	return m_strFolder;
}

CACTypesManager* CGlobalDB::getAcMan( void )
{
	return m_pAcMan;
}

CAirlinesManager* CGlobalDB::getAirlineMan( void )
{
	return m_pAirlineMan;
}

CAirportsManager* CGlobalDB::getAirportMan( void )
{
	return m_pAirportMan;
}

CProbDistManager* CGlobalDB::getProbDistMan( void )
{
	return m_pProbDistMan;
}

// save & load
void CGlobalDB::saveToFile( ArctermFile& file ) const
{
	file.writeInt( m_lIndex );
	file.writeField( m_strName );
	file.writeField( m_strFolder );
	file.writeLine();
}

void CGlobalDB::loadFromFile( ArctermFile& file )
{
	file.getLine();
	file.getInteger( m_lIndex );
	file.getField( m_strName.GetBuffer( 128 ), 128 );
	file.getField( m_strFolder.GetBuffer( 128), 128 );
	m_strName.ReleaseBuffer();
	m_strFolder.ReleaseBuffer();
}

// free **Manager pointer 
void CGlobalDB::clearData()
{
	if( m_pAcMan )
	{
		delete m_pAcMan;
		m_pAcMan = NULL;
	}

	if( m_pAirlineMan )
	{
		delete m_pAirlineMan;
		m_pAirlineMan = NULL;
	}

	if( m_pAirportMan )
	{
		delete m_pAirportMan;
		m_pAirportMan = NULL;
	}
	
	if( m_pProbDistMan )
	{
		m_pProbDistMan->saveDataSetToOtherFile(  m_strFolder + "\\" + _strProbDistFileName  );
		delete m_pProbDistMan;
		m_pProbDistMan = NULL;
	}
}

// check if have load **Manager data 
bool CGlobalDB::hadLoadDatabase() const
{
	return m_pAirlineMan!=NULL && m_pAcMan!=NULL && m_pAirportMan!= NULL && m_pProbDistMan!=NULL;
}

// load **Mamger data
bool CGlobalDB::loadDatabase()
{
	/*
	m_pAcMan = new CACTypesManager();
		
	m_pAirlineMan = new CAirlinesManager();

	m_pAirportMan = new CAirportsManager();

	m_pProbDistMan = new CProbDistManager();

	// read ...
	if( !m_pAcMan->LoadData( m_strFolder) ||
		!m_pAirlineMan->LoadData( m_strFolder) ||
		!m_pAirportMan->LoadData( m_strFolder ) )
	{
		clearData();
		return false;
	}

	// read prob dist
	try
	{
		m_pProbDistMan->loadDataSetFromOtherFile( m_strFolder + "\\" + _strProbDistFileName );
	}
	catch (FileVersionTooNewError* e) 
	{
		CString _strErrMsg;
		e->getMessage( _strErrMsg.GetBuffer(64) );
		AfxMessageBox( _strErrMsg, MB_OK|MB_ICONINFORMATION );
		
		delete e;
		_strErrMsg.ReleaseBuffer();
	}

	return true;
	*/
//	clearData();

// 	m_pAcMan = new CACTypesManager(this);
// 	m_pAcMan->LoadData( m_strFolder);
// 
// 	m_pAirlineMan = new CAirlinesManager(this);
// 	m_pAirlineMan->LoadData( m_strFolder );
// 
// 	m_pAirportMan = new CAirportsManager(this);
// 	m_pAirportMan->LoadData( m_strFolder );
// 
// 	m_pProbDistMan = new CProbDistManager(this);
// 	try
// 	{
// 		m_pProbDistMan->loadDataSetFromOtherFile( m_strFolder + "\\" + _strProbDistFileName );
// 	}
// 	catch (FileVersionTooNewError* e) 
// 	{
// 		CString _strErrMsg;
// 		e->getMessage( _strErrMsg.GetBuffer(64) );
// 		AfxMessageBox( _strErrMsg, MB_OK|MB_ICONINFORMATION );
// 		
// 		delete e;
// 		_strErrMsg.ReleaseBuffer();
// 	}
// 	
	return true;
}