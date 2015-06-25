#include "stdafx.h"

#include <fstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "csv_file.h"
#include "exeption.h"

CSVFile::CSVFile (const char *filename, int fileType, int)
{
    curPos = lineLength = 0;
    init (filename, fileType);
}

// Opens the file in read, write, or append mode
// Reads the first line from the file
// Returns TRUE if file opened successfully
int CSVFile::init (const char *filename, int fileType, int)
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


// Reads the next line from the file into dataLine
// Returns true if successful, false if EOF, -1 if blank line
int CSVFile::getLine (void)
{
    if (eof())
        return FALSE;

    fpIn.getline (dataLine, LARGE);
    curPos = 0;

    // get the length of the dataLine
    lineLength = strlen (dataLine);

    return (isBlank())? -1: TRUE;
}

// copies the dataLine into string
void CSVFile::copyLine (char *string)
{
    memcpy (string, dataLine, lineLength+1);
}


// tests line to see if it contains any data
int CSVFile::isBlank (void)
{
    if (dataLine[0] == NULL)
        return TRUE;

    // determine if the entire line contains blank fields
	for (int i = 0; i < lineLength; i++)
        if (dataLine[i] != COMMA)
            return FALSE;

    return TRUE;
}


void CSVFile::skipLine (void)
{
    if (fpIn.peek() == EOF)
        return;

    char temp[LARGE];
    fpIn.getline (temp, LARGE);
}


// advances skip lines
void CSVFile::skipLines (int skip)
{
	for (int i = 0; i < skip; i++)
		skipLine();
}


// advance current position count fields
void CSVFile::skipField (int count)
{
    for (int i = 0; i < count; i++)
        while (curPos < lineLength && dataLine[curPos++] != COMMA);
}


// sets current position to the start of a specific field number
// field numbers are zero based
void CSVFile::setToField (int fieldNum)
{
    if (fieldNum == END)
    {
        curPos = lineLength;
        return;
    }

    curPos = 0;
    skipField (fieldNum);
}


// counts the number of lines in the file
int CSVFile::countLines (void)
{
	ifsstream::pos_type filePos = fpIn.tellg();
    char line[LARGE];

    reset();
//    fpIn.getline (line, LARGE);
	int count = 0;
	for ( count = 0; !eof(); count++)
        fpIn.getline (line, LARGE);

    fpIn.seekg (filePos);
    return count;
}


// returns the number of characters in the current field
int CSVFile::getFieldLength (void)
{
    int i = curPos;

    while (i < lineLength && dataLine[i] != COMMA)
        i++;

    return i - curPos;
}


int CSVFile::seekToRecord (const char *key, int fieldNum)
{
    char str[LARGE];

	setToField (fieldNum);
	getShortField (str, SMALL);
	if (!_stricmp (str, key))
        return TRUE;

	reset();
	while (getLine())
	{
        setToField (fieldNum);
        getShortField (str, LARGE);
		if (!_stricmp (str, key))
			return TRUE;
	}
	return FALSE;
}


// seeks to passed line number (1 based)
int CSVFile::seekToLine (int lineNum)
{
    reset();
    for (int i = 0; i < lineNum; i++)
        if (!getLine())
            return FALSE;

    return TRUE;
}


// Reads a single character from current position
// Returns FALSE if curPos is the end of a field
// Increments curPos to point to the start of the next field, assuming
// That the current field is 1 character long
int CSVFile::getChar (char& aChar)
{
    aChar = dataLine[curPos];
    curPos++;

    if (aChar == COMMA || curPos == lineLength)
        return FALSE;
    else
    {
        if (dataLine[curPos] == COMMA)
            curPos++;
        return TRUE;
    }
}


// simple test to determine if the current line is the first line of a
// multi-line entry, such as Processor or Passenger Description Tree
int CSVFile::isNewEntryLine (void) const
{
    return (dataLine[0] != COMMA && dataLine[0] != '\0');
}



