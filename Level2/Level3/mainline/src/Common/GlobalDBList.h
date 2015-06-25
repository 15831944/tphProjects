// GlobalDBList.h: interface for the CGlobalDBList class.
//
// Keep list of GlobalDB entry: id, Name, Folder
// Keep list of GlobalDB
// 
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GLOBALDBLIST_H__62C0A57B_6E85_4898_A3C4_079A0D8C5A34__INCLUDED_)
#define AFX_GLOBALDBLIST_H__62C0A57B_6E85_4898_A3C4_079A0D8C5A34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// include
#include "DATASET.H"
#include <vector>

// declare
class CGlobalDB;
typedef std::vector< CGlobalDB* > GLDB_LIST;

#define GOLBALDBLIST CGlobalDBList::GetInstance()
#define DESTROYGOLBLDBLIST CGlobalDBList::DeleteInstance()

/************************************************************************/
/*                      class CGlobalDBList                             */
/************************************************************************/
class CGlobalDBList : public DataSet  
{
protected:
	CGlobalDBList();
	virtual ~CGlobalDBList();
	
public:
	static CGlobalDBList* GetInstance( void );
	static void DeleteInstance( void );
	// Init Value in case no file can be read.
    virtual void initDefaultValues();
    
	// clear data structure before load data from the file.
	virtual void clear();
	
	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	
	virtual const char *getTitle (void) const;
	virtual const char *getHeaders (void) const;

private:
	GLDB_LIST m_vGlobalDB;
	long	  m_lGlobalIndex;			
	static CGlobalDBList* m_pInstance;

private:
	// check if have db file/path exist in the destination path
	bool checkDBIfExist( const CString& _strDestPath ) const;
	
	// get unique name
	CString getUniqueName( const CString& _strName );
public:
	// Given ID, return GlobalDB;
	CGlobalDB* getGlobalDBByID( int _iIdx );

	// get the size of global db
	int getDBCount( void ) const;
	
	// get DB by index
	CGlobalDB* getGlobalDBByIndex( int _idx );

	// copy db 
	bool CopyDB( const CGlobalDB* _pRscDB, const CString& _strDestPath, CGlobalDB** _pDestDb );

	// delete db
	bool DeleteDB( const CGlobalDB* _pDelDB, bool _bDeletePath = false );

	// check name if repeat
	bool checkNameIfRepeat( const CString& _strName, const CGlobalDB* _pGLDB );

	// get all db file name
	void getAllDBFileName( CGlobalDB* _pGLDB, std::vector< CString >& _vAllFileName ) const;

	// add a new db entry
	long addNewDBEntry( const CString& _strDBName, const CString& _strDBPath, bool _bNeedToSaveData = true );
	
	// get db index by name
	long getIDByName( const CString& _strDBName );
};

#endif // !defined(AFX_GLOBALDBLIST_H__62C0A57B_6E85_4898_A3C4_079A0D8C5A34__INCLUDED_)
