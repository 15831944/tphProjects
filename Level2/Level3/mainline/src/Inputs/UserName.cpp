// UserName.cpp: implementation of the CUserName class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "UserName.h"
#include "common\termfile.h"
#include <CommonData/procid.h>
#include "inputs\in_term.h"
#include "engine\proclist.h"

#define SZBUFFERLEN 256
const CString strErrorMsg1 = "Get User Name Failure";
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUserName::CUserName()
{
	m_strName	= "";
	m_iSeqStart = -1;
	m_iSeqEnd	= -1;
	m_iNum		= 0;		//*
	m_strFileName = "";
}

CUserName::~CUserName()
{

}


void CUserName::writeDataToFile( ArctermFile& _file ) const
{
	_file.writeField( m_strName );
	_file.writeInt( m_iSeqStart );
	_file.writeInt( m_iSeqEnd );
	_file.writeInt( m_iNum );
	_file.writeField( m_strFileName );
	_file.writeLine();
	
	int size = m_vAllSeqNum.size();
	_file.writeInt( size );
	for( int i=0; i<size; i++ )
		_file.writeInt( m_vAllSeqNum[i] );
	
	_file.writeLine();
}

void CUserName::readDataFromFile( ArctermFile& _file )
{
	int size,number;
	char szBuf[SZBUFFERLEN];
	
	m_vAllSeqNum.clear();

	_file.getLine();
	_file.getField( szBuf, SZBUFFERLEN );
	m_strName = CString(szBuf);
	_file.getInteger( m_iSeqStart );
	_file.getInteger( m_iSeqEnd );
	_file.getInteger( m_iNum );
	_file.getField( szBuf, SZBUFFERLEN );
	m_strFileName = CString( szBuf );
	
	_file.getLine();
	_file.getInteger( size );
	for( int i=0; i<size; i++ )
	{
		_file.getInteger( number );
		m_vAllSeqNum.push_back( number );
	}
}

bool CUserName::operator==(const CUserName& _name)
{
	if( m_strName == _name.m_strName)
		return true;
	return false;
}

bool CUserName::operator==(const CString& _name)
{
	if( m_strName == _name )
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
CUserNameDataSet::CUserNameDataSet(  ):DataSet(UserNameListFile)
{
}

CUserNameDataSet::~CUserNameDataSet()
{

}

const std::map< CString, CUserName>& CUserNameDataSet::getModelNameToUserNameMap()
{
	return m_mapModelNameToUserName;
}

bool CUserNameDataSet::ifExistInUserList( const ProcessorID& _procID )
{
	CString strProcName = _procID.GetIDString();
	return ifExistInUserList( strProcName );
}

bool CUserNameDataSet::ifExistInUserList( const CString& _strProcID )
{
	std::map< CString, CUserName>::iterator iter;
	//iter = std::find( m_mapModelNameToUserName.begin(), m_mapModelNameToUserName.end(),_strProcID );
	iter = m_mapModelNameToUserName.find( _strProcID );
	
	if( iter != m_mapModelNameToUserName.end() )	// finded ,the procID still exist in the map
		return true;
	return false;
}

bool CUserNameDataSet::printUserName(const CString& _strProcID, std::vector<CString>& _vectString )
{
	CString strBuf;
	if( !ifExistInUserList( _strProcID ) )
		return false;

	// get model name
	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict );
	id.setID( _strProcID );

	int iSeqStart = -1;
	int iSeqEnd = -1;
	int iNum = 0;
	GroupIndex group = m_pInTerm->procList->getGroupIndex( id );
	for( int i=group.start; i<=group.end; i++ )
	{
		CString strNum = m_pInTerm->procList->getProcessor(i)->getID()->GetLeafName();
		// ensure strNum id is Numeric
		for( int i=0; i<strNum.GetLength(); i++ )
			assert( isNumeric( strNum[i] ) );

		int num = atoi( strNum );
		iSeqStart = iSeqStart == -1 ? num : min(iSeqStart,num); //max(min(iSeqStart,num),0);
		iSeqEnd   = max(iSeqEnd,num);
		iNum++;
	}
	
	_vectString.push_back( _strProcID );
	strBuf.Format("%d",iSeqStart);
	_vectString.push_back( strBuf );
	strBuf.Format("%d",iSeqEnd );
	_vectString.push_back( strBuf );
	strBuf.Format("%d", iNum );
	_vectString.push_back(strBuf);
	
	//////////////////////////////////////////////////////////////////////////
	// get user name
	CUserName& name = m_mapModelNameToUserName[_strProcID];
	if( name.getNum() == 0 )	// the name is default value,
	{
		for(int i=0; i<5;i++)
			_vectString.push_back(" ");
	}
	else
	{
		_vectString.push_back( name.getName() );
		strBuf.Format("%d",name.getSeqStart() );
		_vectString.push_back( strBuf );
		strBuf.Format("%d",name.getSeqEnd() );
		_vectString.push_back( strBuf );
		strBuf.Format("%d",name.getNum() );
		_vectString.push_back( strBuf );
		_vectString.push_back( name.getFilaName() );
	}

	return true;
}