// Returns a count of the number of fields in the file
// Based on the spreadSheet "comma separated" file format
int CSVFile::countColumns (void)
{
    // line has (columns - 1) commas
	int columns = 1;

	for (int j = 0; j < lineLength; j++)
    {
        if (dataLine[j] == COMMA)
            columns++;
    }
    return columns;
}


// Returns the current field number (zero based)
int CSVFile::getColumn (void)
{
    int i = curPos;
    int count = 0;

    while (--i >= 0 )
        if (dataLine[i] == COMMA)
            count++;

    return count;
}


// reads n bytes into str
// does not skip to next field
// returns TRUE if it was able to read n bytes
int CSVFile::getNLengthField (char *str, int n)
{
	int i = 0;

    while (dataLine[curPos] != COMMA && i < n && curPos < lineLength)
        str[i++] = dataLine[curPos++];
    return i == n;
}


// Reads a field ending with a comma, newline, or at maxlen
// Delimiting character is not appended
// CurPos is incremented to the next field
// Returns FALSE if the current field is blank
int CSVFile::getField (char *str, int maxlen)
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


// Reads a field ending with a comma, hyphen, newLine, semi-colon, or
// Opening or closing parentheses
// Delimiting character is not appended
// CurPos is incremented past the delimiting character
// Returns the delimiting character, or NULL if blank field
char CSVFile::getShortField (char *str, int maxlen)
{
	int i = 0;

    while (dataLine[curPos] != COMMA &&
           dataLine[curPos] != '\0' &&
//           dataLine[curPos] != HYPHEN &&
           dataLine[curPos] != SEMI_COLON &&
           dataLine[curPos] != OPEN_PAREN &&
           dataLine[curPos] != CLOSE_PAREN &&
           curPos < lineLength &&
           i < maxlen)
                str[i++] = dataLine[curPos++];

    if (dataLine[curPos] != COMMA &&
        dataLine[curPos] != '\0' &&
//        dataLine[curPos] != HYPHEN &&
        dataLine[curPos] != SEMI_COLON &&
        dataLine[curPos] != OPEN_PAREN &&
        dataLine[curPos] != CLOSE_PAREN &&
        i == maxlen && i)
	{
        str[i-1] = NULL;
		skipField (1);
	}
	else
	{
		str[i] = NULL;
		curPos++;
	}

    // returns the delimiting character
    return (dataLine[curPos-1])? dataLine[curPos-1]: '\1';
}


// Reads an integer from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CSVFile::getInteger (short& anInt)
{
	int i = 0;
	char str[SMALL];

	// create str from the digits in the field
    while (isNumeric (dataLine[curPos]))
		str[i++] = dataLine[curPos++];

	// convert the string into an integer
	str[i] = NULL;
	anInt = (short) atoi (str);
	curPos++;

	// returns count of digits
	return i;
}


// Reads an integer from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CSVFile::getInteger (int& anInt)
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


// Reads a long integer from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CSVFile::getInteger (long& anInt)
{
	int i = 0;
	char str[SMALL];

	// create str from the digits in the field
	while (isNumeric (dataLine[curPos]))
	str[i++] = dataLine[curPos++];

	// convert the string into an integer
	str[i] = NULL;
	anInt = atol (str);
	curPos++;

	// returns count of digits
	return i;
}


// Reads a float from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CSVFile::getFloat (float& aFloat)
{
	int i = 0;
    char str[SMALL];

    aFloat = 0.0;
    // create str from digits, decimal point, optional sign
    while (isNumeric (dataLine[curPos]))
        str[i++] = dataLine[curPos++];

    if (!i)
        return FALSE;

    // convert string into a float
    str[i] = NULL;
    aFloat = (float) atof (str);
    curPos++;

    return i;
}

