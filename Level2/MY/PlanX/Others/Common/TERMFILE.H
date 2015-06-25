/*****
*
*   Class       ArctermFile
*   Author      Lincoln Felingham, Aviation Research Corporation
*   Date        November 16, 1992
*   Purpose     Provides a generic interface to input files for ARCTRM
*               Contains methods to correctly read each field from the
*               file, assigning all the appropriate variables and adding
*               to the StringDictionaries when needed
*
*               Also has methods to read specific Arcterm structures
*               from fixed format input files
*
*   Revisions   Feb 5, 96 - LF - Added version handling methods
*
*****/

#ifndef ARCTERM_FILE_H
#define ARCTERM_FILE_H

#include "template.h"
#include "elaptime.h"
#include "csv_file.h"

class CPoint2008;

class COMMON_TRANSFER ArctermFile : public CSVFile
{
protected:
    float m_fversion;
    int trimSpaces;

public:
	ArctermFile();
    virtual ~ArctermFile () {};

    // opens a CSV file, skipping header lines if OPEN operation
    int openFile (const char *p_filename, int p_op, float p_ver = 2.2);
	//used by airside import
    int openAirsideFile (const char *p_filename, int p_op, int p_ver = 10000);
    // Reads the next line from the file into dataLine
    // Returns true if successful, false if EOF, -1 if blank line
    int getLine (void);

    const char *getDataLine (void) const { return dataLine; };

    // sets all characters to uppercase and removes any blanks
    void format (void);
    void setTrimSpaces (int p_flag) { trimSpaces = 0; }

    // returns number of fields in the current line delimited by separator
    short countSubFields (char separator);

    // copies a sub-field (delimited by semi-colon) to str
    short getSubField (char *subField, char separator);

    // returns TRUE if field == ALLPROCESSORS
    int allProcs (void);

    // Reads a time into the passed ElapsedTime instance
    int getTime (ElapsedTime& time, int readSeconds = FALSE);

    // Writes the passed ElapsedTime to the next Field
    int writeTime (const ElapsedTime& time, int writeSeconds = FALSE);

    // Reads a coordinate from a file, scaling it appropriately
    int getPoint (Point& aPoint);

	// Reads a coordinate from a file, scaling it appropriately
	int getPoint2008 (CPoint2008& aPoint);

    // Reads a coordinate from a file, scaling it appropriately
    void writePoint (const Point& aPoint);

	// Reads a coordinate from a file, scaling it appropriately
	void writePoint2008(const CPoint2008& aPoint);

    // writes blank line and Date and Time stamp
    void endFile (void);

    long getPos (void) const { return curPos; }
    void setPos (long p_pos) { curPos = p_pos; }

    // Reads a probability distribution into the appropriate subclass of
    // ProbabilityDistribution and returns a generic pointer to it
    // Assumes that the current position is valid
//    ProbabilityDistribution *getProbDistribution (void);

    // file version number handling
    float getVersion (void) const { return m_fversion; }
    void setVersion (float p_ver) { m_fversion = p_ver; }
};

#endif
