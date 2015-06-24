//////////////////////////////////////////////////////////////////////
// AutoCADLayerSettings.h: Implementation for the 
// CAutoCADLayerSettingsDataSet class.
//
// This is the high level of the design for allowing users to save 
// their own names, colors and visability settings within ARCport.
// Each project will contain one AutoCAD Layer Settings file. This 
// file can be broken up into the individual AutoCAD drawing files 
// used within the project (CAutoCADLayerSettingsFile). Each of 
// these files are futher broken up into their layers 
// (CAutoCADLayerSettingsLayer).
//
//  
// Assumptions:
//
//		1) Maximum number of AutoCAD files (not layers as each file 
//		can have many layers) is defined as Max_AutoCAD_Files and 
//		is currently set to 64 (same as MAX_FLOOR in Termplan/3dview.cpp).
//
//	Written By: Cary Feldstein
//	December 6, 2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "common\projectmanager.h"
#include "common\exeption.h"
#include "common\UndoManager.h"
#include "assert.h"
#include "AutoCADLayerSettings.h"

//////////////////////////////////////////////////////////////////////
// Construction
//////////////////////////////////////////////////////////////////////
CAutoCADLayerSettings::CAutoCADLayerSettings():DataSet( 1/*AutoCADLayerSettingsFile*/ )
//TODO add AutoCADLayerSettingsFile to template.h?
{

}

//////////////////////////////////////////////////////////////////////
// Destruction
//////////////////////////////////////////////////////////////////////
CAutoCADLayerSettings::~CAutoCADLayerSettings()
{

}

//////////////////////////////////////////////////////////////////////
// Function: addAutoCADFile
// 
// adds a AutoCAD drawing file to the layer settings. This will open
// the file and populate the File and Layer settings. 
//
// Parameters:
//	1) CString fileName - The full path name of the AutoCAD drawing to use
//	2) int index - The location in the m_AutoCADFiles array to store this
//                 drawings data
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::addAutoCADFile(CString fileName, int index)
{
	//TODO find way to get AutoCAD file's number of layers to pass to call below.
	m_AutoCADFiles[index] = new CAutoCADLayerSettingsFile(fileName,1);

}

//////////////////////////////////////////////////////////////////////
// Function: removeAutoCADFile
// 
// removes a AutoCAD drawing file to the layer settings. This will 
// delete all entries for the particular file
//
// Parameters:
//		1) int index - The location in the m_AutoCADFiles array to delete
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::removeAutoCADFile(int index)
{
	m_AutoCADFiles[index]->clear();
	delete m_AutoCADFiles[index];
	m_AutoCADFileInUse[index] = false;

}
//////////////////////////////////////////////////////////////////////
// Function: loadDataSet
// 
// loads the data set from the ARCport data file
//
// Parameters:
//		1) CString& _pProjPath - The project path
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::loadDataSet (const CString& _pProjPath)
{
	clear();

    char filename[_MAX_PATH];
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
	
	//saveSet

		file.openFile (filename, WRITE, m_fVersion);
        file.writeField (getTitle());
        file.writeLine();
        file.writeField (getHeaders());
        file.writeLine();
        writeData (file);
        file.endFile();

}
//////////////////////////////////////////////////////////////////////
// Function: saveDataSet
// 
// saves the data set from the ARCport data file
//
// Parameters:
//		1) CString& _pProjPath - The project path
//		2) bool _bUndo - The undo setting
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::saveDataSet (const CString& _pProjPath, bool _bUndo) const
{
	//if( _bUndo )
	//{
	//	CUndoManager* undoMan =CUndoManager::GetInStance( _pProjPath );
	//	if( !undoMan->AddNewUndoProject() )
	//		AfxMessageBox( "Can not create UNDO folder, UNDO did not proceeded", MB_OK|MB_ICONWARNING );

	//}
	if(CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE)
		CAutoSaveHandlerInterface::PAUTOSAVE_HANDLE->DoAutoSave() ;
	char filename[_MAX_PATH];
    PROJMANAGER->getFileName (_pProjPath, fileType, filename);

    ArctermFile file;
    file.openFile (filename, WRITE, m_fVersion);

    file.writeField (getTitle());
    file.writeLine();

    file.writeField (getHeaders());
    file.writeLine();

    writeData (file);

    file.endFile();

}

