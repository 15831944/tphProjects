//////////////////////////////////////////////////////////////////////
// AutoCADLayerSettingsFile.h: interface for the 
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

#ifndef AUTOCADLAYERSETTINGSFILE_H
#define AUTOCADLAYERSETTINGSFILE_H

#include "AutoCADLayerSettingsLayer.h"

class CAutoCADLayerSettingsFile
{

private: 
	int m_numofLayers;
	CString m_AutoCADFileName;
	CAutoCADLayerSettingsLayer* m_Layer[256];

public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CAutoCADLayerSettingsFile(CString AutoCADFileName, int NumofLayers);
	~CAutoCADLayerSettingsFile();
	
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
	int getNumofLayers();

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
	CAutoCADLayerSettingsLayer* getLayer(int index);


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
	CString getAutoCADFileName ();

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
	void clear();
    
};

#endif