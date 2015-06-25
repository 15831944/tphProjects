/*****
*
*   Class       CSVFile
*   Author      Lincoln Felingham, Aviation Research Corporation
*   Date        November 16, 1992, July 30, 1993
*   Purpose     Provides a generic interface to input files for ARC
*               applications
*               Contains methods to correctly read each field from a
*               comma separated file
*               Can also generate .CSV files given a specific format
*               Must be subclassed in order to read time or date formats
*
*****/

#ifndef CSV_FILE_H
#define CSV_FILE_H

#include "fsstream.h"

#define SMALL           48
#define LARGE           102400//2048
#define TRUE            1
#define FALSE           0
#define END             -1
#define BLANK_LINE      -1

// Character definitions
#define CR              '\n'
#define SPACE           ' '
#define arcDECIMAL         '.'
#define COMMA           ','
#define HYPHEN          '-'
#define SEMI_COLON      ';'
#define OPEN_PAREN      '('
#define CLOSE_PAREN     ')'

// type of file operation
#define READ            0
#define WRITE           1
#define APPEND          2


// ASCII classification macro
#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)

class COMMON_TRANSFER CSVFile
{
protected:
    ifsstream fpIn;
    ofsstream fpOut;
	char dataLine[LARGE];
    int curPos;
    int lineLength;

public:
    CSVFile () { curPos = lineLength = 0; };
	CSVFile (const char *filename, int fileType, int p_shareMode = SH_DENYRW);

    // Opens the file for reading, writing, or appending
    int init (const char *filename, int fileType, int p_shareMode = SH_DENYRW);

    void closeIn (void) { fpIn.close(); };
    void closeOut (void) { fpOut.close(); };

    // Repositions the file pointer to the start of the file
    void reset (void) { fpIn.seekg (0l); fpIn.clear(); };

    // tests line to see if it contains any data
    int isBlank (void);
    int isBlankField (void) { return (dataLine[curPos] == COMMA) ||
        (dataLine[curPos] == '\0') || (curPos >= lineLength); };

    // advances to next line
    void skipLine (void);

    // advances skip lines
	void skipLines (int skip);

    // advance current position count fields
    void skipField (int count);

    // sets current position to the start of a specific field number
    // field numbers are zero based
    void setToField (int fieldNum);

    // returns the number of characters in the current field
    int getFieldLength (void);

    // tests fp for end of file
    int eof (void) { return (fpIn.eof() || fpIn.peek() == EOF); };

    // tests for end of line
    int eol (void) { return (curPos >= lineLength); };

    // counts the number of lines in the file
    int countLines (void);

	// loads the row with the value "key" in the passed column
	int seekToRecord (const char *key, int fieldnum);

    // seeks to passed line number (1 based)
    int seekToLine (int lineNum);

    // Reads the next line from the file into dataLine
    // Returns true if successful, false if EOF, -1 if blank line
    virtual int getLine (void);

    // copies the dataLine into string
    void copyLine (char *string);

    // Reads a single character from current position
    // Returns FALSE if curPos is the end of a field
    // Increments curPos to point to the start of the next field,
    // Assumes that the current field is 1 character long
    int getChar (char& aChar);

    // tests current line to determine if the first field is blank
    int isNewEntryLine (void) const;

    // Returns a count of the number of fields in the current line
    int countColumns (void);

    // Returns the current field number (zero based)
    int getColumn (void);

    // reads n bytes into str
    // does not skip to next field
    // returns TRUE if it was able to read n bytes
    int getNLengthField (char *str, int n);

    // Reads a field ending with a comma, newline, or at maxlen
    // NULL character is appended, not delimiter
    // CurPos is incremented to the next field
    // Returns FALSE if the field is blank, otherwise the field length
    int getField (char *str, int maxlen);

    // Reads a field ending with a comma, hyphen, newLine, semi-colon,
    // or in fact just about anything else
    // Delimiting character is not appended
    // CurPos is incremented past the delimiting character
    // Returns the delimiting character, or NULL if blank field
    char getShortField (char *str, int maxlen);

	// Reads a int integer from the currentPosition
    // Increments curPos past the first non-digit character
    // Returns FALSE if dataLine[curPos] is not a digit
    int getInteger (short& anInt);

	// Reads a int integer from the currentPosition
    // Increments curPos past the first non-digit character
    // Returns FALSE if dataLine[curPos] is not a digit
    int getInteger (int& anInt);

	// Reads a long integer from the currentPosition
    // Increments curPos past the first non-digit character
    // Returns FALSE if dataLine[curPos] is not a digit
	int getInteger (long& anInt);

    // Reads a float from the currentPosition
    // Increments curPos past the first non-digit character
    // Returns FALSE if dataLine[curPos] is not a digit
    int getFloat (float& aFloat);

    // Reads a double from the currentPosition
    // Increments curPos past the first non-digit character
    // Returns FALSE if dataLine[curPos] is not a digit
    int getFloat (double& aFloat);

    // Reads a two part flight ID into the passed parameters
    // Optional hyphen between airline and id
    // Airline: 3 characters (NO NULL CHARACTER APPENDED)
    // acID: 4 characters (NO NULL CHARACTER APPENDED)
    // Conventions:
    //      default value of "FLEX": airline set to ANY_AIRLINE,
    //      aircraft id is set to ANY_ACID
    //      value of "NONE": airline set to NONE
    //      in both cases, acID is set to all NULL characters
//    int getFlightID (char *airline, char *acID);

    // flushes dataLine to output stream
    int writeLine (void);

    // writes a CR to output stream
    void blankLine (void) { fpOut << CR; };
    void writeBlankLines (int lines);

    void writeBlankField (void);

    void writeBlankFields (int fields);

    // copies a single character field to the dataLine
    void writeChar (char c);

    // copies a string field to the dataLine
    void writeField (const char *field) { insertField (field); };

    // copies a short int field to the dataLine
    void writeInt (short integer);

    // copies an int field to the dataLine
    void writeInt (int integer);

    // copies a long int field to the dataLine
    void writeInt (long integer);

    // copies a floating point field to the dataLine
    void writeFloat (float aReal, int precision = 4);

	// copies a floating point field to the dataLine
    void writeDouble (double aReal, int precision = 4);

    // removes current field from existence, moves all following columns
    // one to the left
    void deleteField (void);

    // removes contents of current field
    void deleteValue (void);

    void appendField (const char *field);
    void appendValue (const char *field);
    void insertField (const char *field);
    void insertValue (const char *field);
	void InitDataFromString( const char* _sData );
};

#endif