//////////////////////////////////////////////////////////////////////
// Function: readData
// 
// Function inherited from DataSet Class. 
//
// Parameters:
//		1) ArctermFile& p_file - pointer to settings file.
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::readData(ArctermFile& p_file)
{
	p_file.getLine();

	while (!p_file.isBlank ())
	{
		int nCount;
		int fileCount = 0;
		int numLayers;
		char t_Name[MAX_PATH];
		p_file.getInteger(nCount);

		for (int i=0; i < nCount; i++)
		{
			p_file.getLine();

			p_file.getInteger(fileCount);
			p_file.getField(t_Name, MAX_PATH);
			p_file.getInteger(numLayers);

			m_AutoCADFileInUse[i] = true;
			m_AutoCADFiles[i] = new CAutoCADLayerSettingsFile(CString(t_Name), numLayers);

			for (int j=0; j<numLayers; j++)
			{
				int tempval;
				p_file.getInteger(tempval);
				p_file.getField (t_Name, MAX_PATH);
				m_AutoCADFiles[i]->getLayer(j)->setLayerName(CString(t_Name));
                p_file.getInteger(tempval);
				m_AutoCADFiles[i]->getLayer(j)->setAutoCADFileLayerNum(tempval);
				p_file.getInteger(tempval);
				m_AutoCADFiles[i]->getLayer(j)->setColor(tempval);
				p_file.getInteger(tempval);

				if (tempval)
				{
					m_AutoCADFiles[i]->getLayer(j)->showLayer();
				}
				else
				{
					m_AutoCADFiles[i]->getLayer(j)->hideLayer();
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
// Function: readObsoleteData
// 
// Function inherited from DataSet Class. Just called readData right now
// as only one version of file exists.
//
// Parameters:
//		1) ArctermFile& p_file - pointer to settings file.
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::readObsoleteData(ArctermFile &p_file)
{
	readData(p_file);
}
//////////////////////////////////////////////////////////////////////
// Function: writeData
// 
// Function inherited from DataSet Class. 
//
// Parameters:
//		1) ArctermFile& p_file - pointer to settings file.
//
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::writeData (ArctermFile& p_file) const
{
	int nCount = m_NumOfAutoCADFiles;
	int fileCount = 0; //Used to delete holes in array when writing data 
	
	p_file.writeInt(nCount);
	p_file.writeLine();

	for( int i=0; i<Max_AutoCAD_Files; i++ ) //for each autoCAD file
	{

		if (m_AutoCADFileInUse[i]) // then write the file data
		{
			fileCount++;

			CAutoCADLayerSettingsFile* pCADFile = m_AutoCADFiles[i];
			
			p_file.writeInt(fileCount);
			p_file.writeField( pCADFile->getAutoCADFileName().GetString());
			
			int numLayers = pCADFile->getNumofLayers();
			p_file.writeInt(numLayers);

			for (int j=0; j<numLayers; j++) // write each layer data
			{
				CAutoCADLayerSettingsLayer* pCADLayer = pCADFile->getLayer(j);
				p_file.writeInt(j);
				p_file.writeField(pCADLayer->getLayerName());
				p_file.writeInt(pCADLayer->getAutoCADFileLayerNum());
				p_file.writeInt(pCADLayer->getColor());
				p_file.writeInt((int)pCADLayer->getDisplaySetting());
				
			}

			p_file.writeLine();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// Function: clear
// 
// Function inherited from DataSet Class. 
//
// Parameters: None
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettings::clear()
{
	for (int i=0; i < Max_AutoCAD_Files; i++)
	{
		if (m_AutoCADFileInUse[i])
		{
			removeAutoCADFile(i);
		}
	}


}