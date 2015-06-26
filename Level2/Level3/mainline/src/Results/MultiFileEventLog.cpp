#include "StdAfx.h"
#include ".\multifileeventlog.h"

#define	EVENT_FILE_SIZE	(1.5*1024*1024*1024)

MultiFileEventLog::MultiFileEventLog(void)
:m_pMultiEventLog(NULL)
{
}

MultiFileEventLog::~MultiFileEventLog(void)
{
	closeEventFile();
}

void MultiFileEventLog::deleteEventFile(const char *p_filename)
{
	FileManager fileMan;
	CString sFileName = p_filename;
	CString sNextFileName = p_filename;

	long lFileID = 0;
	while(fileMan.IsFile (sNextFileName))
	{
		fileMan.DeleteFile (sNextFileName);

		lFileID++;
		CString strLeft;
		long lPos = sFileName.Find('.');
		strLeft = sFileName.Left(lPos);
		sNextFileName.Format(_T("%s%03d.log"),strLeft,lFileID);
	}
}

void MultiFileEventLog::createEventFile(const char *p_filename)
{
	deleteEventFile(p_filename);

	if (m_pMultiEventLog)
	{
		delete m_pMultiEventLog;
		m_pMultiEventLog = NULL;
	}
	
	m_pMultiEventLog = new ListMultiEventFile(p_filename,0);
	EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
	fsstream* pFsstream = new fsstream (p_filename, stdios::out|stdios::binary );
	pEventLog->SetEventFile(pFsstream);
	m_pMultiEventLog->SetEventLog(pEventLog);

	if (pFsstream->bad() || !*pFsstream)
		throw new FileOpenError (p_filename);

	closeEventFile();
	openEventFile (p_filename);
}

void MultiFileEventLog::openEventFile( const char *p_filename )
{
	if (m_pMultiEventLog == NULL)
	{
		m_pMultiEventLog = new ListMultiEventFile(p_filename,0);

		EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
		pEventLog->openEventFile(m_pMultiEventLog->GetEventFileName());

		m_pMultiEventLog->SetEventLog(pEventLog); 

		m_pMultiEventLog->openEventFile(p_filename);
	}
}

void MultiFileEventLog::closeEventFile( void )
{
	ListMultiEventFile* pEventFileLog = m_pMultiEventLog;
	while (m_pMultiEventLog)
	{
		m_pMultiEventLog = pEventFileLog->GetNext();
		delete pEventFileLog;
		pEventFileLog = m_pMultiEventLog;
	}

	m_pMultiEventLog = NULL;
}


long MultiFileEventLog::GetLastFileID() const
{
	if (m_pMultiEventLog == NULL)
		return 0l;
	
	return m_pMultiEventLog->GetLastFileID();
}

EventLog<MobEventStruct>* MultiFileEventLog::GetEventLog( long lFile /*= 0*/ )
{
	ListMultiEventFile* pEventFileLog = m_pMultiEventLog;
	while (pEventFileLog)
	{
		if (pEventFileLog->GetFileID() == lFile)
		{
			return pEventFileLog->GetEventLog();
		}

		pEventFileLog = pEventFileLog->GetNext();
	}

	return NULL;
}

long MultiFileEventLog::getPositionForWrite( void )const
{
	if(m_pMultiEventLog == NULL)
		return 0l;
	
	return m_pMultiEventLog->getPositionForWrite();
}

bool MultiFileEventLog::needCreateFile( long lCount ) const
{
	if (m_pMultiEventLog == NULL)
		return false;
	;
	return m_pMultiEventLog->needCreateFile(lCount);
}

void MultiFileEventLog::setEventList( const std::vector<MobEventStruct*>& p_item )
{
	if (m_pMultiEventLog == NULL)
		return;

	m_pMultiEventLog->setEventList(p_item);
}


//////////////////////////////////////////////////////////////////////////
//ListMultiEventFile

ListMultiEventFile::ListMultiEventFile( const CString& sDefaultName,long lFileID )
:m_sFileEventName(sDefaultName)
,m_lFileID(lFileID)
,m_pNextEventLog(NULL)
{

}

ListMultiEventFile::~ListMultiEventFile()
{
	closeEventFile();
}

