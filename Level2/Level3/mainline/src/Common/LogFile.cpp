
#include "stdafx.h"

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fcntl.h>      //O_XXXXXX
#include <sys\stat.h>   //S_IXXXXX, stat()
#include <share.h>      //SH_XXXXX
#include <io.h>         //sopen(), close(), read(), write()
using namespace std ;

#include "exeption.h"
#include "LogFile.h"

infstream::infstream (const char *szName, ios_base::openmode  nMode, int nShare)
  : ifstream(szName, nMode){ ;}

void infstream::open (const char *szName, ios_base::openmode  nMode, int nShare)
{ 
	ifstream::open (szName, nMode); 
}

outfstream::outfstream (const char *szName, ios_base::openmode  nMode , int nShare) 
  :ofstream(szName, nMode)
{ ; }

void outfstream::open (const char *szName, ios_base::openmode  nMode , int nShare)
{ ofstream::open (szName, nMode); }



CLogFile::CLogFile(const char *filename, int fileType,int)
{
	curPos = lineLength = 0;
	init (filename, fileType);
}

// Opens the file in read, write, or append mode
// Reads the first line from the file
// Returns TRUE if file opened successfully
int CLogFile::init (const char *filename, int fileType,int)
{
	switch (fileType)
	{
	case READ:
		fpIn.open (filename);
		if (fpIn.bad())
			throw new FileOpenError (filename);
		return getLine();

	case WRITE:
		fpOut.open (filename);
		if (fpOut.bad())
			throw new FileCreateError (filename);
		break;

	case APPEND:
		fpOut.open (filename, stdios::app);
		if (fpOut.bad())
			throw new FileOpenError (filename);
		break;

	default:
		throw new StringError ("Unknown file operation requested");
	}
	return TRUE;
}

int CLogFile::openFile(const char *p_filename, int p_op)
{
	if (!CLogFile::init (p_filename, p_op))
		throw new FileOpenError (p_filename);

	return TRUE;
}

// Reads the next line from the file into dataLine
// Returns true if successful, false if EOF, -1 if blank line
int CLogFile::getLine (void)
{
	if (eof())
		return FALSE;

	fpIn.getline (dataLine, LARGE);
	curPos = 0;

	// get the length of the dataLine
	lineLength = strlen (dataLine);

	return (isBlank())? -1: TRUE;
}

// tests line to see if it contains any data
int CLogFile::isBlank (void)
{
	if (dataLine[0] == NULL)
		return TRUE;

	// determine if the entire line contains blank fields
	for (int i = 0; i < lineLength; i++)
		if (dataLine[i] != COMMA)
			return FALSE;

	return TRUE;
}

// Reads a field ending with a comma, newline, or at maxlen
// Delimiting character is not appended
// CurPos is incremented to the next field
// Returns FALSE if the current field is blank
int CLogFile::getField (char *str, int maxlen)
{
	int i = 0;

	while (dataLine[curPos] != COMMA && i < maxlen && curPos < lineLength)
		str[i++] = dataLine[curPos++];

	if (dataLine[curPos] != COMMA && i == maxlen)
	{
		str[--i] = NULL;
		skipField (1);
	}
	else
	{
		str[i] = NULL;
		curPos++;
	}

	CString csStr = str;
	csStr.TrimLeft();
	csStr.TrimRight();
	strcpy( str, csStr );

	// returns count of characters in field
	return i;
}

// advance current position count fields
void CLogFile::skipField (int count)
{
	for (int i = 0; i < count; i++)
		while (curPos < lineLength && dataLine[curPos++] != COMMA);
}

// flushes dataLine to output stream
int CLogFile::writeLine (void)
{
	dataLine[lineLength] = CR;
	fpOut.write (dataLine, lineLength+1);

	curPos = lineLength = 0;
	dataLine[curPos] = NULL;

	// return status of fpOut
	return TRUE;
}

// assumes curPos is the start of a field
// field will be inserted previous to the current one
void CLogFile::insertField (const char *field)
{
	if (curPos >= lineLength)
	{
		curPos = lineLength;
		appendField (field);
		return;
	}

	int size = strlen (field);
	if (size + lineLength + 1 >= LARGE)
		return;

	lineLength += size + 1;

	char temp[LARGE];
	strcpy (temp, dataLine+curPos);

	memcpy (dataLine+curPos, field, size);
	curPos += size;
	dataLine[curPos++] = COMMA;
	strcpy (dataLine+curPos, temp);
}

// copies the passed field to the end of the line, ensuring it does not
// exceed the maximum line length
void CLogFile::appendField (const char *field)
{
	if (lineLength)
		dataLine[lineLength++] = COMMA;
	curPos = lineLength;

	int size = strlen (field);
	lineLength += size;

	if (lineLength >= LARGE)
	{
		size = LARGE - curPos - 1;
		lineLength = LARGE - 1;
	}

	memcpy (dataLine+curPos, field, size);
	dataLine[lineLength] = NULL;
	curPos = lineLength;
}

// Reads an integer from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CLogFile::getInteger (int& anInt)
{
	int i = 0;
	char str[SMALL];

	// create str from the digits in the field
	while (isNumeric (dataLine[curPos]))
		str[i++] = dataLine[curPos++];

	// convert the string into an integer
	str[i] = NULL;
	anInt = atoi (str);
	curPos++;

	// returns count of digits
	return i;
}

// copies a int int field to the dataLine
void CLogFile::writeInt (int integer)
{
	char string[SMALL];
	_itoa (integer, string, 10);
	insertField (string);
}