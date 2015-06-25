// SimAndReportManager.cpp: implementation of the CSimAndReportManager class.
//
//////////////////////////////////////////////////////////////////////
#include"stdafx.h"
#include "SimAndReportManager.h"
#include "common\mutilreport.h"
#include "common\exeption.h"
#include "common\fileman.h"
#include "inputs\SimParameter.h"
#include "projectmanager.h"
#include "UndoManager.h"
#include "main\resource.h"
#include "ARCTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSimAndReportManager::CSimAndReportManager():DataSet( SimAndReportFile, 2.4f )
{
	m_bModifiedInput = false;
	m_nAAStartTime = 0;
	m_nAAEndTime = 0;
	m_bAirsideSim = false;
}

CSimAndReportManager::~CSimAndReportManager()
{

}
// get all simResultName 
void CSimAndReportManager::GetAllSimResultName( std::vector<CString>& _vSimResultName )const
{
	_vSimResultName.push_back( strSimResult );
}

// get all subSimName down _stSimResultName;
void CSimAndReportManager::GetSubSimResultName( std::vector<CString>& _vSubSimResultName )const
{	
	for( unsigned i=0; i<m_vSimResult.size(); i++ )
	{
		if( i== 0 )
		{
			for( int j=0; j<m_vSimResult.at(i).getCount(); j++ )
			{
				_vSubSimResultName.push_back( m_vSimResult.at(i).getAt(j).getSubSimName() );
			}
			return;
		}
	}
}

void CSimAndReportManager::GetReportsOfSubSimResult(  const CString& _stSubSimResultName, std::vector<CString>& _vSubSimResultReports )const
{

	for( unsigned i=0; i<m_vSimResult.size() ; i++ )
	{
		if( i==0 )
		{
			for( int j=0; j<m_vSimResult.at(i).getCount(); j++ )
			{
				if( m_vSimResult.at(i).getAt(j).getSubSimName() == _stSubSimResultName )
				{
					const CReportSet& reportSet = m_vSimResult.at(i).getAt(j).getReportSet();
					reportSet.getAllReortName( _vSubSimResultReports );
					return;
				}
			}
		}
	}
}

void CSimAndReportManager::SetCurrentSimResult( int _subSimIndex )
{
	assert( _subSimIndex>=0 && _subSimIndex< this->getSubSimResultCout() );

	CString strNum;
	strNum.Format("%d",_subSimIndex );
	SetCurrentSimResult( strSimResult + strNum);

}

void CSimAndReportManager::SetCurrentSimResult(  const CString& _sSubSimName )
{
	ClearAll();

	m_sSimName		= strSimResult; 
	m_sSubSimName	= _sSubSimName;
	m_sSubSimName.MakeUpper();

	for( unsigned i=0; i<m_vSimResult.size(); i++ )
	{
		if( i==0 )
		{
			for( int j=0 ;j<m_vSimResult.at(i).getCount(); j++ )
			{
				CString strTemp = m_vSimResult.at(i).getAt(j).getSubSimName();
				strTemp.MakeUpper();
				if(  strTemp == m_sSubSimName )
				{
					m_mapLogTypeToFileName	  = m_vSimResult[i].getAt(j).getLogsMap();
					m_mapReportTypeToFileName = m_vSimResult[i].getAt(j).getReportSet().getReportMap();
					return;
				}

			}
		}
	}
	
}

CSimItem* CSimAndReportManager::getSimItem( int _subSimIndex )
{
	assert( _subSimIndex>=0 && _subSimIndex< this->getSubSimResultCout() );
	
	CString strNum;
	strNum.Format("%d",_subSimIndex );
	return getSimItem( strSimResult + strNum);
}

CSimItem* CSimAndReportManager::getSimItem(  const CString& _sSubSimName )
{
	CString strSubSimName = _sSubSimName;
	strSubSimName.MakeUpper();

	for( unsigned i=0; i<m_vSimResult.size(); i++ )
	{
		if( i==0 )
		{
			for( int j=0 ;j<m_vSimResult.at(i).getCount(); j++ )
			{
				CString strTemp = m_vSimResult.at(i).getAt(j).getSubSimName();
				strTemp.MakeUpper();
				if(  strTemp == strSubSimName )
				{
					return m_vSimResult.at(i).At(j);;
				}
				
			}
		}
	}
	return NULL;
}

