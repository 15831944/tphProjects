#include "stdafx.h"
#include "airportinfo.h"
#include "../common/exeption.h"
#include "../common/ProjectManager.h"
#include "../common/termfile.h"
#include "../common/template.h"
#include "../common/UndoManager.h"
// #include "../main/TermPlanDoc.h"
#include <afx.h>

CAirportInfo::CAirportInfo(void)
:DataSet(AirPortInfoFile)
{
	m_latitude=new CLatitude();
	m_longtitude=new CLongitude();
	m_dx = 0;
	m_dy = 0;
	m_elevation = 0;
//	m_bHideControl=TRUE;
}

CAirportInfo::~CAirportInfo(void)
{
	if (m_latitude)
		delete m_latitude;
	if (m_longtitude)
		delete m_longtitude;
}
/*
void CAirportInfo::loadDataSet(const CString& _pProjPath)
{
	clear();

	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename);
///	PROJMANAGER->getFullName(_pProjPath,"INPUT",
	ArctermFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet(_pProjPath, false);
		return;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
		saveDataSet(_pProjPath, false);
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
*/
bool CAirportInfo::loadDataSet(const CString& _pProjPath,int mode, int airport)
{
	clear();

	char filename[_MAX_PATH];
	// get the file full name
	PROJMANAGER->getFileName (_pProjPath, fileType, filename,2,airport);
	ArctermFile file;
	try { file.openFile (filename, READ); }
	catch (FileOpenError *exception)
	{
		delete exception;
		initDefaultValues();
		saveDataSet(_pProjPath,airport,"Airport1",false);
		return false;
	}

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
	{
		readObsoleteData( file );
		file.closeIn();
		saveDataSet(_pProjPath,airport,"Airport1",false);

		return false;
	}
	else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
		file.closeIn();
		throw new FileVersionTooNewError( filename );

		return false;
	}
	else
	{
		readData (file);
		file.closeIn();
		return true;
	}
	
	return false;
}

void CAirportInfo::readData(ArctermFile& p_file)
{
//	p_file.getLine();
 
	if(!p_file.isBlank ())
	{
		char airportName[256];
		p_file.getLine();
		p_file.getField(airportName,256);
		m_csAirportName=airportName;
		
		m_longtitude->ReadData(p_file);
		m_latitude->ReadData(p_file);

		p_file.getFloat(m_dx);
		p_file.getFloat(m_dy);

		p_file.getFloat(m_elevation);
		
	}
}

void CAirportInfo::clear()
{
	return;
}

void CAirportInfo::writeData(ArctermFile& p_file,CString strName) const
{
	p_file.writeField(strName);
	return;
}

void CAirportInfo::writeData(ArctermFile& p_file) const
{

	p_file.writeField(m_csAirportName);
	m_longtitude->WriteData(p_file);
	m_latitude->WriteData(p_file);
	
	p_file.writeDouble(m_dx);
	p_file.writeDouble(m_dy);
	
	p_file.writeDouble(m_elevation);
	return;
}


void CAirportInfo::saveDataSet(const CString& _pProjPath,int airport,CString strName,bool _bUndo)
{
	char filename[_MAX_PATH];
	PROJMANAGER->getFileName (_pProjPath, fileType, filename,2,airport);

	if( _bUndo )
	{
		CFile file1;
		CAirportInfo::CreateFile(file1,filename,CFile::modeReadWrite);

		//CUndoManager* undoMan = CUndoManager::GetInStance( _pProjPath );
		//if( !undoMan->AddNewUndoProject() )
		//{
		//	AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );
		//}


	}


	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	ArctermFile file;
	file.openFile (filename, WRITE, m_fVersion);

	file.writeField (getTitle());
	file.writeLine();

	file.writeField (getHeaders());
	file.writeLine();
	writeData (file);
	file.writeLine();
	file.endFile();

}

