/*****
*
*   Class       CLogFile
*   Author      Porter Ma, Aviation Research Corporation
*   Date        November 25, 2006
*   Purpose     take charge of Log file,read and save Log information. 
*               Also has methods to read specific  structures
*               from fixed format input files.and it apply to other project
*
*
*****/
#pragma once
#pragma warning(disable:4275)

#include <fstream>
#include <share.h>

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

#define stdinfstream std::ifstream
#define stdoutfstream std::ofstream
#define stdios std::ios

#define SH_DENYNONE _SH_DENYNO


class infstream : public stdinfstream
{

public:
	infstream(){}
	infstream (const char *szName, stdios::openmode nMode = stdios::in, int nShare = SH_DENYNONE);
	virtual void open (const char *szName, stdios::openmode nMode = stdios::in, int nShare = SH_DENYNONE);
	virtual ~infstream() { ;}
};

class outfstream : public stdoutfstream
{
public:
	outfstream() {}
	outfstream (const char *szName, stdios::openmode nMode = stdios::out, int nShare = SH_DENYRW) ;
	virtual void open (const char *szName, stdios::openmode nMode = stdios::out, int nShare = SH_DENYRW);
	virtual ~outfstream() {;}
};

// ASCII classification macro
#define isNumeric(x)  ((x>=48&&x<=57)||x==43||x==45||x==46)


class  CLogFile
{
protected:
	infstream fpIn;
	outfstream fpOut;
	char dataLine[LARGE];
	int curPos;
	int lineLength;

public:
	CLogFile () { curPos = lineLength = 0; };
	CLogFile (const char *filename, int fileType, int p_shareMode = SH_DENYRW);

	void closeIn (void) { fpIn.close(); };
	void closeOut (void) { fpOut.close(); };

	// Opens the file for reading, writing, or appending
	int init (const char *filename, int fileType, int p_shareMode = SH_DENYRW);

	// tests line to see if it contains any data
	int isBlank (void);
	int isBlankField (void) { return (dataLine[curPos] == COMMA) ||
		(dataLine[curPos] == '\0') || (curPos >= lineLength); };

	int openFile (const char *p_filename, int p_op);

	// Reads the next line from the file into dataLine
	// Returns true if successful, false if EOF, -1 if blank line
	int getLine (void);

	// flushes dataLine to output stream
	int writeLine (void);

	// tests fp for end of file
	int eof (void) { return (fpIn.eof() || fpIn.peek() == EOF); };

	// Reads a field ending with a comma, newline, or at maxlen
	// NULL character is appended, not delimiter
	// CurPos is incremented to the next field
	// Returns FALSE if the field is blank, otherwise the field length
	int getField (char *str, int maxlen);

	// copies a string field to the dataLine
	void writeField (const char *field) { insertField (field); };
	void insertField (const char *field);

	// advance current position count fields
	void skipField (int count);
	void appendField (const char *field);

	// Reads a int integer from the currentPosition
	// Increments curPos past the first non-digit character
	// Returns FALSE if dataLine[curPos] is not a digit
	int getInteger (int& anInt);

	// copies an int field to the dataLine
	void writeInt (int integer);


};