void ListMultiEventFile::closeEventFile( void )
{
	if(m_pEventlog)
	{
		delete m_pEventlog;
		m_pEventlog = NULL;
	}
	
}

ListMultiEventFile* ListMultiEventFile::GetNext() const
{
	return m_pNextEventLog;
}

CString ListMultiEventFile::GetEventFileName() const
{
	if (m_lFileID == 0)
	{
		return m_sFileEventName;
	}

	CString strLeft;
	long lPos = m_sFileEventName.Find('.');
	strLeft = m_sFileEventName.Left(lPos);
	CString sFileName;
	sFileName.Format(_T("%s%03d.log"),strLeft,m_lFileID);
	return sFileName;
}

CString ListMultiEventFile::GetNextFileName()const
{
	CString strLeft;
	long lPos = m_sFileEventName.Find('.');
	strLeft = m_sFileEventName.Left(lPos);
	CString sFileName;
	sFileName.Format(_T("%s%03d.log"),strLeft,m_lFileID+1);
	return sFileName;
}

bool ListMultiEventFile::hasNext() const
{
	return m_pNextEventLog?true:false;
}

void ListMultiEventFile::openEventFile( const char *p_filename )
{
	CString sFileName;
	sFileName = GetNextFileName();
	FileManager fileMan;
	if(fileMan.IsFile ( sFileName ))
	{
		m_pNextEventLog = new ListMultiEventFile(p_filename,m_lFileID+1);
		EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
		pEventLog->openEventFile(sFileName);
		m_pNextEventLog->SetEventLog(pEventLog); 
		m_pNextEventLog->openEventFile(p_filename);
	}

}
void ListMultiEventFile::SetEventLog( EventLog<MobEventStruct>* pEventlog )
{
	//current event file doesn't have value
	m_pEventlog = pEventlog;
}

ListMultiEventFile* ListMultiEventFile::SetNext()
{
	if (m_pNextEventLog == NULL)
	{
		m_pNextEventLog = new ListMultiEventFile(m_sFileEventName,m_lFileID+1);
		EventLog<MobEventStruct>* pEventLog = new EventLog<MobEventStruct>;
		pEventLog->createEventFile(m_pNextEventLog->GetEventFileName());

		m_pNextEventLog->SetEventLog(pEventLog);

		closeEventFile();
		return m_pNextEventLog;
	}

	return m_pNextEventLog->SetNext();
}


void ListMultiEventFile::setEventList( const std::vector<MobEventStruct*>& p_item )
{
	long lPosition = getPositionForWrite();
	long lCount = (long)p_item.size(); 
	long lSize = lCount*sizeof(MobEventStruct);
	if (lPosition + lSize > EVENT_FILE_SIZE)
	{
		ListMultiEventFile* pNextEventFile = SetNext();
		if (pNextEventFile->GetEventLog())
		{
			pNextEventFile->GetEventLog()->setEventList(p_item);
		}
		return;
	}
	if (hasNext())
	{
		return m_pNextEventLog->setEventList(p_item);
	}

	m_pEventlog->setEventList(p_item);
}

long ListMultiEventFile::GetLastFileID() const
{
	ListMultiEventFile* pNextEventFile = m_pNextEventLog;
	while (pNextEventFile)
	{
		if (!pNextEventFile->hasNext())
		{
			return pNextEventFile->GetFileID();
		}
		pNextEventFile = pNextEventFile->GetNext();
	}

	return GetFileID();
}

long ListMultiEventFile::getPositionForWrite( void ) const
{
	ListMultiEventFile* pNextEventFile = m_pNextEventLog;
	while (pNextEventFile)
	{
		if (!pNextEventFile->hasNext())
		{
			if (pNextEventFile->GetEventLog())
			{
				return pNextEventFile->GetEventLog()->getPositionForWrite();
			}
			return 0l;
		}
		pNextEventFile = pNextEventFile->GetNext();
	}

	if (m_pEventlog)
	{
		return m_pEventlog->getPositionForWrite();
	}

	return 0l;
}

bool ListMultiEventFile::needCreateFile( long lCount )const
{
	if (hasNext())
	{
		return m_pNextEventLog->needCreateFile(lCount);
	}

	if (lCount <= EVENT_FILE_SIZE)
	{
		return false;
	}
	return true;
}