bool CUserNameDataSet::addUserName(const ProcessorID& _procID )
{
	CString strMsg;
	CString strProcName = _procID.GetIDString();
	if( !isProcIDValid( _procID) )
	{
		strMsg.Format(" The processor: %s is invalid, so can not drag the processor to mapping list",strProcName );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	if( ifExistInUserList(strProcName) )
	{
		strMsg.Format(" The processor: %s still exist",strProcName );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	CUserName name;
	m_mapModelNameToUserName[ strProcName ] = name;
	return true;
}

bool CUserNameDataSet::isProcIDValid(const ProcessorID& _procID )
{
	GroupIndex group = m_pInTerm->procList->getGroupIndex( _procID );
	if( group.start ==-1 && group.end == -1 )
		return false;

	int iParentLen = _procID.idLength();
	int iChildLen  = m_pInTerm->procList->getProcessor(group.start)->getID()->idLength();
	if( iParentLen != iChildLen-1 )
		return false;
	
	for( int i =group.start; i<=group.end; i++ )
	{
		CString strNum = m_pInTerm->procList->getProcessor(i)->getID()->GetLeafName();
		for( int i=0; i<strNum.GetLength(); i++ )
			if( !isNumeric( strNum[i] ) )
				return false;
	}

	return true;
}

bool CUserNameDataSet::modifyUseName(const ProcessorID& _procID, const CUserName& _userName)
{
	CString strMsg;
	CString strProcName = _procID.GetIDString();
	if( !ifExistInUserList(strProcName) )
	{
		strMsg.Format(" The processor: %s still exist",strProcName );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	if( userNameIfExist(_userName.getName()) )
	{
		strMsg.Format(" The user name: %s has been used",_userName.getName() );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}

	if( !canModifyUseName( _procID, _userName) )
	{
		//strMsg.Format("The number of processor: %s and the number of username: %s don't match", strProcName, _userName.getName() );
		//AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}
	
	m_mapModelNameToUserName[strProcName] = _userName;
	return true;
}

bool CUserNameDataSet::canModifyUseName(const ProcessorID& _procID, const CUserName& _userName)
{
	CString strMsg;
	CString strProcName = _procID.GetIDString();

	GroupIndex group = m_pInTerm->procList->getGroupIndex( _procID );
	if( group.end == -1 && group.start == -1 )
	{
		strMsg.Format(" The processor: %s is invalid", strProcName );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}

	int iProcessorNum = group.end-group.start+1;
	const std::vector<int>& vNumList =  _userName.getSeqNumList();
	// first just if the number is fits
	if( (UINT)iProcessorNum < vNumList.size() )
	{
		strMsg.Format(" The number of processor: %s less than the number of username: %s ", strProcName, _userName.getName() );
		AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
		return false;
	}

	// then just if each num of vNumList exits in processor group
	std::vector<int> vLeafNum;
	for( int i = group.start; i<=group.end; i++ )
	{
		CString strNum = m_pInTerm->procList->getProcessor(i)->getID()->GetLeafName();
		//for( int i=0; i<strNum.GetLength(); i++ )
		//	if( !isNumeric( strNum[i] ) )
		//		return false;
		int iProcNum = atoi( strNum );
		vLeafNum.push_back( iProcNum );
	}

	for( int ii= 0; ii<(int)vNumList.size(); ii++ )
	{
		std::vector<int>::const_iterator iter= std::find( vLeafNum.begin(), vLeafNum.end(), vNumList[ii] );
		if( iter == vLeafNum.end() )	// not exist
		{
			strMsg.Format(" The username num:%d don't exist in processor list",vNumList[ii] );
			AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
			return false;
		}
	}
	/*
	// then just if each num of processor exits in vNumList
	for( int i = group.start; i<=group.end; i++ )
	{
		CString strNum = m_pInTerm->procList->getProcessor(i)->getID()->GetLeafName();
		//for( int i=0; i<strNum.GetLength(); i++ )
		//	if( !isNumeric( strNum[i] ) )
		//		return false;
		int iProcNum = atoi( strNum );
		std::vector<int>::const_iterator iter = std::find(vNumList.begin(), vNumList.end(), iProcNum );
		if( iter == vNumList.end() )	// not exist
		{
			strMsg.Format(" The processor num:%d don't exist in user name list",iProcNum );
			AfxMessageBox( strMsg,  MB_OK|MB_ICONINFORMATION );
			return false;
		}
	}
	*/
	return true;
}


bool CUserNameDataSet::deleteUserName( const ProcessorID& _procID )
{
	CString strProcName = _procID.GetIDString();

	std::map< CString, CUserName>::iterator iter;
	//iter = std::find( m_mapModelNameToUserName.begin(), m_mapModelNameToUserName.end(),strProcName);
	iter = m_mapModelNameToUserName.find(strProcName);
	if( iter != m_mapModelNameToUserName.end() )
	{
		m_mapModelNameToUserName.erase( iter );
		return true;
	}
	else
		return false;
}

bool CUserNameDataSet::canSave()
{
	std::map< CString, CUserName>::iterator iter;
	for( iter = m_mapModelNameToUserName.begin(); iter!= m_mapModelNameToUserName.end(); ++iter )
	{
		if( iter->second.getNum() == 0)
			return false;
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
// data set virtual function
// clear data structure before load data from the file.
void CUserNameDataSet::clear()
{
	m_mapModelNameToUserName.clear();
}

void CUserNameDataSet::writeData( ArctermFile& p_file ) const
{
	int size = m_mapModelNameToUserName.size();
	p_file.writeInt( size );
	p_file.writeLine();

	std::map< CString, CUserName >::const_iterator iter;
	for( iter = m_mapModelNameToUserName.begin(); iter != m_mapModelNameToUserName.end(); ++iter )
	{
		p_file.writeField( iter->first );
		p_file.writeLine();
		iter->second.writeDataToFile( p_file );
	}
}


void CUserNameDataSet::readData( ArctermFile& p_file )
{
	int size;
	char szBuf[SZBUFFERLEN];
	
	m_mapModelNameToUserName.clear();

	p_file.getLine();
	p_file.getInteger( size );
	
	for( int i=0; i<size; i++ )
	{
		p_file.getLine();
		p_file.getField( szBuf, SZBUFFERLEN );

		CUserName userName;
		userName.readDataFromFile( p_file );
		m_mapModelNameToUserName[CString(szBuf)] = userName;
	}
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CUserNameDataSet::userNameIfExist( const CString& _strUserName ) const
{
	CString strUserName = _strUserName;
	strUserName.MakeUpper();
	
	std::map<CString, CUserName>::const_iterator iter;
	for( iter = m_mapModelNameToUserName.begin(); iter!= m_mapModelNameToUserName.end(); ++iter )
	{
		if( iter->second.getName() == strUserName )
			return true;
	}

	return false;
	/*
	std::map<CString, CUserName>::const_iterator iter = std::find( m_mapModelNameToUserName.begin(), m_mapModelNameToUserName.end(), _strUserName );
	if( iter!= m_mapModelNameToUserName.end() )
		return true;
	return false;
	*/
}


bool CUserNameDataSet::findProcNameByUserName( CString& _strUserName ,CString& _strProcName ) const
{
	// if _strUserName is a processor name, return false
	_strUserName.MakeUpper();
	//if( m_pInTerm->procList->getProcessor( _strUserName )!= NULL )
	//	return false;
	
	int pos, prepos;
	pos = prepos = _strUserName.Find('-');
	if( pos == -1 )		// not '-'
		return false;

	while( pos!= -1 )
	{
		prepos = pos;
		pos = _strUserName.Find('-',prepos+1);
	}
	CString strName = _strUserName.Left( prepos );
	CString strNum = _strUserName.Mid( prepos+1 );

	// assure strNum must is num
	for( int i=0; i<strNum.GetLength(); i++ )
		if( !isNumeric( strNum[i]))
			return false;
	int iSeqNum = atoi( strNum );


	std::map<CString, CUserName>::const_iterator iter;
	for( iter = m_mapModelNameToUserName.begin(); iter!= m_mapModelNameToUserName.end(); ++iter )
	{
		if( iter->second.getName() == strName )
		{
			ProcessorID procID;
			procID.SetStrDict( m_pInTerm->inStrDict );
			procID.setID( iter->first );
			
			// just the iSeqNum if exist in the procID' leaf Name
			GroupIndex group = m_pInTerm->procList->getGroupIndex( procID );
			for( int j=group.start; j<=group.end; j++ )
			{
				CString strLeaf = m_pInTerm->procList->getProcessor(j)->getID()->GetLeafName();
				if( atoi(strLeaf) == iSeqNum )
				{
					_strProcName = m_pInTerm->procList->getProcessor(j)->getID()->GetIDString();
					return true;
				}
			}
			break;
		}
	}
	
	return false;
}


bool CUserNameDataSet::getUserNameByProcessorName( const CString& _strProcName, CString& _strUserName ) 
{
	int pos, prepos;
	pos = prepos = _strUserName.Find('-');
	if( pos == -1 )		// no '-'
	{
		_strUserName = strErrorMsg1;
		return false;
	}
	
	while( pos!= -1 )
	{
		prepos = pos;
		pos = _strUserName.Find('-',prepos+1);
	}
	CString strName = _strUserName.Left( prepos );
	CString strNum = _strUserName.Mid( prepos+1 );
	
	if( !ifExistInUserList( strName) )
	{
		_strUserName = strErrorMsg1;
		return false;
	}
	_strUserName = m_mapModelNameToUserName[strName].getName()+"-"+strNum;
	return true;
}