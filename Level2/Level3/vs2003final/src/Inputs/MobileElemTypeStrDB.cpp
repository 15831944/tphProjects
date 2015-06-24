// MobileElemTypeStrDB.cpp: implementation of the CMobileElemTypeStrDB class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ASSERT.h"
#include "MobileElemTypeStrDB.h"
#include <algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMobileElemTypeStrDB::CMobileElemTypeStrDB()
:DataSet( MobileElemTypeStrDBFile, 2.7f)

{
	
}

CMobileElemTypeStrDB::~CMobileElemTypeStrDB()
{

}

CString CMobileElemTypeStrDB::GetString( int _nIdx )
{
	ASSERT( _nIdx>=0 && _nIdx< static_cast<int>(m_csList.size()) );
	return m_csList[_nIdx];
}

// return -1 for no entry
int CMobileElemTypeStrDB::GetIndex( CString _csStr )
{
	for( unsigned i=0; i<m_csList.size(); i++ )
	{
		if(_csStr == m_csList[i])
			return i;
	}

	return -1;
}


void CMobileElemTypeStrDB::RemoveAt( int _nIdx )
{
	ASSERT(0);
	m_csList.erase( m_csList.begin()+_nIdx);
}

void CMobileElemTypeStrDB::AddElem( CString& _csStr)
{
	ASSERT(0);
	m_csList.push_back( _csStr );
}

void CMobileElemTypeStrDB::ModifyElem( int _indx, const CString& _csStr )
{
	ASSERT( _indx>=0 && _indx < static_cast<int>(m_csList.size()) );
	m_csList[_indx] = _csStr;
}

// clear data structure before load data from the file.
void CMobileElemTypeStrDB::clear()
{
	m_csList.clear();
	m_csList.push_back ("PASSENGER");
}


void CMobileElemTypeStrDB::readData(ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}
	if( m_csList.size() != ALLPAX_COUNT )
	{
		initDefaultValues();
	}
}
void CMobileElemTypeStrDB::read2_6Data( ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}

	// get the tailing three items: USER DEFINE1, USER DEFINE2, USER DEFINE3
	CString strUserDefine3 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine2 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine1 = m_csList.back();
	m_csList.pop_back();

	m_csList.push_back("PURSE");
	m_csList.push_back("DUTYFREE");

	// at last, push the three user define items to the back of the vector
	m_csList.push_back(strUserDefine1);
	m_csList.push_back(strUserDefine2);
	m_csList.push_back(strUserDefine3);

}

void CMobileElemTypeStrDB::read2_5Data( ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}

	// get the tailing three items: USER DEFINE1, USER DEFINE2, USER DEFINE3
	CString strUserDefine3 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine2 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine1 = m_csList.back();
	m_csList.pop_back();

	m_csList.push_back("LAG BAG");
	m_csList.push_back("PURSE");
	m_csList.push_back("DUTYFREE");
	// at last, push the three user define items to the back of the vector
	m_csList.push_back(strUserDefine1);
	m_csList.push_back(strUserDefine2);
	m_csList.push_back(strUserDefine3);

}

void CMobileElemTypeStrDB::read2_4Data(ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}

	// get the tailing three items: USER DEFINE1, USER DEFINE2, USER DEFINE3
	CString strUserDefine3 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine2 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine1 = m_csList.back();
	m_csList.pop_back();

	m_csList.push_back("TRAY");
	m_csList.push_back("BACKPACK");
	m_csList.push_back("LAG BAG");
	m_csList.push_back("PURSE");
	m_csList.push_back("DUTYFREE");
	// at last, push the three user define items to the back of the vector
	m_csList.push_back(strUserDefine1);
	m_csList.push_back(strUserDefine2);
	m_csList.push_back(strUserDefine3);

}

void CMobileElemTypeStrDB::read2_3Data( ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}

	// get the tailing three items: USER DEFINE1, USER DEFINE2, USER DEFINE3
	CString strUserDefine3 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine2 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine1 = m_csList.back();
	m_csList.pop_back();


	std::vector<CString>::iterator iter = std::find(m_csList.begin(),m_csList.end(),CString(_T("LAGS BAG INSIDE ROLLABOARD")));
	if (iter != m_csList.end())
	{
		*iter = CString(_T("ROLLABOARD EXTENDED"));
	}

	m_csList.push_back("TRAY");
	m_csList.push_back("BACKPACK");
	m_csList.push_back("LAG BAG");
	m_csList.push_back("PURSE");
	m_csList.push_back("DUTYFREE");
	// at last, push the three user define items to the back of the vector
	m_csList.push_back(strUserDefine1);
	m_csList.push_back(strUserDefine2);
	m_csList.push_back(strUserDefine3);
}