// Reads a double from the currentPosition
// Increments curPos past the first non-digit character
// Returns FALSE if dataLine[curPos] is not a digit
int CSVFile::getFloat (double& aFloat)
{
	int i = 0;
    char str[SMALL];

    aFloat = 0.0;
    // create str from digits, decimal point, optional sign
    while (isNumeric (dataLine[curPos]))
        str[i++] = dataLine[curPos++];

    if (!i)
        return FALSE;

    // convert string into a float
    str[i] = NULL;
    aFloat = atof (str);
    curPos++;

    return i;
}
            
// flushes dataLine to output stream
int CSVFile::writeLine (void)
{
    dataLine[lineLength] = CR;
    fpOut.write (dataLine, lineLength+1);

    curPos = lineLength = 0;
    dataLine[curPos] = NULL;

    // return status of fpOut
    return TRUE;
}


// copies the passed field to the end of the line, ensuring it does not
// exceed the maximum line length
void CSVFile::appendField (const char *field)
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


// copies the passed field to the end of the line, ensuring it does not
// exceed the maximum line length
void CSVFile::appendValue (const char *field)
{
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


// removes current field from existence, moves all following columns
// one to the left
void CSVFile::deleteField (void)
{
    if (curPos == lineLength)
        return;

    int i = curPos;

    while (dataLine[i++] != COMMA && i < lineLength);

    if (i == lineLength)
        curPos--;

    strcpy (dataLine+curPos, dataLine+i);
    lineLength = strlen (dataLine);
}


// removes contents of current field
void CSVFile::deleteValue (void)
{
    if (curPos == lineLength)
        return;

    int i = curPos;

    while (dataLine[i] != COMMA && i < lineLength)
        i++;
    strcpy (dataLine+curPos, dataLine+i);
    lineLength = strlen (dataLine);
}


// assumes curPos is the start of a field
// field will be inserted previous to the current one
void CSVFile::insertField (const char *field)
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


// inserts field into the dataLine at the current position
void CSVFile::insertValue (const char *field)
{
    if (curPos == lineLength)
    {
        appendValue (field);
        return;
    }

    int size = strlen (field);
    if (size + lineLength >= LARGE)
        return;

    lineLength += size;

    char temp[LARGE];
    strcpy (temp, dataLine+curPos);

    memcpy (dataLine+curPos, field, size);
    curPos += size;
    strcpy (dataLine+curPos, temp);
}
 

// copies a single character field to the dataLine
void CSVFile::writeChar (char c)
{
    char string[2] = " ";
    if (c)
        string[0] = c;
    insertField (string);
}


// copies a int int field to the dataLine
void CSVFile::writeInt (short integer)
{
    char string[SMALL];
    _itoa (integer, string, 10);
    insertField (string);
}


// copies a int int field to the dataLine
void CSVFile::writeInt (int integer)
{
    char string[SMALL];
    _itoa (integer, string, 10);
    insertField (string);
}


// copies a long int field to the dataLine
void CSVFile::writeInt (long integer)
{
    char string[SMALL];
    _ltoa (integer, string, 10);
    insertField (string);
}


// copies a floating point field to the dataLine
void CSVFile::writeFloat (float aReal, int precision)
{
    char formatStr[32] = "%.";
    _itoa (precision, formatStr+2, 10);
    strcat (formatStr, "f");

    char tempStr[128];
    sprintf (tempStr, formatStr, aReal);
    insertField (tempStr);
}

// copies a floating point field to the dataLine
void CSVFile::writeDouble (double aReal, int precision)
{
	writeFloat( (float)aReal, precision );
}


void CSVFile::writeBlankLines (int lines)
{
    for (int i = 0; i < lines; i++)
        fpOut << '\n';
}

void CSVFile::writeBlankField (void)
{
    strcat (dataLine, ",");
    curPos = lineLength = strlen (dataLine);
}

void CSVFile::writeBlankFields (int fields)
{
    for (int i = 0; i < fields; i++)
        writeBlankField();
}
void CSVFile::InitDataFromString( const char* _sData )
{
	strcpy( dataLine, _sData );
	lineLength = strlen( _sData );
	curPos = 0;
	
}

