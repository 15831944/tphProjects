// UserName.h: interface for the CUserName class.
#pragma once
#include "common\dataset.h"

class ArctermFile;
class ProcessorID;
class InputTerminal;

class CUserName  
{
private:
	CString				m_strName;
	int					m_iSeqStart;
	int					m_iSeqEnd;
	int					m_iNum;
	CString				m_strFileName;
	std::vector<int>	m_vAllSeqNum;
public:
	CUserName();
	virtual ~CUserName();

	void setName( const CString& _strName ) { m_strName = _strName;	}
	void setSeqStart( int _iSeqStart ) { m_iSeqStart = _iSeqStart;	}
	void setSeqEnd( int _iSeqEnd ) { m_iSeqEnd = _iSeqEnd;	}
	void setNum( int _iNum ) { m_iNum = _iNum;	}
	void setFileName( const CString& _strFileName ) { m_strFileName = _strFileName; }
	void setSeqNumList( const std::vector<int>& _vSeqNum ) { m_vAllSeqNum = _vSeqNum; }

	const CString& getName( void ) const { return m_strName;	}
	int getSeqStart( void ) const { return m_iSeqStart;	}
	int getSeqEnd( void ) const { return m_iSeqEnd;	}
	int getNum( void ) const { return m_iNum;}
	const CString& getFilaName( void ) const { return m_strFileName; }
	const std::vector<int>& getSeqNumList( void ) const { return m_vAllSeqNum;}

	void writeDataToFile( ArctermFile& _file ) const;
	void readDataFromFile( ArctermFile& _file );

	bool operator==(const CUserName& _name);
	bool operator==(const CString& _name);
};

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class CUserNameDataSet :public DataSet
{
private:
	std::map<CString, CUserName> m_mapModelNameToUserName;

public:
	CUserNameDataSet(  );
	virtual ~CUserNameDataSet();
	
	const std::map< CString, CUserName>& getModelNameToUserNameMap();
	bool ifExistInUserList( const ProcessorID& _procID );
	bool ifExistInUserList( const CString& _strProcID );	
	bool printUserName( const CString& _strProcID, std::vector<CString>& _vectString );
	bool addUserName( const ProcessorID& _procID );
	bool isProcIDValid( const ProcessorID& _procID );
	bool modifyUseName( const ProcessorID& _procID, const CUserName& _userName);
	bool deleteUserName( const ProcessorID& _procID );
	bool canSave();
	//////////////////////////////////////////////////////////////////////////
	// data set virtual function
	// clear data structure before load data from the file.
	virtual void clear();
	virtual void readData( ArctermFile& p_file );
	virtual void writeData( ArctermFile& p_file ) const;
	
	virtual const char *getTitle () const
	{
		return "User Name List DataSet";
	}
	virtual const char *getHeaders () const
	{
		return "The Map Of Model Name To User Name";
	}
private:
	bool canModifyUseName(const ProcessorID& _procID, const CUserName& _userName);

//////////////////////////////////////////////////////////////////////////////////	
// a!
public:
	bool findProcNameByUserName( CString& _strUserName ,CString& _strProcName ) const;
	bool userNameIfExist( const CString& _strUserName ) const;
	bool getUserNameByProcessorName( const CString& _strProcName, CString& _strUserName ) ;
};
