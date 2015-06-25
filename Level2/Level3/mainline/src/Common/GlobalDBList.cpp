// GlobalDBList.cpp: implementation of the CGlobalDBList class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "globaldblist.h"
#include "globaldb.h"
#include "projectmanager.h"
#include "common\fileman.h"
#include "assert.h"

CGlobalDBList* CGlobalDBList::m_pInstance = NULL;

#define DB_FILE_COUNT 7
const CString __database_file[] =
{
	"acdata.acd",
	"category.cat",
	"Airlines.dat",
	"subairlines.sal",
	"Airports.dat",
	"Sector.sec",
	"probdist.pbd"
};

#define DB_PATH_COUNT 0
const CString __database_path[] = 
{
	"MobImages",
	"Shapes"
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGlobalDBList::CGlobalDBList()
	:DataSet( GlobalDBListFile ), m_lGlobalIndex(0l)
{
	
}

CGlobalDBList::~CGlobalDBList()
{
	clear();
}

CGlobalDBList* CGlobalDBList::GetInstance( void )
{
	if( m_pInstance == NULL )
	{
		m_pInstance = new CGlobalDBList();
		m_pInstance->loadDataSet("");
	}
	
	return m_pInstance;
}

void CGlobalDBList::DeleteInstance( void )
{
	if( m_pInstance )
	{
		m_pInstance->saveDataSet("", false );
		delete m_pInstance;
	}

	m_pInstance = NULL;
}

// clear data structure before load data from the file.
void CGlobalDBList::clear()
{
	for( unsigned i=0; i<m_vGlobalDB.size(); i++ )
	{
		delete m_vGlobalDB[i];
	}
	m_vGlobalDB.clear();
}


// Init Value in case no file can be read.
void CGlobalDBList::initDefaultValues()
{
	clear();

	m_lGlobalIndex = 0l;

	CGlobalDB* _pDB = new CGlobalDB;
	_pDB->setName("DEFAULT DB");
	_pDB->setFolder( PROJMANAGER->GetDefaultDBPath() );
	_pDB->setIndex( m_lGlobalIndex++ );

	m_vGlobalDB.push_back( _pDB );
}

void CGlobalDBList::readData (ArctermFile& p_file)
{
	int _iCount =0;

	p_file.getLine();
	p_file.getInteger( m_lGlobalIndex );
	p_file.getInteger( _iCount );
	
	for( int i=0; i< _iCount; i++ )
	{
		CGlobalDB* _pDB = new CGlobalDB();
		_pDB->loadFromFile( p_file );
		
		m_vGlobalDB.push_back( _pDB );
	}
}


void CGlobalDBList::writeData (ArctermFile& p_file) const
{
	p_file.writeInt( m_lGlobalIndex );
	p_file.writeInt( (int)m_vGlobalDB.size() );
	p_file.writeLine();
	
	for( unsigned i=0; i< m_vGlobalDB.size(); i++ )
	{
		m_vGlobalDB[i]->saveToFile( p_file );
	}
}

const char* CGlobalDBList::getTitle (void) const
{
	return "GLOBAL DB LIST";
}

const char* CGlobalDBList::getHeaders (void) const
{
	return "ID,NAME,FOLDER LOCATION";
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// Given ID, return GlobalDB;
CGlobalDB* CGlobalDBList::getGlobalDBByID( int _iIdx )
{
	int _iCount = m_vGlobalDB.size();
	for( int i=0; i<_iCount; i++ )
	{
		if( m_vGlobalDB[i]->getIndex() == _iIdx )
			return m_vGlobalDB[i];
	}

	return NULL;
}

// get the size of global db
int CGlobalDBList::getDBCount( void ) const
{
	return m_vGlobalDB.size();
}

// get DB by index
CGlobalDB* CGlobalDBList::getGlobalDBByIndex( int _idx )
{
	assert( _idx >=0 && (unsigned)_idx < m_vGlobalDB.size() );
	
	return m_vGlobalDB[_idx ];
}

// check if have db file/path exist in the destination path
bool CGlobalDBList::checkDBIfExist( const CString& _strDestPath ) const
{
	FileManager fm;

	// first check directory
	int i;
	for( i=0; i< DB_PATH_COUNT; i++ )
	{
		CString _strPathName = _strDestPath + "\\" + __database_path[i];
		if( fm.IsDirectory( _strPathName) )
			return true;
	}

	// then check file
	for( i=0; i< DB_FILE_COUNT; i++ )
	{
		CString _strFileName = _strDestPath + "\\" + __database_file[i];
		if( fm.IsFile( _strFileName))
			return true;
	}

	return false;
}

// copy db 
bool CGlobalDBList::CopyDB( const CGlobalDB* _pRscDB, const CString& _strDestPath, CGlobalDB** _pDestDb )
{
	assert( _pRscDB );
	
	bool _bReplace = true;
	if( checkDBIfExist( _strDestPath ) )	
	{
		CString _strMsg;
		_strMsg.Format("There are some DB file exist in the path: %s \r\nDo you want to replace they?",_strDestPath );
		
		int _iReturn = AfxMessageBox( _strMsg, MB_YESNOCANCEL | MB_ICONQUESTION );
		switch( _iReturn )
		{
		case IDYES:
			_bReplace = true;
			break;

		case IDNO:
			_bReplace = false;
			break;

		case IDCANCEL:
			return false;

		default:
			break;
		}
	}

	FileManager fm;
	// first copy directory
	for( int i=0; i< DB_PATH_COUNT; i++  )
	{
		CString strPathName = _strDestPath + "\\" + __database_path[i];
		if( fm.IsDirectory( strPathName) )	// the directory already exist
		{
			if(  _bReplace )
			{
				fm.DeleteDirectory( strPathName );
				fm.MakePath( strPathName );
			}
			else
			{
				continue;
			}
		}
		else
		{
			fm.MakePath( strPathName );
		}

		fm.CopyDirectory( _pRscDB->getFolder() + "\\" + __database_path[i], strPathName ,NULL);
	}
	
	// then copy file
	for( int k = 0; k< DB_FILE_COUNT; k++ )
	{
		CString strFileName = _strDestPath + "\\" + __database_file[k];

		fm.CopyFile( _pRscDB->getFolder() + "\\" + __database_file[k], strFileName, _bReplace );
	}

	// create a new db add add it to dblist
	CGlobalDB* _pDB = new CGlobalDB;
	_pDB->setIndex( m_lGlobalIndex ++ );
	_pDB->setName( getUniqueName( "COPY OF "+ _pRscDB->getName() ) );
	_pDB->setFolder( _strDestPath );
	m_vGlobalDB.push_back( _pDB );
	
	*_pDestDb = _pDB; 
	return true;
}

// delete db
bool CGlobalDBList::DeleteDB( const CGlobalDB* _pDelDB, bool _bDeletePath )
{
	// delete file /path
	if( _bDeletePath )
	{
		FileManager fm;
		// first delete path
		int i=0;
		for( i=0; i< DB_PATH_COUNT; i++ )
		{
			CString strPathName = _pDelDB->getFolder() + "\\" + __database_path[i];
			fm.DeleteDirectory( strPathName );
		}

		// the delete file
		for( i=0; i< DB_FILE_COUNT; i++ )
		{
			CString strFileName = _pDelDB->getFolder() + "\\" + __database_file[i];
			fm.DeleteFile( strFileName );
		}
	}
	
	// delet from DB list
	for( unsigned k=0; k<m_vGlobalDB.size(); k++ )
	{
		if( m_vGlobalDB[k] == _pDelDB )
		{
			m_vGlobalDB.erase( m_vGlobalDB.begin() + k );
			delete _pDelDB;
		}
	}

	return true;
}

// check name if repeat
bool CGlobalDBList::checkNameIfRepeat( const CString& _strName, const CGlobalDB* _pGLDB  )
{
	for( unsigned i=0; i< m_vGlobalDB.size(); i++ )
	{
		if( _pGLDB!= m_vGlobalDB[i] && m_vGlobalDB[i]->getName() == _strName )
		{
			return true;
		}
	}

	return false;
}

// get unique name
CString CGlobalDBList::getUniqueName( const CString& _strName )
{
	int _iCount = 0;
	for( unsigned i=0; i< m_vGlobalDB.size(); i++ )
	{
		CString _strDBName = m_vGlobalDB[i]->getName();
		_strDBName = _strDBName.Left( _strName.GetLength() );
		if( _strName.CompareNoCase( _strDBName ) == 0 )
			_iCount++;
	}

	if( _iCount == 0 )
		return _strName;

	CString _strReturnName;
	_strReturnName.Format("%s(%d)",_strName, _iCount );
	return _strReturnName;
}

// get all db file name
void CGlobalDBList::getAllDBFileName( CGlobalDB* _pGLDB, std::vector< CString >& _vAllFileName ) const
{
	assert( _pGLDB );

	for( int i=0; i< DB_FILE_COUNT; i++ )
	{
		_vAllFileName.push_back( _pGLDB->getFolder() + "\\" + __database_file[i] );
	}
}

// add a new db entry
long CGlobalDBList::addNewDBEntry( const CString& _strDBName, const CString& _strDBPath, bool _bNeedToSaveData )
{
	CGlobalDB* _pDB = new CGlobalDB;
	_pDB->setIndex( m_lGlobalIndex ++ );
	_pDB->setName( getUniqueName( _strDBName ) );
	_pDB->setFolder( _strDBPath );
	m_vGlobalDB.push_back( _pDB );
	
	if( _bNeedToSaveData )
		m_pInstance->saveDataSet("", false );

	return _pDB->getIndex();
}

// get db index by name
long CGlobalDBList::getIDByName( const CString& _strDBName )
{
	for( unsigned i=0; i< m_vGlobalDB.size(); i++ )
	{
		if( m_vGlobalDB.at(i)->getName() == _strDBName )
		{
			return m_vGlobalDB.at(i)->getIndex();
		}
	}

	return 0l;
}
