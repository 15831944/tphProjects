//////////////////////////////////////////////////////////////////////
// AutoCADLayerSettingsFile.cpp: implementation for the 
// CAutoCADLayerSettingsFile class.
//
// Private Members:
//   int m_numofLayers : Number of layers within this AutoCAD File (fixed)
//	 CString m_AutoCADFileName : AutoCAD full path file name to read in layer info
//								during construction.
//	 CAutoCADLayerSettingsLayer* m_Layer[] : Array of Layer object pointers.
//
//	Written By: Cary Feldstein
//	December 6, 2005
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AutoCADLayerSettingsFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CAutoCADLayerSettingsFile::CAutoCADLayerSettingsFile(CString AutoCADFileName, int NumofLayers)
{
	m_AutoCADFileName = AutoCADFileName;
	m_numofLayers = NumofLayers;

	for (int i=0; i<m_numofLayers; i++)
	{
		m_Layer[i] = new CAutoCADLayerSettingsLayer("1",true,1,i);

	}
}

CAutoCADLayerSettingsFile::~CAutoCADLayerSettingsFile()
{

}

//////////////////////////////////////////////////////////////////////
// Function: getNumofLayers
// 
// gives number of layers for this file.
//
// Parameters: None
//
// Return Value: m_numofLayers
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
int CAutoCADLayerSettingsFile::getNumofLayers()
{
 return m_numofLayers;
}

//////////////////////////////////////////////////////////////////////
// Function: getLayer
// 
// gives a pointer to the requested layer. 
//
// Parameters: 
//		1) int index : layer index to return
//
// Return Value: m_Layer[index]
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
CAutoCADLayerSettingsLayer* CAutoCADLayerSettingsFile::getLayer(int index)
{
	return m_Layer[index];
}


//////////////////////////////////////////////////////////////////////
// Function: getAutoCADFileName
// 
// returns the full path file name to the file. 
//
// Parameters: None
//		
// Return Value: CString
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
CString CAutoCADLayerSettingsFile::getAutoCADFileName()
{
	return m_AutoCADFileName;
}

//////////////////////////////////////////////////////////////////////
// Function: clear
// 
// deletes all layers that are members of this class. 
//
// Parameters: None
//		
// Return Value: None
//
// Exceptions: None
//
//////////////////////////////////////////////////////////////////////
void CAutoCADLayerSettingsFile::clear()
{
	for (int i=0; i < m_numofLayers; i++)
	{
		delete m_Layer[i];
	}
}
