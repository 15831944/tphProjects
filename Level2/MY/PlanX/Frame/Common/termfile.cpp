/*****
*
*   Class       ArctermFile
*   Author      Lincoln Felingham, Aviation Research Corporation
*   Date        November 16, 1992
*   Purpose     Provides a generic interface to input files for ARCTRM
*
*               Contains methods to correctly read each field from the
*               file, assigning all the appropriate variables and adding
*               to the StringDictionaries when needed
*
*****/

#include "termfile.h"
#include <dos.h>
#include <ctype.h>
#include <string.h>

//#include "ProbabilityDistribution.h"
//#include "inputs\procdist.h"
//#include "inputs\fltdist.h"

#include "point.h"
#include "exception.h"
#include "csvfile.h"
#include "elaptime.h"
#include "util.h"
#include "point2008.h"


// opens a CSV file, skipping header lines if OPEN operation
int ArctermFile::openFile (const char *p_filename, int p_op, float p_ver)
{
    trimSpaces = 0;
    if (!CSVFile::init (p_filename, p_op))
        throw new FileOpenError (p_filename);

    if (p_op == READ)
    {
        char string[1024];
        getField (string, 1024);
        if (eol() || isBlankField())
            m_fversion = p_ver;
        else
            getFloat (m_fversion);

        if (!getLine() || !getLine())
            return FALSE;
    }

    if (p_op == WRITE)
    {
        writeField (p_filename);
        writeFloat (m_fversion = p_ver, 2);
        writeLine();
    }

    return TRUE;
}

int ArctermFile::openAirsideFile (const char *p_filename, int p_op, int p_ver)
{
	trimSpaces = 0;
	if (!CSVFile::init (p_filename, p_op))
		throw new FileOpenError (p_filename);

	if (p_op == READ)
	{
		char string[1024];
		getField (string, 1024);
		if (eol() || isBlankField())
			m_fversion = static_cast<float>(p_ver);
		else
		{
			int nver = 0;
			getInteger(nver);
			m_fversion = static_cast<float>(nver);
		}
		if (!getLine() || !getLine())
			return FALSE;
	}

	if (p_op == WRITE)
	{
		writeField (p_filename);
		m_fversion = static_cast<float>(p_ver);
		writeInt(p_ver);
		writeLine();
	}

	return TRUE;
}
// Reads the next line from the file into dataLine
// Returns true if successful, false if EOF, -1 if blank line
int ArctermFile::getLine (void)
{
    if (CSVFile::getLine() == -1)
        return FALSE;

    if (eof() || dataLine[0] == NULL)
        return FALSE;

    format();
    return TRUE;
}

// sets all characters to uppercase and removes any blanks
void ArctermFile::format (void)
{
    int source, dest, length = strlen (dataLine);

    for (source = dest = 0; source <= length; source++, dest++)
    {
        if (trimSpaces)
            while (dataLine[source] == SPACE)
                source++;
        dataLine[dest] = (char) toupper (dataLine[source]);
    }
    dataLine[dest] = '\0';
}

// returns number of fields in the current one delimited by separator
short ArctermFile::countSubFields (char separator)
{
    if (dataLine[curPos] == COMMA || curPos >= lineLength)
        return 0;

    short delimiterCount = 1;
    for (int i = curPos; i < lineLength && dataLine[i] != COMMA; i++)
        if (dataLine[i] == separator)
            delimiterCount++;
    return delimiterCount;
}

// copies a sub-field (delimited by separator) to str
short ArctermFile::getSubField (char *subField, char separator)
{
	short i = 0;

    while (curPos < lineLength &&
           dataLine[curPos] != COMMA &&
           dataLine[curPos] != separator)
        subField[i++] = dataLine[curPos++];

    subField[i] = NULL;
    curPos++;

    // returns count of characters in field
    return i;
}

// returns TRUE if field == ALLPROCESSORS
int ArctermFile::allProcs (void)
{
    int temp = curPos;
    char str[SMALL];

    if (!getField (str, SMALL))
        return TRUE;

    curPos = temp;
    return !strcmp (str, "ALLPROCESSORS");
}

// reads an (X, Y, Z) 3D float coordinate and translates it into cm
int ArctermFile::getPoint (Point& aPoint)
{
    float coord[3] = {0.0, 0.0, 0.0};

    if (isBlankField())
    {
        curPos += COLUMNS;
        return FALSE;
    }
    else
    {
        getFloat (coord[0]);
        getFloat (coord[1]);

        if (isBlankField())
            curPos++;
        else
            getFloat (coord[2]);

        aPoint.init ((double) (coord[0] * SCALE_FACTOR),
                     (double) (coord[1] * SCALE_FACTOR),
                     (double) (coord[2] * SCALE_FACTOR));

        return TRUE;
    }
}

// Reads a coordinate from a file, scaling it appropriately
int ArctermFile::getPoint2008 (CPoint2008& aPoint)
{
	float coord[3] = {0.0, 0.0, 0.0};

	if (isBlankField())
	{
		curPos += COLUMNS;
		return FALSE;
	}
	else
	{
		getFloat (coord[0]);
		getFloat (coord[1]);

		if (isBlankField())
			curPos++;
		else
			getFloat (coord[2]);

		aPoint.init ((double) (coord[0] * SCALE_FACTOR),
			(double) (coord[1] * SCALE_FACTOR),
			(double) (coord[2] * SCALE_FACTOR));

		return TRUE;
	}
}