CReportItem* CSimAndReportManager::getCurReportItem( void )
{
	m_sSubSimName.MakeUpper();
	
	for( unsigned i=0; i<m_vSimResult.size(); i++ )
	{
		if( i==0 )
		{
			for( int j=0 ;j<m_vSimResult.at(i).getCount(); j++ )
			{
				CString strTemp = m_vSimResult.at(i).getAt(j).getSubSimName();
				strTemp.MakeUpper();
				if(  strTemp == m_sSubSimName )
				{
					CSimItem* pItem = m_vSimResult.at(i).At(j);;
					return ((*(pItem->GetReportSet()))[(ENUM_REPORT_TYPE)m_enReportType]);
				}
				
			}
		}
	}	
	return NULL;
}

void CSimAndReportManager::SetCurrentReportType(ENUM_REPORT_TYPE _enReportType)
{
	 m_enReportType = _enReportType;
	 m_sCurrentReportFileName = m_mapReportTypeToFileName[ _enReportType ];
}

CString CSimAndReportManager::GetCurrentLogsFileName(InputFiles _enLogType, const CString& _strPath )
{
	return _strPath + "\\" + m_mapLogTypeToFileName[ _enLogType ];
}
CString CSimAndReportManager::GetCurrentReportFileName( const CString& _strPath )
{
	return _strPath + "\\" +m_sCurrentReportFileName;
}


void CSimAndReportManager::ClearAll()
{
	m_mapReportTypeToFileName.clear();
	m_mapLogTypeToFileName.clear();
}

bool CSimAndReportManager::addNewSimResult( const CSimParameter* _pSimPara, const CString& _strPath, InputTerminal* _pInTerm )
{
	PLACE_METHOD_TRACK_STRING();
	int size = m_vSimResult.size();
	if( size>=1 )		// still have a SimResult, must delete it
	{
		//assert( size == 1 );
		// remove the still exist path
		CString strDelPath = _strPath + "\\" + strSimResult;
//		FileManager::DeleteDirectory( strDelPath );
		// remove item from vector
		//m_vSimResult.erase( m_vSimResult.begin() );
		m_vSimResult.clear();
	}

	CSimResult newResult;
	newResult.initSimResult( _pSimPara,_pInTerm, _strPath );
	m_vSimResult.push_back( newResult );
	
	ClearAll();
	return true;
}