BOOL CAirportInfo::CreateFile(CFile& file,LPCTSTR lpszFileName, UINT nOpenFlags,int nCreationDisposition)
{
	BOOL result=FALSE;
	CFileException e;
	if (nCreationDisposition==OPEN_ALWAYS)
	{
		//打开已有的文件
		nOpenFlags &=~CFile::modeCreate;
		if (file.Open(lpszFileName,nOpenFlags))
		{
			return TRUE;
		}
	}
	nOpenFlags |= CFile::modeCreate;
	if (file.Open(lpszFileName,nOpenFlags,&e)==FALSE)
	{
//		m_nLastError=e.m_cause;
		switch(e.m_cause) 
		{
		case CFileException::badPath:
			{
				
				char *pTempFileName=new char[strlen(lpszFileName)+1];
				if (pTempFileName)
				{
					strcpy(pTempFileName,lpszFileName);
					char *pBuffer=strrchr(pTempFileName,'\\');
					if (pBuffer)
					{
						pBuffer[0]='\0';
						if (CreateDirectory(pTempFileName)==TRUE)
						{
							if (file.Open(lpszFileName,nOpenFlags,&e)==FALSE)
							{
//								m_nLastError=e.m_cause;
//								m_strLastError.Format("创建文件（%s）错误，错误号（%d）",lpszFileName,e.m_cause);
							}
							else
							{

								result=TRUE;
							}
						}
						else
						{
						}
						*pBuffer='\\';
					}
					delete pTempFileName;
				}
			}
			break;
		case CFileException::sharingViolation:	//共享冲突
			{
//				m_strLastError.Format("打开文件（%s）错误：可能该文件被别处打开",lpszFileName);
			}
			break;
		default:
//			m_strLastError.Format("代开文件（%s）错误",lpszFileName);
			break;
		}
	}
	else
		result=TRUE;
	return result;
}

BOOL CAirportInfo::CreateDirectory(LPCTSTR lpszDirectory)
{
	CString strErrorMessage;
	int i;
	BOOL bFlag=TRUE;
	BOOL bNetFlag=FALSE;
	char *pBuf,*bufTree=new char[lstrlen(lpszDirectory)+1];
	pBuf=bufTree;
	lstrcpy(bufTree,lpszDirectory);
	while(bufTree[lstrlen(bufTree)-1]=='\\')
	{
		bufTree[lstrlen(bufTree)-1]='\0';
	}

	HANDLE hHandle;
	WIN32_FIND_DATA FindData;
	CString strText,strDir;
	hHandle=FindFirstFile(bufTree,&FindData);
	if(hHandle!=INVALID_HANDLE_VALUE)
	{
		FindClose(hHandle);
		if(!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
//			strErrorMessage.Format("因为存在%s文件，拷贝不能正常进行!",bufTree);
			bFlag=FALSE;
			goto End;
		}
		else
			goto End;
	}
	strDir=bufTree;
	strDir+="\\";
	strText="";
	i=strDir.Find("\\\\");
	if(i>=0)
	{
		strText+="\\\\";
		strDir=strDir.Right(strDir.GetLength()-i-2);
		bNetFlag=TRUE;
	}
	i=strDir.Find('\\');
	if(i>=0)
	{
		strText+=strDir.Left(i+1);
		strDir=strDir.Right(strDir.GetLength()-i-1);
	}
	i=strDir.Find('\\');
	while(i>=0)
	{
		strText+=strDir.Left(i);
		strDir=strDir.Right(strDir.GetLength()-i-1);
		i=strDir.Find('\\');
		if(!bNetFlag)
		{
			hHandle=FindFirstFile(strText,&FindData);
			if(hHandle!=INVALID_HANDLE_VALUE)
			{
				FindClose(hHandle);
				if(!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
//					strErrorMessage.Format("因为存在%s文件，不能正常创建目录!",strText);
					bFlag=FALSE;
					goto End;
				}
			}
			else
			{
				if(!::CreateDirectory(strText,NULL))
				{
//					strErrorMessage.Format("创建%s目录错误!",strText);
					bFlag=FALSE;
					goto End;
				}
			}
		}
		else
			bNetFlag=FALSE;
		strText+='\\';
	}
End:
	delete pBuf;

	return bFlag;
}
void CAirportInfo::initDefaultValues()
{
	m_csAirportName="AirPort1";
//	m_latitude->SetValue("N 0:0.0");
//	m_longtitude->SetValue("S 0:0.0");
	m_dx=0;
	m_dy=0;
}

void CAirportInfo::GetPosition(double& dx,double& dy)
{
	dx=m_dx;
	dy=m_dy;
}
void CAirportInfo::SetPosition(double dx, double dy)
{
	m_dx = dx;
	m_dy = dy;
}

void CAirportInfo::GetElevation(double& elevation)
{
	elevation = m_elevation;
}

void CAirportInfo::SetElevation(double elevation)
{
	m_elevation =elevation;
}
