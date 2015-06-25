#include "stdafx.h"
#include "assert.h"
#include "dataset.h"
#include "exeption.h"
#include "projectmanager.h"
#include "UndoManager.h"
#include "ARCTracker.h"
#include "Shlwapi.h"
#include "fileman.h"


// Description: Read Data From Default File. Creat new if no file exist. Update to newer version if necessary.
// Exception:	FileVersionTooNewError;
void DataSet::loadDataSet (const CString& _pProjPath)
{
	PLACE_METHOD_TRACK_STRING();
    clear();

	char filename[_MAX_PATH] = {0};
    PROJMANAGER->getFileName (_pProjPath, fileType, filename);

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


// Description: Read Data From Specific File. Init the data if no file exist.
// Exception:	FileVersionTooNewError
void DataSet::loadDataSetFromOtherFile( const CString& _csFileName )
{
	PLACE_METHOD_TRACK_STRING();
    clear();

    ArctermFile file;
    try { file.openFile (_csFileName, READ); }
    catch (FileOpenError *exception)
    {
		delete exception;
        initDefaultValues();
        return;
    }

	float fVersionInFile = file.getVersion();

	if( fVersionInFile < m_fVersion || fVersionInFile == 21 )
    {
        readObsoleteData( file );
        file.closeIn();
    }
    else if( fVersionInFile > m_fVersion )
	{
		// should stop read the file.
        file.closeIn();
		throw new FileVersionTooNewError( _csFileName );		
	}
    else
    {
        readData (file);
        file.closeIn();
    }
}


void DataSet::saveDataSet (const CString& _pProjPath, bool _bUndo ) const
{
	PLACE_METHOD_TRACK_STRING();
	//if( _bUndo )
	//{
	//	CUndoManager* undoMan = CUndoManager::GetInStance( _pProjPath );
	//	if( !undoMan->AddNewUndoProject() )
	//	{
	//		AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );
	//	}
	//}
    //check if need save the project 
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
    char filename[_MAX_PATH];
    PROJMANAGER->getFileName (_pProjPath, fileType, filename);

	//check file path ready exsit, if no, create the path
	CString strPathFile(filename);
	int nPos = strPathFile.ReverseFind('\\');
	if (nPos != -1)
	{
		CString strFullPath = strPathFile.Left(nPos);

		if (!PathFileExists((LPCSTR)strFullPath))
		{
			FileManager::MakePath(strFullPath);
		}
	}
	

    ArctermFile file;
    file.openFile (filename, WRITE, m_fVersion);

    file.writeField (getTitle());
    file.writeLine();

    file.writeField (getHeaders());
    file.writeLine();

    writeData (file);

    file.endFile();
}

void DataSet::saveDataSetToOtherFile (const CString& _csFileName) const
{
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
    ArctermFile file;
    file.openFile (_csFileName, WRITE, m_fVersion);

    file.writeField (getTitle());
    file.writeLine();

    file.writeField (getHeaders());
    file.writeLine();

    writeData (file);

    file.endFile();
}

DataSet::DataSet( int p_file, float p_version /*= 2.2*/ )
{
	fileType = p_file; m_fVersion = p_version; m_pInTerm = NULL;
}
//Jan98
/*
PassengerConstraintDatabase* DataSet::getPassengerConstraintDatabase(int i_nx)
{
	PassengerConstraintDatabase* tmp = new PassengerConstraintDatabase();
	return tmp;
}
*/