
#include "StdAfx.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <share.h>
//#include <ctype.h>
//#include <dos.h>

#include "fsstream.h"
#include "fileman.h"
#include "util.h"
#include <math.h>

#ifdef WINVER
#undef WINVER
#define WINVER 0x0500
#endif



inline
static long double old_factorial(unsigned n)
{
	if (!n)
        return 1;
	 unsigned i;
    long double f = 1;
    for (i=1; i<=n; i++)
        f *= i;
    return f;
}

long double factorial (unsigned n)
{
	static unsigned nLastValid = 6;
	static long double f[256] = { 1.0, 1.0, 2.0, 6.0, 24.0, 120.0, 720.0 }; //fill in only first nLastValid entries
	if(n>255)
		return old_factorial(n);
	while(nLastValid < n) {
		nLastValid++;
		f[nLastValid] = f[nLastValid-1]*nLastValid;
	}
	return f[n];
}

long double Pow (long double x, long double y)
{
    if ( x == 0 && y == 0 )
        return 1.0;
    else
        return powl (x,y);
}

int getReal (char *str, double *d)
{
	 int i, j, dotCount = 0, length = strlen (str);

	 while (length && isspace (str[length-1]))
		  length--;
	 str[length] = '\0';

	 for (i = 0; i < length; i++)
		  if (!isspace(str[i]))
				break;

	 if (i == length)
		  return FALSE;

	 if (str[i] == '-' || str[i] == '+')
		  i++;

	 for ( j = i; j < length; j++)
		  if (str[j] == '.')
		  {
				if (++dotCount > 1)
					 return FALSE;
		  }
		  else
				if (!isdigit (str[j]))
					 return FALSE;

	 *d = atof (str);
	 return TRUE;
}

int getDouble (char *str, double *f)
{
	 int i, j, dotCount = 0, length = strlen (str);

	 while (length && isspace (str[length-1]))
		  length--;
	 str[length] = '\0';

	 for (i = 0; i < length; i++)
		  if (!isspace(str[i]))
				break;

	 if (i == length)
		  return FALSE;

	 if (str[i] == '-' || str[i] == '+')
		  i++;

	 for ( j = i; j < length; j++)
		  if (str[j] == '.')
		  {
				if (++dotCount > 1)
					 return FALSE;
		  }
		  else
				if (!isdigit (str[j]))
					 return FALSE;

    *f = atof (str);
    return TRUE;
}

int isValidFileName (char *str)
{
    int len = strlen (str);
    int dots = 0;
    if (str[0] == '.')
        return FALSE;
    for (int i = 0; i < len; i++)
    {
        if (str[i] == '.')
            if (dots)
                return FALSE;
				else
            {
                ++dots;
                continue;
            }

        if (str[i] < 33)
            return FALSE;
        if (str[i] > 126)
            return FALSE;
        if (str[i] > 90 && str[i] < 94)
            return FALSE;
        if (str[i] > 57 && str[i] < 64)
            return FALSE;
        if (str[i] > 45 && str[i] < 48)
            return FALSE;
        if (str[i] > 41 && str[i] < 45)
            return FALSE;
		  if (str[i] == 34)
            return FALSE;
    }
    return TRUE;
}


void hugeMemCopy (char huge *dest, char huge *source, int count)
{
    for (int i = 0; i < count; i++)
        dest[i] = source[i];
}


int roundUp (float val, int base)
{
    int temp = 1 + (int)(val / base);
    return temp * base;
}


int roundDown (float val, int base)
{
    int temp = (int)(val / base);
    return temp * base;
}

int filecopy (char* destFileName, char* sourceFileName, int appendMode)
{
    char buf [128];
    unsigned count;
    FILE* src = _fsopen (sourceFileName, "rb", SH_DENYNO );
    FILE* dest;
    if (appendMode)
        dest = _fsopen (destFileName, "ab", SH_DENYNO );
    else
		  dest = _fsopen (destFileName, "wb", SH_DENYNO );
    if (!src || !dest)
        return 0;
    while ( (count = fread (buf, 1, sizeof(buf), src)) == sizeof(buf) )
        fwrite (buf, 1,sizeof(buf), dest);
    if (count)
		fwrite (buf, 1,count, dest);
    fclose (src);
    fclose (dest);
    return 1;
}

// rotate a point around a center for angle in rad
void pointRotate (int* _x, int* _y,float angle)
{
    int x = *_x;
    int y = *_y;

	 float cosa = cos(angle);
    float sina = sin(angle);

    *_x = (int)(cosa * x + sina * y);
    *_y = (int)(-sina * x + cosa * y);
}

// count # of lines in a file
int countLines (char* fileName)
{
	int count = 0;
	char c ;
	ifsstream f( fileName );
	if (!f.good())
		{
//		cout << "Can not open file " << fileName ;
		return(-1);
		}
	while ( !f.eof() )
	{
		f.get(c);
		if (c == '\n')
			count++;
	}
	f.close();
	return count;
}

long countRecords (char* fileName, unsigned recordLen)
{
	FileManager f;
	long count;
	long pos = f.FileSize(fileName);
	if( pos == -1l )
		{
//		 cout << "Can not open file " << fileName;
		 return 0 ;
		 }
	count = pos / recordLen;
//	if ( count*recordLen != pos )
//			cout << "File doesnot contain an even # of records" << '\n';
	return count;
}

const long  SECTOR_SIZE = 512;

long fileSize ( std::istream& f )
{
	f.seekg( 0L, std::istream::end );
	return static_cast<long>(f.tellg());
}

int operator > ( std::istream& f, char huge* buf )
{
	char huge* p = buf;
	long size = fileSize( f );
	long numSectors = size / SECTOR_SIZE;
	int remaining = (int) ( size - ( numSectors * SECTOR_SIZE ) ) ;
	f.seekg( 0L, std::ifstream::beg );
	for ( long i = 0; i < numSectors; i++, p += SECTOR_SIZE )
		{
		f.read( (char *)p , SECTOR_SIZE);
		if ( f.gcount() != SECTOR_SIZE )
			return 0;
		}
	if( remaining )
		{
		f.read( (char *)p , remaining );
		if ( f.gcount() != remaining)
			return 0;
		}
	return 1;
}

int writeStream ( std::ostream& f, char huge* buf, long size )
{
	char huge* p = buf;
	long numSectors = size / SECTOR_SIZE;
	int remaining = (int) ( size - ( numSectors * SECTOR_SIZE ) ) ;
	f.seekp( 0L, std::ostream::beg );
	for ( long i = 0; i < numSectors; i++, p += SECTOR_SIZE )
		{
		f.write( (char *)p , SECTOR_SIZE);
		}
	if( remaining )
		{
		f.write( (char *)p , remaining );
		}
	return 1;
}
/*tx
char *getSystemDate (void)
{
	struct date d;
	static char systemDate[32];
	getdate(&d);
	sprintf(systemDate, "%d/%d/%d", d.da_year, d.da_mon, d.da_day);
	return systemDate;
}
*//*
	unsigned long coreleft (void)
	{
		UINT unused;
		return (long)GetFreeSpace(unused);
	}
*/