// converts aPoint from cm to m and writes it to the file
void ArctermFile::writePoint (const Point& aPoint)
{
    double xCoord, yCoord, zCoord;
    aPoint.getPoint (xCoord, yCoord, zCoord);

    writeFloat ((float)(xCoord / SCALE_FACTOR), 2);
    writeFloat ((float)(yCoord / SCALE_FACTOR), 2);
    writeFloat ((float)(zCoord / SCALE_FACTOR), 2);
}

// Reads a coordinate from a file, scaling it appropriately
void ArctermFile::writePoint2008(const CPoint2008& aPoint)
{
	double xCoord, yCoord, zCoord;
	aPoint.getPoint (xCoord, yCoord, zCoord);

	writeFloat ((float)(xCoord / SCALE_FACTOR), 2);
	writeFloat ((float)(yCoord / SCALE_FACTOR), 2);
	writeFloat ((float)(zCoord / SCALE_FACTOR), 2);
}

// Reads a time into the passed ElapsedTime instance
// if readSeconds is TRUE, method will attempt to read seconds field
int ArctermFile::getTime (ElapsedTime& time, int readSeconds)
{
	char str[25];
	if( !getField( str, 25 ) )
		return FALSE;

	return time.SetTimeByString( str );
}

// Writes the passed ElapsedTime to the next Field
int ArctermFile::writeTime (const ElapsedTime& time, int writeSeconds)
{
    char timeString[20];
	ElapsedTime etSafeTime=time;
//	if(time<0L)
//	{
//		etSafeTime+=WholeDay;
//	}
    etSafeTime.PrintTime (timeString, writeSeconds);
    writeField (timeString);
    return TRUE;
}

// writes blank line and Date and Time stamp
void ArctermFile::endFile (void)
{
    blankLine();

	COleDateTime sysDate = COleDateTime::GetCurrentTime();

    char dateString[64];
	sprintf( dateString, "%d/%02d/%d,%d:%d:%d",
				sysDate.GetMonth(),
				sysDate.GetDay(),
				sysDate.GetYear(),
				sysDate.GetHour(),
				sysDate.GetMinute(),
				sysDate.GetSecond() );

    writeField (dateString);
    writeLine();
    closeOut();
}

ArctermFile::ArctermFile()
{
	m_fversion = 0.0; trimSpaces = 0;
}

//#if 0
//// Reads a probability distribution into the appropriate subclass of
//// ProbabilityDistribution and returns a generic pointer to it
//// Currently only handles historical (discrete) and uniform
//// Also assumes that the current position is valid
//ProbabilityDistribution *ArctermFile::getProbDistribution (void)
//{
//    char str[SMALL];
//
//    // get ProbabiltyDistribution field
//    getShortField (str, SMALL);
//    if (!stricmp (str, "HISTORICAL") || !stricmp (str, "DISCRETE") ||
//        !stricmp (str, "HISTOGRAM"))
//    {
//        // initialize instance
//        HistogramDistribution *historical = new HistogramDistribution;
//        if (!historical->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) historical;
//    }
//    else if (!stricmp (str, "EMPIRICAL"))
//    {
//        // initialize instance
//        EmpiricalDistribution *empirical = new EmpiricalDistribution;
//        if (!empirical->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) empirical;
//    }
//    else if (!stricmp (str, "BERNOULLI"))
//    {
//        int tempPos = curPos;
//        BernoulliDistribution temp;
//        if (!temp.readDistribution (*this))
//        {
//            curPos = tempPos;
//            EmpiricalDistribution *empirical = new EmpiricalDistribution;
//            if (!empirical->readDistribution (*this))
//                return NULL;
//            return (ProbabilityDistribution *) empirical;
//        }
//
//        BernoulliDistribution *bernoulli= new BernoulliDistribution;
//        bernoulli->init (temp.getValue1(), temp.getValue2(), temp.getProb1());
//        return (ProbabilityDistribution *) bernoulli;
//    }
//    else if (!stricmp (str, "UNIFORM"))
//    {
//        // initialize instance
//        UniformDistribution *uniform = new UniformDistribution;
//        if (!uniform->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) uniform;
//    }
//    else if (!stricmp (str, "BETA"))
//    {
//        BetaDistribution *betaDist = new BetaDistribution;
//        if (!betaDist->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) betaDist;
//    }
//    else if (!stricmp (str, "CONSTANT"))
//    {
//        ConstantDistribution *constant = new ConstantDistribution;
//        if (!constant->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) constant;
//    }
//    else if (!stricmp (str, "NORMAL") || !stricmp (str, "GAUSSIAN"))
//    {
//        NormalDistribution *normal = new NormalDistribution;
//        if (!normal->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) normal;
//    }
//    else if (!stricmp (str, "EXPONENTIAL"))
//    {
//        ExponentialDistribution *exponential = new ExponentialDistribution;
//        if (!exponential->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) exponential;
//    }
//    else if (!stricmp (str, "PROCESSOR"))
//    {
//        ProcessorDistribution *processor = new ProcessorDistribution;
//        if (!processor->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) processor;
//    }
//    else if (!stricmp (str, "FLIGHT_TYPE"))
//    {
//        FlightTypeDistribution *flightType = new FlightTypeDistribution;
//        if (!flightType->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) flightType;
//    }
//    else if (!stricmp (str, "PROCESSORWITHPIPE"))
//    {
//        CProcessorDistributionWithPipe *processor = new CProcessorDistributionWithPipe;
//        if (!processor->readDistribution (*this))
//            return NULL;
//        return (ProbabilityDistribution *) processor;
//    }
//    // other probability types yet to be written
//    else
//        throw new TwoStringError ("Unknown Probability Distribution ", str);
//}
//#endif
//