void CMobileElemTypeStrDB::read2_2Data(ArctermFile& p_file )
{
	p_file.getLine();
	while( p_file.getLine() )
	{
		char szBuf[256];
		p_file.getField( szBuf, 256 );
		m_csList.push_back( CString( szBuf ) );
	}

	// get the tailing three items: USER DEFINE1, USER DEFINE2, USER DEFINE3
	CString strUserDefine3 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine2 = m_csList.back();
	m_csList.pop_back();
	CString strUserDefine1 = m_csList.back();
	m_csList.pop_back();

	// add new items according to file version
	float fVersionInFile = p_file.getVersion();
	if (fVersionInFile < 2.3)
	{
		ASSERT(m_csList.size() == (7 + 1));
		m_csList.push_back("LAPTOP CASE");
		m_csList.push_back("LAPTOP");
		m_csList.push_back("ROLLABOARD EXTENDED");
		m_csList.push_back("ROLLABOARD");
		m_csList.push_back("COAT");
		m_csList.push_back("SHOES");
		m_csList.push_back("MISCELLENEOUS(KEY | CELLPHONE | BELT)");
		m_csList.push_back("TRAY");
		m_csList.push_back("BACKPACK");
		m_csList.push_back("LAG BAG");
		m_csList.push_back("PURSE");
		m_csList.push_back("DUTYFREE");
		// etc
	}
	// if there is a new version, keep update code of this kind
	// 
	// if (fVersionInFile < yourNewVersion)
	// {
	//     ASSERT(m_csList.size() == (theOldNOPAX_COUNT + 1));
	//     m_csList.push_back("...");
	//     // etc
	// }

	// at last, push the three user define items to the back of the vector
	m_csList.push_back(strUserDefine1);
	m_csList.push_back(strUserDefine2);
	m_csList.push_back(strUserDefine3);

}
void CMobileElemTypeStrDB::readObsoleteData ( ArctermFile& p_file )
{
	if(p_file.getVersion() > 2.5f)
	{
		read2_6Data(p_file);
	}
	else if(p_file.getVersion() > 2.4f)
	{
		read2_5Data(p_file);
	}
	else if(p_file.getVersion() > 2.3f)
	{
		read2_4Data(p_file);
	}
	else if (p_file.getVersion() > 2.2f)
	{
		read2_3Data(p_file);
	}
	else
	{
		read2_2Data(p_file);
	}


	ASSERT(m_csList.size() == ALLPAX_COUNT);
	if( m_csList.size() != ALLPAX_COUNT )
	{
		initDefaultValues();
	}

}


void CMobileElemTypeStrDB::writeData( ArctermFile& p_file ) const
{
	int nSize = m_csList.size();
	for( int i=0; i<nSize; i++ )
	{
		p_file.writeField( m_csList[i] );
		p_file.writeLine();
	}
}

void CMobileElemTypeStrDB::initDefaultValues()
{	
	clear ();
	m_csList.push_back("VISITOR");
	m_csList.push_back("CHECKED BAGGAGE");
	m_csList.push_back("HAND BAG");
	m_csList.push_back("CARTE");
	m_csList.push_back("WHEELCHAIR");
	m_csList.push_back("ANIMAL");
	m_csList.push_back("CAR");
	m_csList.push_back("LAPTOP CASE");
	m_csList.push_back("LAPTOP");
	m_csList.push_back("ROLLABOARD EXTENDED");
	m_csList.push_back("ROLLABOARD");
	m_csList.push_back("COAT");
	m_csList.push_back("SHOES");
	m_csList.push_back("MISCELLENEOUS(KEY | CELLPHONE | BELT)");
	m_csList.push_back("TRAY");
	m_csList.push_back("BACKPACK");
	m_csList.push_back("LAG BAG");
	m_csList.push_back("PURSE");
	m_csList.push_back("DUTYFREE");


	// the three user define type should be at the tail
	m_csList.push_back ("USER DEFINE1");
	m_csList.push_back ("USER DEFINE2");
	m_csList.push_back ("USER DEFINE3");
}


const char* CMobileElemTypeStrDB::getTitle () const
{
	return "Mobile element Type String Database";
}


const char* CMobileElemTypeStrDB::getHeaders () const
{
	return "String";
}
