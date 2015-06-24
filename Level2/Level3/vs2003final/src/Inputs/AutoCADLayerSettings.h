//////////////////////////////////////////////////////////////////////
// AutoCADLayerSettings.h: interface for the 
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

#ifndef AUTOCADLAYERSETTINGS_H
#define AUTOCADLAYERSETTINGS_H

#include "..\common\dataset.h"
#include "AutoCADLayerSettingsFile.h"

#define Max_AutoCAD_Files 64

class CAutoCADLayerSettings : public DataSet
{

private:
	int m_NumOfAutoCADFiles;

	//array containing the AutoCADFiles used in this project
	CAutoCADLayerSettingsFile* m_AutoCADFiles[Max_AutoCAD_Files];

	// Boolean flag to mark if the index value contains a valid
	// pointer
	bool m_AutoCADFileInUse[Max_AutoCAD_Files];

public:

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	CAutoCADLayerSettings();
	~CAutoCADLayerSettings();

public:

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
	void addAutoCADFile(CString fileName, int index);

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
	void removeAutoCADFile(int index);
	
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
	void loadDataSet (const CString& _pProjPath);

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
	void saveDataSet (const CString& _pProjPath, bool _bUndo) const;


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
	void readData (ArctermFile& p_file);
	
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
	void readObsoleteData (ArctermFile& p_file);

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
	void writeData (ArctermFile& p_file) const;

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
	void clear (void);

	//////////////////////////////////////////////////////////////////////
	// Function: getTitle
	// 
	// Function inherited from DataSet Class. 
	//
	// Parameters: None
	//		
	// Return Value: "AutoCAD Layer Settings file"
	//
	// Exceptions: None
	//
	//////////////////////////////////////////////////////////////////////
	virtual const char *getTitle (void) const { return "AutoCAD Layer Settings file" ;};

	//////////////////////////////////////////////////////////////////////
	// Function: getHeaders
	// 
	// Function inherited from DataSet Class. 
	//
	// Parameters: None
	//		
	// Return Value: "AutoCAD Layer Settings data"
	//
	// Exceptions: None
	//
	//////////////////////////////////////////////////////////////////////
	virtual const char *getHeaders (void) const {return "AutoCAD Layer Settings data " ;};

};

#endif