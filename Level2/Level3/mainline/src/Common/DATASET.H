/*****
*
*   Class       DataSet
*   Author      Lincoln Fellingham
*   Date        April 6, 1996
*   Purpose     Defines a standardized data set with basic I/O
*               functions. All input data sets in ARCTERM will be
*               derived from this class.
*
*               Key functionality includes file I/O, file version
*               handling, and cleanup functions
*
*				Nov. 12, 2002
*				File version control:
*				1. if the version is newer, fail
*				2. if the version is older, read older version and create new version.
*				
*
*****/

#ifndef DATA_SET_H
#define DATA_SET_H

#include "termfile.h"
class InputTerminal;
//Jan98
//#include "inputs\pax_db.h"
#include "CAutoSaveHandlerInterface.h"
class COMMON_TRANSFER DataSet
{
protected:
	InputTerminal* m_pInTerm;
    float m_fVersion;
    int fileType;
		
public:
    DataSet (int p_file, float p_version = 2.2f);
    virtual ~DataSet () {};

	void SetInputTerminal( InputTerminal* _pInTerm ){ m_pInTerm = _pInTerm; }
	InputTerminal* GetInputTerminal() { return m_pInTerm;	};
    void setFileType (int p_file) { fileType = p_file; }
    int getFileType (void) const { return fileType; }
    virtual void initDefaultValues (void) {};
    
	// clear data structure before load data from the file.
	virtual void clear (void) = 0;

	// Description: Read Data From Default File. Creat new if no file exist. Update to newer version if necessary.
	// Exception:	FileVersionTooNewError
	virtual void loadDataSet (const CString& _pProjPath);
	
	// Exception:	DirCreateError;
	virtual void saveDataSet (const CString& _pProjPath, bool _bUndo) const;

	// Description: Read Data From Specific File. Init the data if no file exist.
	// Exception:	FileVersionTooNewError
	void loadDataSetFromOtherFile( const CString& _csFileName );

	void saveDataSetToOtherFile (const CString& _csFileName) const;

	virtual void readData (ArctermFile& p_file) = 0;
	virtual void readObsoleteData ( ArctermFile& p_file ){ readData(p_file); }
	virtual void writeData (ArctermFile& p_file) const = 0;

	virtual const char *getTitle (void) const = 0;
	virtual const char *getHeaders (void) const = 0;

	 //Jan98 : added for protection in flight departing window
//	 virtual PassengerConstraintDatabase* getPassengerConstraintDatabase(int);
};

#endif
