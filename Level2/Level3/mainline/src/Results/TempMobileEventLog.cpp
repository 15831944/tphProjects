#include "StdAfx.h"
#include ".\tempmobileeventlog.h"


//////////////////////////////////////////////////////////////////////////
//TempMultiFileMobbileEventLog
TempMobileEventLog::TempMobileEventLog( void )
{
	//100,000
	m_nPaxPerFile = 4000;
}

TempMobileEventLog::~TempMobileEventLog( void )
{

}
void TempMobileEventLog::addEvent( MobEventStruct *pEvent )
{
	int nFileID = (pEvent->elementID)/m_nPaxPerFile;

	TempEventFile *pFile = GetFile(nFileID);
	if(pFile)
		pFile->addEvent(pEvent);

}

void TempMobileEventLog::FlushLog()
{
	int nCount =getFileCount();
	for (int nFile = 0; nFile < nCount; nFile ++)
	{
		TempEventFile *pFile = m_vFiles[nFile];
		if(pFile)
			pFile->FlushLog();
	}
}



void TempMobileEventLog::createEventFile( const char *p_filename )
{
	m_strDefaultName = CString(p_filename);
	deleteEventFile(p_filename);

}
void TempMobileEventLog::closeEventFile( void )
{
	//flush log to files
	FlushLog();
	int nCount =getFileCount();
	for (int nFile = 0; nFile < nCount; nFile ++)
	{
		TempEventFile *pFile = m_vFiles[nFile];
		if(pFile)
			pFile->closeEventFile();
		delete pFile;
	}
	m_vFiles.clear();
}

int TempMobileEventLog::getFileCount()
{
	return static_cast<int>(m_vFiles.size());
}

TempEventFile * TempMobileEventLog::GetFile( int nFileID )
{
	while( static_cast<int>(m_vFiles.size()) <= nFileID)
	{
		int nNewFileID = static_cast<int>(m_vFiles.size());
		TempEventFile *pNewFile = new TempEventFile(m_strDefaultName,nNewFileID);
		pNewFile->createEventFile(NULL);

		m_vFiles.push_back(pNewFile);
	}

	ASSERT(nFileID < (int)m_vFiles.size());
	return m_vFiles[nFileID];
}
void TempMobileEventLog::deleteEventFile(const char *p_filename)
{
	FileManager fileMan;
	CString sFileName = p_filename;
	CString sNextFileName = p_filename;

	long lFileID = 0;

	do
	{
		CString strLeft;
		long lPos = sFileName.Find('.');
		strLeft = sFileName.Left(lPos);
		sNextFileName.Format(_T("%s%03d.log"),strLeft,lFileID);

		if(fileMan.IsFile (sNextFileName))
			fileMan.DeleteFile (sNextFileName);
		else
			break;

		lFileID++;

	}while(true);

}

void TempMobileEventLog::openEventFile( const char *p_filename )
{
	FileManager fileMan;
	CString sFileName = p_filename;
	CString sNextFileName = p_filename;
	long lFileID = 0;

	do
	{
		CString strLeft;
		long lPos = sFileName.Find('.');
		strLeft = sFileName.Left(lPos);
		sNextFileName.Format(_T("%s%03d.log"),strLeft,lFileID);

		if(fileMan.IsFile (sNextFileName))
		{
			TempEventFile *pNewFile = new TempEventFile(sFileName,lFileID);
			pNewFile->openEventFile(NULL);

			m_vFiles.push_back(pNewFile);

		}
		else
			break;

		lFileID++;

	}while(true);
}

EventLog<MobEventStruct>* TempMobileEventLog::GetEventLog( int nFile )
{
	if(nFile >= 0&& nFile < getFileCount())
	{
		if( m_vFiles[nFile])
			return m_vFiles[nFile]->GetEventLog();
	}
	return NULL;
	
}




//////////////////////////////////////////////////////////////////////////
//file
TempEventFile::TempEventFile( const CString& sDefaultName,long lFileID )
{
	m_sFileEventName = sDefaultName;
	m_lFileID = lFileID;


	m_nReserveSize = 1000;
	m_vEvent.reserve(m_nReserveSize);



}

TempEventFile::~TempEventFile()
{

}

void TempEventFile::createEventFile( const char *p_filename )
{
	CString sFileName = GetFileName();
	fsstream* pFsstream = new fsstream (sFileName, stdios::out|stdios::binary );
	EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
	pEventLog->SetEventFile(pFsstream);
	SetEventLog(pEventLog);

	if (pFsstream->bad() || !*pFsstream)
		throw new FileOpenError (p_filename);

	closeEventFile();
	openEventFile (p_filename);

}

void TempEventFile::closeEventFile( void )
{
	if(m_pEventlog)
	{
		delete m_pEventlog;
		m_pEventlog = NULL;
	}
}
void TempEventFile::openEventFile( const char *p_filename )
{
	CString sFileName = GetFileName();

	FileManager fileMan;
	if(fileMan.IsFile ( sFileName ))
	{
		EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
		pEventLog->openEventFile(sFileName);
		SetEventLog(pEventLog); 
		//openEventFile(p_filename);
	}

}
CString TempEventFile::GetFileName()const
{
	CString strLeft;
	long lPos = m_sFileEventName.Find('.');
	strLeft = m_sFileEventName.Left(lPos);
	CString sFileName;
	sFileName.Format(_T("%s%03d.log"),strLeft,m_lFileID);
	return sFileName;
}
void TempEventFile::ClearEventLog()
{
	std::vector<MobEventStruct *>::iterator iter = m_vEvent.begin();
	for (;iter != m_vEvent.end(); ++ iter)
	{
		delete *iter;
	}
	m_vEvent.clear();
	m_vEvent.reserve(m_nReserveSize);

}
void TempEventFile::addEvent( MobEventStruct *pEvent )
{
	m_vEvent.push_back(pEvent);

	if(m_vEvent.size() == m_nReserveSize)
	{
		FlushLog();
	}
}

void TempEventFile::FlushLog()
{
	GetEventLog()->setEventList(m_vEvent);
	ClearEventLog();
}