bool CSimAndReportManager::removeSimResult( void )
{

	for( unsigned i=0; i<m_vSimResult.size(); i++ )
	{
		m_vSimResult.erase( m_vSimResult.begin() + i );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// read and write
void CSimAndReportManager::clear (void)
{
	ClearAll();
}

void CSimAndReportManager::readData (ArctermFile& p_file)
{
	p_file.getLine();

	char szDate[100];
//	char *pszDate = NULL;
	int	nNum[3];
	int i = 0;
/*
	int lenth = p_file.getLine();
	pszDate = new char[lenth+1];
	assert(pszDate);
	p_file.copyLine(pszDate);

	char seps[] = ",";
	char *token = NULL;
	CString strDate;
	int nPrevPos = 0;
	int nPos = 0;

	m_startDate.SetDate(0, 0, 0);
	token = strtok(pszDate, seps);
	for (int t = 0; token != NULL && t < 3; t++)
	{
		strDate = token;
		if( 0 == t && strstr(token,"-") != NULL )
		{
			i = 0;
			nPrevPos = 0;
			nPos = 0;
			while (((nPos = strDate.Find('-', nPos + 1)) != -1) && (i < 3))
			{
				//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
				nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
				nPrevPos = nPos + 1;
			}				
			nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));
			m_startDate.SetDate(nNum[0], nNum[1], nNum[2]);
		}
		else
		{
			i = 0;
			nPrevPos = 0;
			nPos = 0;
			while (((nPos = strDate.Find('-', nPos + 1)) != -1) && (i < 3))
			{
				//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
				nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
				nPrevPos = nPos + 1;
			}				
			nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

			if( 1 == t)
			{
				m_UserStartTime.set(nNum[0], nNum[1], nNum[2]);
			}
			else
			{
				m_UserEndTime.set(nNum[0], nNum[1], nNum[2]);
			}
			
		}
		
		token = strtok(NULL, seps);
	}
	
	delete[] pszDate;
	pszDate = NULL;
*/

	p_file.getField(szDate, 16);
	CString strDate = szDate;
	if (!strDate.IsEmpty())
	{
		int nPrevPos = 0;
		int nPos = 0;
		while (((nPos = strDate.Find('-', nPos + 1)) != -1) && (i < 3))
		{
			//CString strTemp = strDate.Mid(nPrevPos, nPos - nPrevPos); 
			nNum[i++] = atoi(strDate.Mid(nPrevPos, nPos - nPrevPos));
			nPrevPos = nPos + 1;
		}
		
		//CString strTemp = strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos); 
		nNum[i] = atoi(strDate.Mid(nPrevPos, strDate.GetLength() - nPrevPos));

		m_startDate.SetAbsoluteDate( true );
		m_startDate.SetDate(nNum[0], nNum[1], nNum[2]);
	}
	else
		m_startDate.SetAbsoluteDate( false );

	p_file.getLine();
	p_file.getTime( m_UserStartTime, true );
	p_file.getTime( m_UserEndTime, true );

	p_file.getInteger( m_nAAStartTime);
	p_file.getInteger( m_nAAEndTime);
	
	p_file.getLine();
	char ch;
	p_file.getChar( ch );
	if( ch == 'Y' )
		m_bModifiedInput = true;
	else
		m_bModifiedInput = false;
	readSimResult( p_file );

	//read airside sim run mark
	p_file.getLine();
	int nMark = 0;
	p_file.getInteger(nMark);
	m_bAirsideSim = nMark >0 ?true:false;
}

void CSimAndReportManager::readObsoleteData ( ArctermFile& p_file )
{
	readSimResult( p_file );
}

// read the sim result part.
void CSimAndReportManager::readSimResult ( ArctermFile& p_file )
{
	//read the num of simresult
	int iResultNum;
	p_file.getLine();
	p_file.getInteger( iResultNum );
	for( int i=0; i<iResultNum; i++)
	{
		CSimResult simResult;
		simResult.readFromFile( p_file );
		m_vSimResult.push_back( simResult );
	}	
}


void CSimAndReportManager::writeData (ArctermFile& p_file) const
{
	CString strDate = m_startDate.WriteStartDate();
	p_file.writeField( strDate );
	p_file.writeLine();

	p_file.writeTime(m_UserStartTime, TRUE);	//by aivin
	p_file.writeTime(m_UserEndTime, TRUE);
	
	p_file.writeInt( m_nAAStartTime);
	p_file.writeInt( m_nAAEndTime);

	p_file.writeLine();
	// write the num of simresult;

	p_file.writeChar( m_bModifiedInput ? 'Y' : 'N' );
	p_file.writeLine();
	p_file.writeInt( (int)m_vSimResult.size() );
	p_file.writeLine();
	// write each simresult
	for( unsigned i=0;i<m_vSimResult.size(); i++ )
	{
		m_vSimResult[i].writeToFile( p_file );
	}

	//write airside sim mark
	p_file.writeLine();
	p_file.writeInt( m_bAirsideSim?1:0 );
	p_file.writeLine();
}

int CSimAndReportManager::getSubSimResultCout( void )
{
	if( m_vSimResult.size() <= 0 )
		return 0;
	// because only one, so only need to get 0
	return m_vSimResult[0].getCount();
}

void CSimAndReportManager::SetAndSaveInputModified( const CString& _csProjPath )
{
	m_bModifiedInput = true;
	saveDataSet( _csProjPath, false );
}

const CStartDate& CSimAndReportManager::GetStartDate() const
{
	return m_startDate;
}

void CSimAndReportManager::SetStartDate(const CStartDate &_date)
{
	m_startDate = _date;
}

void CSimAndReportManager::SetUserStartTime(const ElapsedTime& _date)
{
	m_UserStartTime = _date;
}

const ElapsedTime& CSimAndReportManager::GetUserStartTime() const
{
	return m_UserStartTime;
}

void CSimAndReportManager::SetUserEndTime(const ElapsedTime& _date)
{
	m_UserEndTime = _date;
}

const ElapsedTime& CSimAndReportManager::GetUserEndTime() const
{
	return m_UserEndTime;
}

void CSimAndReportManager::loadDataSet2 (const CString& _pProjPath,int mode)
{
	clear();

	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename,mode);

	ArctermFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet2(_pProjPath, false,2);
		return;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
		saveDataSet2(_pProjPath, false,2);
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );		
	}
	else
	{
		readData (file);
		file.closeIn();
	}
}

void CSimAndReportManager::saveDataSet2 (const CString& _pProjPath, bool _bUndo,int mode ) const
{
	//if( _bUndo )
	//{
	//	CUndoManager* undoMan = CUndoManager::GetInStance( _pProjPath );
	//	if( !undoMan->AddNewUndoProject() )
	//	{
	//		AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );
	//	}
	//}
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE && _bUndo)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename,mode);

	ArctermFile file;
	file.openFile (filename, WRITE, m_fVersion);

	file.writeField (getTitle());
	file.writeLine();

	file.writeField (getHeaders());
	file.writeLine();

	writeData (file);

	file.endFile();
}

